#include <windows.h>
#include "C67lib.h"
#include <stdio.h>
#include "Target.h"

extern "C";

CARDINFO* dsp = NULL;

//20060902  float	MaxMicrons[NUMDAC] = {39.719F,39.694F,39.620F,100.F};	// per JWS
//20060902  float	MinMicrons[NUMDAC] = {0.,0.,0.,0.};
//float	MicronsPerADU[NUMDAC] = {5.9029E-4F,5.89606E-4F,5.88924E-4F,15.259E-4F}; //per JWS
float	MicronsPerADU[NUMDAC] = {5.9029E-4F,5.89606E-4F,5.88924E-4F,15.259E-4F}; //z-fixed:2004/0519
// >>> ma = U.arrF(39.719,39.694,39.620,100.)
// >>> mpa = U.arrF(5.9029E-4,5.89606E-4,5.88924E-4,15.259E-4)
// >>> ma / mpa  # maxMicron/umPerADU = maxADU
// [[ 67287.265625  67322.921875  67275.234375  65535.09375 ]]

//20060829  float	UpperLimit[NUMDAC];
//20060829  float	LowerLimit[NUMDAC];

ProStr profileHdr;
void *FrameData = NULL;
void *framePtr;
ContextStr	myContext;



//20060829  unsigned int *DioPtr = NULL;
//20060829  float *AmpPtr[NUMDAC];
//20060829  unsigned int	*DlyPtr = NULL;

timeValuePair *digitalList = NULL;
timeValuePair *analogList[NUMDAC];
float	Base[NUMDAC];



HANDLE	C67InternalEvent= NULL;
//HANDLE	C67UserEvent = NULL;

//20060902 int		MappedDMA=FALSE;
volatile int	collecting=FALSE;		// collecting data
volatile int	loaded=FALSE;			// profile loaded
volatile int	downloaded = FALSE;		// profile downloaded to target
volatile int	armed=FALSE;			// profile Initialized

char	ErrStr[128];


int IsCollecting( )
{
	return collecting;
}


float read_mailbox_float(int target, int box)
{
		MailboxValue Value;

		Value.x = read_mailbox(target,box);
		return Value.f;
}

void write_mailbox_float(int target,  int box, float value)
{
		MailboxValue Value;

		Value.f=value;
		write_mailbox(target, box, Value.x);
}

void	WriteCommand(int WhatToDo)
{
		mailbox_interrupt(TARGET,WhatToDo);
		int rply = ReadReply();
		sprintf(ErrStr,"WriteCommand 0x%x Read 0x%x\n",WhatToDo,rply);
		DebugPrint(ErrStr);
		if(rply != WhatToDo) {
		  sprintf(ErrStr,"WriteCommand got BAD ECHO: Wrote 0x%x Read 0x%x",WhatToDo,rply);
		  throw (char*)ErrStr;
		}
}

void	WriteParam(int param)
{
		write_mailbox(TARGET,0,param);
		int rply = ReadReply();
		sprintf(ErrStr,"WriteParam 0x%x Read 0x%x\n",param,rply);
		DebugPrint(ErrStr);
		if(rply != param) {
		  sprintf(ErrStr,"WriteParam 0x%x BUT reply read: 0x%x\n",param,rply);
		  throw (char*)ErrStr;
		}
}

int		ReadReply(int timeoutMS)
{		
  //seb int count=10;  
  while((!check_inbox(0, 0)) && timeoutMS)
	{
	  Sleep(1); // seb Sleep(200);
	  timeoutMS -= 1;
	}
  
  if(timeoutMS)
	return read_mailbox(TARGET,0);
  else
	//seb return TIMO;
	throw (char*)"timeout in DSP C67lib.cpp:ReadReply() [read_mailbox]";
}

void C67Close()
{
		// todo
		// release allocated memory
		FreeBuffers();
		// free any allocated buffers
		free(FrameData);
		FrameData = NULL; // seb 20041106
		// free events
		if(C67InternalEvent != NULL) {
			CloseHandle(C67InternalEvent);
			C67InternalEvent = NULL;  // seb 20041106
		}
		// deal with target interrupts
		// close target
		target_reset(TARGET);
		target_close(TARGET);
}

void C67Open(char *executive)
{	
	int i;
	int	rtn;

	// Initialize library statics here
	for(i=0;i<NUMDAC;i++)
	  {
		Base[i]=0.;
		//20060829  		UpperLimit[i]=MaxMicrons[i];
		//20060829  		LowerLimit[i]=MinMicrons[i];
		//20060829  	
		analogList[i] = NULL;
	  }

	printf(" target opening... \n");
	if(!target_open(TARGET))
	  throw (char*) "target_open(TARGET) failed";
		//return FALSE;
	  

	/* target successfully opened: perform accesses */
	//20060920 DebugPrint
	printf(" target opened - target_reset ...\n");
	/* finished accessing target */

    target_reset(TARGET);
	printf(" target reseted - target_run ...\n");
    target_run(TARGET);
	printf(" done -- starting talker ...\n");

    /* start talker program on target */
    if(start_talker(TARGET))
	  {	
		//20060920 DebugPrint
		printf("started talker -- downloading COFF program...\n");
		/* perform COFF program download */
		if(!(iicoffld(executive,TARGET, NULL)))
		  {
			printf("downloaded. - starting app...\n");
			/* start application on target */
			start_app(TARGET);
			//20060920 DebugPrint
			printf("app started.  (now readReplay and initDMA.)\n");
		  }
		else
		  {
			throw (char*)"C67Open:failed to load application";
			//PrintError("C67Open:failed to load application");
			//return FALSE;
		  }
	  }
	else
	{
	  throw (char*)"C67Open:Could not start talker";
		//PrintError("Could not start talker");
		//return FALSE;
	}

	rtn=ReadReply(100);//int timeoutMS)
	if ( rtn != READY) {
	  sprintf(ErrStr,"C67Open does not get READY (0x%x)", rtn);
	  throw (char*)ErrStr;
	}
	DebugPrint("READBACK %x\n",rtn);

	// map busmaster address
	InitDMA();
	  //return FALSE;
	//return TRUE;
}
	
unsigned int	*InitDMA()
{
		int rtn;
		DebugPrint("In InitDMA stub");

		dsp = (CARDINFO *)target_cardinfo(TARGET);
		if(!dsp)
		  throw (char*)"C67lib-InitDMA target_cardinfo() failed";

		DebugPrint(" dsp: 0x%x\n",dsp);
		WriteCommand(INIT_DMA);
		WriteParam(dsp->BusMaster.PhysAddr);

		if((rtn=ReadReply()) != DONE)
		  {
			sprintf(ErrStr,"C67lib-InitDMA reply 0x%x !=DONE", rtn);
			throw (char*)ErrStr;
		  }

//20060902		MappedDMA = TRUE;
		return (unsigned int *)dsp->BusMaster.Addr;
}


void	PrintCardInfo()
{
		if(dsp == NULL)
		{
			PrintError(" Cardinfo not initialized ");
			return;
		}
		printf("dsp = 0x%x\n",dsp);

		printf("CARDINFO reports 0x%x\n",dsp->Target);
		printf("DDHandle: 0x%x\n",dsp->Device);
		printf("DLL Version: 0x%x\n",dsp->Info.DLL_Version);
		printf("Busmaster pmem address: 0x%x\n",dsp->BusMaster.PhysAddr);
		printf("Busmaster vmem address: 0x%x\n",dsp->BusMaster.Addr);
		printf("Busmaster linearmem address: 0x%x\n",dsp->BusMaster.DriverAddr);
		printf("Busmaster pmem size: %d\n",dsp->BusMaster.Size);
		printf("Target mem size: %d\n",dsp->Info.CellSize);
}

void PrintError(char *str)
{
	printf("%s\n",str);
}

float	ReadPosition(int axis)
{
	int position;

	if(collecting) { throw (char*)"DSP is still collecting"; } // seb
	DebugPrint("In ReadPosition stub\n");

	//WriteCommand(READ_POSITION);
	//WriteParam(axis);
	int com=READ_POSITION_0+axis;
	WriteCommand( com );

	DebugPrint("Waiting...\n");
	position=ReadReply();
	return ADUToMicron(axis,position);
}

float	ReadActual(int axis)
{
	int	position;

	if(collecting) { throw (char*)"DSP is still collecting"; } // seb
	DebugPrint("In ReadActual stub\n");
// 	WriteCommand(READ_ACTUAL);
// 	WriteParam(axis);
	int com=READ_ACTUAL_0+axis;
	WriteCommand( com  );
	position = ReadReply();
	return ADUToMicron(axis,position);
}

void WriteShutter(int value)
{
		int rtn;

		if(collecting) { throw (char*)"DSP is still collecting"; } // seb
		DebugPrint("In WriteShutter stub\n");
		WriteCommand(WRITE_SHUTTER);
		//WriteParam(value&0x7f); // seb: 2004/07/07
		// WriteParam(value&0xff);   // now we have DIC shutter on bit 15
		// //seb 20041012 now Laser405 is bit 7
		WriteParam(value&0xff80);   // seb 20041012  now  CAM_OFFSET  = 0  !!! not 8
		if((rtn=ReadReply()) != DONE)
			throw (char*)"WriteShutter: incorrect reply";
}

int	ReadShutter()
{

		if(collecting) { throw (char*)"DSP is still collecting"; } // seb
		DebugPrint("In ReadShutter stub\n");
		WriteCommand(READ_SHUTTER);
		return ReadReply();
}

void TriggerSpec()
{
  int rtn;
  if(collecting) { throw (char*)"DSP is still collecting"; } // seb
  DebugPrint("In TriggerSpec stub\n");
  WriteCommand(TRIGGER_SPEC);
  if((rtn=ReadReply()) != DONE)
	throw (char*)"TriggerSpec: incorrect reply";
}

//20060829  use SetBase 
//20060902 now limit checks done on DSP
/*
void SetBaseAbsolute(int	axis, float value)
{
	int limit=FALSE;
	DebugPrint("In SetBaseAbsolute stub\n");

	// if collecting must take care 
	if(collecting)
	{
		// compare limits and adjust
		// want to go to maximum possible  
		// return flag if limited 
		if (value > UpperLimit[axis])
		{
			limit=TRUE;
			SetBase(axis,UpperLimit[axis]);
		}
		else if(value < LowerLimit[axis])
		{
			limit=TRUE;
			SetBase(axis,LowerLimit[axis]);
		}
		else
		  SetBase(axis,value);
	}
	else	// doesn't matter here
		SetBase(axis,value);
	if(limit)
	  throw (char*)"SetBaseAbsolute would go beyond range limits";
}
*/
void SetBase(int axis, float value)
{
	int rtn;
	DebugPrint("In SetBase stub\n");
	int p=MicronToADU(axis,value);
	int com=SET_BASE_0+axis;
	WriteCommand( com | (p<<8) );
	
	//WriteCommand(SET_BASE);
	//WriteParam(axis);
	//WriteParam(MicronToADU(axis,value));
	

	Base[axis]=value;
	if((rtn=ReadReply()) != DONE)
	  {
		if(rtn == 0xffeedd01)
		  throw (char*)"SetBase min limited";
		if(rtn == 0xffeedd02)
		  throw (char*)"SetBase max limited";

		sprintf(ErrStr,"SetBase reply 0x%x !=DONE", rtn);
		throw (char*)ErrStr;
	  }
}


//CHECK //20060829  
/*
void SetAmplitude(int axis, float ampM)
{
	int rtn;
	float	testULim,testLLim;
	
	DebugPrint("In SetAmplitude stub: %f\n",ampM);

	if(collecting) throw (char*)"DSP is still collecting"; // seb: we can't test armed - because SetAmplitude called INSIDE InitProfile || !armed)

	if((ampM > ADUToMicron(axis,MAXDAC)) || (ampM < MinMicrons[axis]))
	  	  throw (char*)"SetAmplitude would be larger than range limits";
	// test new amplitude to see if it makes base illegal
	testULim = MaxMicrons[axis]-ampM*profileHdr.ULim[axis];
	testLLim = MinMicrons[axis]+ampM*profileHdr.LLim[axis];
	if((Base[axis] > testULim) || (Base[axis] < testLLim))
	  	  throw (char*)"SetAmplitude would go beyond range limits";

	// else all is well and we can proceed
	// 	WriteCommand(SET_AMPLITUDE);
	// 	WriteParam(axis);
	// 	WriteParam(MicronToADU(axis,ampM));
	int a=MicronToADU(axis,ampM);
//20060829  	int com=SET_AMPLITUDE_0+axis;
//20060829  	WriteCommand( com | (a<<8 16 ?? ) );
	
	if((rtn=ReadReply()) != DONE)
	  throw (char*)"SetAmplitude: incorrect reply";
	// Fix up limits
	UpperLimit[axis]= testULim;
	LowerLimit[axis]= testLLim;
	DebugPrint("Set limits upper: %f lower: %f\n",UpperLimit[axis],LowerLimit[axis]);
}
*/

void MoveAbsolute(int axis, float target)
{
	int rtn;
	if(collecting) { throw (char*)"DSP is still collecting"; } // seb
	DebugPrint("In MoveAbsolute stub\n");

	if((target > ADUToMicron(axis,MAXDAC)) || (target < 0))
	  throw (char*)"MoveAbsolute would go beyond range limits";

	// 	WriteCommand(MOVE_ABSOLUTE);
	// 	WriteParam(axis);
	// 	WriteParam(MicronToADU(axis,target));
	int p=MicronToADU(axis,target);
	int com=MOVE_ABSOLUTE_0+axis;
	WriteCommand( com | (p<<8) );

	if((rtn=ReadReply()) != DONE)
		throw (char*)"MoveAbsolute: incorrect reply";
}

void MoveRelative(int axis, float delta)
{
	float current;
	if(collecting) { throw (char*)"DSP is still collecting"; } // seb
	DebugPrint("In MoveRelative stub\n");

	current = ReadPosition(axis);
	DebugPrint("Current position %f\n",current);
	MoveAbsolute(axis, current+delta);
}

void Expose(int CamMask)
{
	int rtn;
	if(collecting) { throw (char*)"DSP is still collecting"; } // seb
	DebugPrint("In Expose stub\n");

	WriteCommand(EXPOSE);
	WriteParam(CamMask);
	if((rtn=ReadReply()) != DONE)
	  throw (char*)"Expose: incorrect reply";
}

void Flash()
{
	int rtn;
	if(collecting) { throw (char*)"DSP is still collecting"; } // seb
	DebugPrint("In Flash stub\n");

	WriteCommand(FLASH);
	if((rtn=ReadReply()) != DONE)
	  throw (char*)"Flash: incorrect reply";
}

unsigned int	ReadDigital()
{
  if(collecting) { throw (char*)"DSP is still collecting"; } // seb
  DebugPrint("In ReadDigital stub\n");

	WriteCommand(READ_DIGITAL);
	return ReadReply();
}

void WriteDigital(int value)
{
	int rtn;
	if(collecting) { throw (char*)"DSP is still collecting"; } // seb
	DebugPrint("In WriteDigital stub\n");

	WriteCommand(WRITE_DIGITAL);
	WriteParam(value);
	if((rtn=ReadReply()) != DONE)
	  throw (char*)"WriteDigital: incorrect reply";
}

void Abort()
{
		DebugPrint("In Abort stub\n");
	// todo
	// clear flags
	WriteCommand(ABORT);
	// call CollectionCleanup()
	CollectionCleanup();
}

void UpdateNReps(int newCount)
{
  DebugPrint("In UpdateNReps stub\n");
  WriteCommand( UPDATE_NREPS | (newCount<<8) );
}

void	PostStatus(void *ssptr)
{
//	int rtn;
		DebugPrint("In PostStatus stub\n");
	// todo
	// is dma inited?
	WriteCommand(POST_STATUS);
	// read data back in mailbox
	//	 reps
	//	step
	//  adc[i]
	//  dac[i]
	//  dio
	// ??? status flags???
	//wait for done
	// host globals
	// collecting, loaded, min max base home ...
	//	format and print status structure
	//rtn=ReadReply();
}

void	ReturnFrameData(void *userbuf)
{	// convert to microns and copy data to user buffer
	// free report bffer

	int	i;
	Xframe *src = (Xframe *)FrameData;
	Fframe *dest = (Fframe *)userbuf;

	if(!src || !dest) { //seb 20041113
	  throw (char*)"ReturnFrameData: FrameData or userbuf == NULL";
	  //PrintError("ReturnFrameData: FrameData or userbuf == NULL");
	  //return ;
	}
	// todo
	for(i=0;i<(int)myContext.count ;i++)
	{
		XframeToFframe((void *)src,(void *)dest);
		src++;
		dest++;
	}
	free(FrameData);
	FrameData = NULL;
}

void CollectionISR(void *p)
{
	ContextStr *cptr=(ContextStr *)p;
	// switch on int mailbox contents
	switch(mailbox_interrupt_ack(TARGET))
	{
	case NEXT:		{
						cptr->count += 1;
						if(cptr->single)
						{	if(cptr->usrEvent == NULL)
									break;	// do nothing
							else
								//copy data 
								XframeToFframe((void *)cptr->dmasrc ,(void *)cptr->fptr );
								//and set event
								SetEvent(cptr->usrEvent);
								break;
						}
						else
						{

							// copy data and increment pointer iff space allocated
							if(cptr->fptr != NULL)
							{
								memcpy((void *)cptr->fptr ,(const void *)cptr->dmasrc ,16);
								(cptr->fptr)++;
							}
							break;
						}
					break;		// 
					}//end NEXT
	case DONE:		// set cptr->myEvent;
	  //printf("sebDebug: CollectionISR DONE\n");//20050407
	  SetEvent(cptr->myEvent);
	  break;
	}

	//	copy dma data into report buffer
	//		if report buffer full allocate next one
	//		increment frame count
	//	case DONE:
	//		fire internal event
	//	rtn
	
}

void	XframeToFframe(void *in, void *out)
{
	Xframe *from = (Xframe *)in;
	Fframe *to = (Fframe *)out;

	to->rep = from->rep ;
	to->step = from->step ;
	to->adc0 = ADUToMicron(0,(unsigned)(from->pair0 & 0xffff));
	to->adc1 = ADUToMicron(1,(unsigned)((from->pair0 >> 16) &0xffff));
	to->adc2 = ADUToMicron(2,(unsigned)(from->pair1 & 0xffff));
	to->adc3 = ADUToMicron(3,(unsigned)((from->pair1 >> 16) &0xffff));
}

int	Collect(doneCallbackFct *doneCallback)
{	
	DWORD waitrtn;

	DebugPrint("in collect stub\n");
	if(!armed)
	  throw (char*)"Collect: Profile not Initialized yet";
	if(C67InternalEvent == NULL)   // seb 20041106
		throw (char*)"Collect: C67InternalEvent == NULL";

	collecting = TRUE;
	try {
	  ResetEvent(C67InternalEvent);
	  //printf("sebDebug: Collect-go now\n");//20050407
	  WriteCommand(COLLECT);
	  // todo
	  // sleep on InternalEvent;
	  waitrtn=WaitForSingleObject(C67InternalEvent,INFINITE);
	  //printf("sebDebug: Collect-WaitForSingleObject returned\n");//20050407
	} catch(...) {
	  collecting = FALSE;
	  throw;
	}
	DebugPrint("Collect: DataCollection finished\n");
	switch(waitrtn)
	  {
	  case WAIT_ABANDONED:			DebugPrint("Wait returns WAIT_ABANDONED %d\n",waitrtn);
		break;	
	  case WAIT_OBJECT_0:				DebugPrint("Wait Returns WAIT_OBJECT_0 %d \n",waitrtn);
		break;
	  case WAIT_TIMEOUT:				DebugPrint("Wait Returns WAIT_TIMEOUT %d \n", waitrtn);
		break;
	  case WAIT_FAILED:				DebugPrint(" Wait Returns WAIT_FAILED %d \n", waitrtn);
		break;
	  default:						DebugPrint(" Wait Returns UNKNOWN %d \n",waitrtn);
	  }
	  
	  CollectionCleanup();
	  // return frame count;
	  
	//seb	printf("sebdebug: %p\n", doneCallback);
	if(doneCallback) doneCallback(waitrtn);

	return GetFrameCount();
}

void  *InitProfile(int reps, HANDLE usrEvent)
{
	int rtn;
	int	i,total;
	int high;
	int nsamples;
	int index;

	DebugPrint("In InitProfile stub\n");

	if(collecting)    throw (char*)"ERROR: DSP is still collecting"; 
	if(!downloaded)	  throw (char*)"InitProfile: DSP profile not downloaded yet";
	if(reps < 0)	  throw (char*)"InitProfile: Illegal repitition count <0";

	if(FrameData != NULL) {	// previously used and not read
		free(FrameData);
		FrameData = NULL; //20040803  
	}

	WriteCommand(INIT_PROFILE);
	WriteParam(reps);
	rtn = ReadReply();
	if(rtn != READY)
	  {
		sprintf(ErrStr,"InitProfile error: 0x%x ",rtn);
		throw (char*)ErrStr;
	  }
	
	sprintf(ErrStr,"read back %x\n",rtn);
	DebugPrint(ErrStr);
	//todo
	// loaded = TRUE; // set in LoadProfile()
	/// allocate internal event	
	if((C67InternalEvent=CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) //unnamed, auto reset, cleared event.
	{
	  throw (char*)"InitProfile: could not create internal event";
	}
	// set base[i]
		// read dacs and set 
	for(i=0;i<NUMDAC;i++)
	{
	  SetBase(i,ReadPosition(i));
	  //20060902 now limit checks done on DSP  (see setbase)

	  //20060829  		SetAmplitude(i,0.0);	// SetAmplitude must be called explicitly
	  //20060829  		if((Base[i] > UpperLimit[i]) || (Base[i] < LowerLimit[i]))
	  //20060829  		{ 
	  //20060829  		  sprintf(ErrStr," Base[%d] out of limits\n Upperlimit:%f \n Lowerlimit:%f\n Base:%f\n",
	  //20060829  				  i,UpperLimit[i],LowerLimit[i],Base[i]);
	  //20060829  		  throw (char*)ErrStr;
	  //20060829  		}
	  // sets upper and lower limits
	  // same as amplitudes
	}
	// enable host isr
	host_interrupt_install(TARGET, CollectionISR,(void *) &myContext);
	DebugPrint("Addr myContext: 0x%x\n",&myContext);
	host_interrupt_enable(TARGET);

	// allocate report buffer for reps count uless reps = -1 then allocate first one
	// find out how large. 
	if(0) // HACK FIXME (reps > 0)  
	  {
		nsamples = 0;
		// initially high could transition at clock zero
		high = (profileHdr.InitDio & (1<<ADC_OFFSET));
		for(index=0;index < profileHdr.nDigital; )
		  {
			if(high)
			  {
				while((digitalList[index].val & (1<<ADC_OFFSET)) 
					  && (index < profileHdr.nDigital ))
				  index += 1;
				nsamples += 1; // negative transition triggers ADC
				DebugPrint("%d  %d\n",index,nsamples);
				high=FALSE;
			  }	
			while(!(digitalList[index].val & (1<<ADC_OFFSET)) 
				  && (index < profileHdr.nDigital ))
			  index += 1;
			high = TRUE;	// gone hi again
		  }
		// total number of samples in data collection
		if((total=nsamples*reps) == 0)
		  DebugPrint(" No ADC samples taken\n"); 
		else
		  {// allocate space
			DebugPrint("Allocating for %d samples\n",total);
			if((FrameData=(void *)malloc(total*sizeof(Xframe))) == NULL)
			  {
				CollectionCleanup();
				throw (char*)"InitProfile: Could not allocate FrameData Buffer";
			  }
		  }
	  }
	else	// don't try to allocate infinate memory
	  {
		total=1;
		// allocate space
		DebugPrint("Allocating for %d sample\n",total);
		if((FrameData=(void *)malloc(total*sizeof(Fframe))) == NULL)
		  {
			CollectionCleanup();
			throw (char*)"InitProfile: Could not alloccate Frame Data Buffer";
		  }
	  }
	framePtr=FrameData;
	// create context structure for ISR??
	myContext.count =0;
	if(reps < 0) 
	  myContext.single=TRUE;
	else 
	  myContext.single=FALSE;
	myContext.usrEvent=usrEvent;
	myContext.myEvent=C67InternalEvent;
	myContext.fptr=(Xframe *)framePtr;  //sebC++
	myContext.dmasrc = (int *)dsp->BusMaster.Addr;   //sebC++
	// set armed
	armed = TRUE;
	return FrameData;
}

int	GetFrameCount()
{
	return myContext.count ;
}

void	CollectionCleanup()
{
  //todo ???
  // disablehost isr
  host_interrupt_disable(TARGET);
  // collecting = FALSE;
  collecting=FALSE;
  // armed = FALSE;
  armed=FALSE;
  // free internal event
  if(C67InternalEvent != NULL) // seb 20041106
	{
	  CloseHandle(C67InternalEvent);
	  C67InternalEvent = NULL;  // seb 20041106
	}
}



void _PutHdr( ProStr *profileHdr)
{
	int  i;
	 
	WriteParam(profileHdr->count );
//	if ((rtn=ReadReply()) != (int) profileHdr->count)
//		printf("Error wrote 0x%x read 0x%x\n",profileHdr->count,rtn);
	write_mailbox_float(0,0,profileHdr->clock );
	//	if((frtn=read_mailbox_float(0,0)) != profileHdr->clock )
	sprintf(ErrStr," wrote %f read %f\n", profileHdr->clock ,
			read_mailbox_float(0,0));
	DebugPrint(ErrStr);
	WriteParam(profileHdr->InitDio );
//	if ((rtn=ReadReply()) != (int)profileHdr->InitDio)
//		printf("Error wrote 0x%x read 0x%x\n",profileHdr->InitDio ,rtn);
 	WriteParam(profileHdr->nDigital);
	for (i=0;i<NUMDAC;i++)
	  {
		WriteParam(profileHdr->nAnalog[i]);
	  }
}


void DownloadProfile()
{
	unsigned int *mapped = (unsigned int *)dsp->BusMaster.Addr;
	unsigned int *xferptr;
	int rtn,i;

	if(collecting) throw (char*)"DSP is still collecting"; // seb
	DebugPrint("In DownloadProfile stub\n");
	// make sure profile is loaded
	if(!loaded) throw (char*)"DSP has no profile loaded"; // seb

	// send header so target can allocate memory

	WriteCommand(DOWNLOAD_PROFILE);

	_PutHdr(&profileHdr);
	// wait for target to allocate memory

	
	rtn=ReadReply(); // ready1
	if(rtn != 0xf000010f) //READY)
	  {
		//sprintf(ErrStr,"DMA setup error: 0x%06x (dly,dig,ana0123)\n",rtn);
		sprintf(ErrStr,"DownLoadProfile malloc error: 0x%06x (dly,dig,ana3210)",rtn);
		//PrintError(ErrStr);
		//return rtn;
		throw (char*)ErrStr;
	  }
	

	sprintf(ErrStr,"profileHdr.nDigital %d\n", profileHdr.nDigital);
	DebugPrint(ErrStr);
 	if(profileHdr.nDigital )
	{
	   //seb20060906  WriteParam(profileHdr.nDigital*2 );
		
		// now do dma to target
		DmaToTarget((int*)digitalList,(int*)mapped,2*profileHdr.nDigital);

		rtn = ReadReply(); //ready2
		if(rtn!=0xf000020f)
		  {
			sprintf(ErrStr,"Error after DMA dig: 0x%x",rtn);
			throw (char*)ErrStr;
		  }
	}
	for (i=0;i<NUMDAC;i++)
	  {
		sprintf(ErrStr,"profileHdr.nAnalog[%i] %d\n", i, profileHdr.nAnalog[i]);
		DebugPrint(ErrStr);
		if(profileHdr.nAnalog[i] )
		  {
			//seb20060906  WriteParam(profileHdr.nAnalog[i]*2 );
			
			// now do dma to target
			DmaToTarget((int*)analogList[i],(int*)mapped,
						2*profileHdr.nAnalog[i]);

			rtn = ReadReply(); //ready3
			if(rtn!=(0xf000030f | (i<<3)))
			  {
				if((rtn & 0xff000001) == 0xff000001)
				  sprintf(ErrStr,"ana%d profile goes too high (0x%x)",i, rtn);
				else if((rtn & 0xff000002) == 0xff000002)
				  sprintf(ErrStr,"ana%d profile goes too low (0x%x)",i, rtn);
				else
				  sprintf(ErrStr,"Error after DMA ana%d: 0x%x",i,rtn);
				throw (char*)ErrStr;
			  }
		  }
	  }

	if((rtn=ReadReply()) != 0xfffffff1) //DONE)
	  {
		sprintf(ErrStr,"DownloadProfile done expected reply 0xfffffff1 but got 0x%x", rtn);
		throw (char*)ErrStr;
	  }
	downloaded = TRUE;
}

void	Profile_Set(const char *bytes, const int len,
					unsigned int *DigList, int nx, int ndig,
					unsigned int *AnaList0, int nx0, int nAnalog0,
					unsigned int *AnaList1, int nx1, int nAnalog1,
					unsigned int *AnaList2, int nx2, int nAnalog2,
					unsigned int *AnaList3, int nx3, int nAnalog3
					)
{
  if(len != sizeof(ProStr)) throw (char*)"byte string is of wrong size";
  loaded=FALSE; // profile loaded
  downloaded=FALSE;// profile downloaded to target
  memcpy((void*)&profileHdr, bytes, len);
  FreeBuffers();


  if(ndig != profileHdr.nDigital) 
	throw (char*)"DigList is of wrong length (ny)";
  if(profileHdr.nDigital>0 && nx  != 2) 
	throw (char*)"DigList needs to be 2D with nx=2";
  if(nAnalog0 != profileHdr.nAnalog[0]) 
	throw (char*)"nAnalog[0]List is of wrong length (ny)";
  if(profileHdr.nAnalog[0]>0 && nx0 != 2) 
	throw (char*)"nAnalog[0]List needs to be 2D with nx=2";
  if(nAnalog1 != profileHdr.nAnalog[1]) 
	throw (char*)"nAnalog[1]List is of wrong length (ny)";
  if(profileHdr.nAnalog[1]>0 && nx1 != 2) 
	throw (char*)"nAnalog[1]List needs to be 2D with nx=2";
  if(nAnalog2 != profileHdr.nAnalog[2]) 
	throw (char*)"nAnalog[2]List is of wrong length (ny)";
  if(profileHdr.nAnalog[2]>0 && nx2 != 2) 
	throw (char*)"nAnalog[2]List needs to be 2D with nx=2";
  if(nAnalog3 != profileHdr.nAnalog[3]) 
	throw (char*)"nAnalog[3]List is of wrong length (ny)";
  if(profileHdr.nAnalog[3]>0 && nx3 != 2) 
	throw (char*)"nAnalog[3]List needs to be 2D with nx=2";
  
  GetBuffers();

  if(profileHdr.nDigital>0)
	memcpy((void*)digitalList, DigList, 
		   2*profileHdr.nDigital*sizeof(unsigned int));
  if(profileHdr.nAnalog[0]>0)
	memcpy((void*)(analogList[0]), AnaList0, 
		   2*profileHdr.nAnalog[0]*sizeof(unsigned int));
  if(profileHdr.nAnalog[1]>0)
	memcpy((void*)(analogList[1]), AnaList1, 
		   2*profileHdr.nAnalog[1]*sizeof(unsigned int));
  if(profileHdr.nAnalog[2]>0)
	memcpy((void*)(analogList[2]), AnaList2, 
		   2*profileHdr.nAnalog[2]*sizeof(unsigned int));
  if(profileHdr.nAnalog[3]>0)
	memcpy((void*)(analogList[3]), AnaList3, 
		   2*profileHdr.nAnalog[3]*sizeof(unsigned int));
  
  loaded=TRUE ;
}

float ADUToMicron(int axis,unsigned int Adu)
{
	return (float)(Adu*MicronsPerADU[axis]);
}

unsigned int MicronToADU(int axis,float Micron)
{	
  unsigned int a = (unsigned int)(Micron/MicronsPerADU[axis]);
  if(a > 0xffff) throw (char*)"piezo conversion out of 16 bit range";
  return a;
}



void GetBuffers()
{
	int fail=FALSE;
	int i;

    digitalList = (timeValuePair *)calloc( 2*profileHdr.nDigital  , sizeof( unsigned int ) );
	if( digitalList == NULL )
	  { 
		FreeBuffers();
		throw (char*)"Cannot allocate Digtal buffer";
	  }

	for(i=0;i<NUMDAC;i++)
	  {
		analogList[i]= (timeValuePair *)calloc( 2*profileHdr.nAnalog[i]  , sizeof( unsigned int ) );
		if( analogList[i] == NULL )
		  { 
			FreeBuffers();
			throw (char*)"Cannot allocate one of the ana buffer";
		  }
	  }
}

void FreeBuffers()
{
  int i;
  free(digitalList);
  digitalList = NULL;
  for (i=0;i<NUMDAC;i++)
	{	
	  free(analogList[i]);
	  analogList[i] = NULL;
	}
  loaded=FALSE;
}

void DmaToTarget(int *Inbuf, int *shared, int numwds)
{
  int numpak,resid, packb;
  int i,rtn;
  int *Sptr,*tptr;
  Sptr = Inbuf;
  tptr = shared;
  
  numpak = numwds/PKTSIZE;
  resid = numwds%PKTSIZE;
  packb = PKTSIZE*sizeof(int);
  
  sprintf(ErrStr," %d packets, with %d residual words\n", numpak,resid);
  DebugPrint(ErrStr);
  WriteParam(numpak);
  // start the  numpak loop
  for(i=0;i<numpak;i++)
	{	
	  sprintf(ErrStr,"i:src:dest:bytes %d:0x%x:0x%x:%d \n", i,Sptr,tptr,packb);
	  DebugPrint(ErrStr);
	  memcpy((void *)tptr,(const void *)Sptr, packb);
	  //		LoadPacket(Sptr, mapped, PKTSIZE);
	  //Sleep(50);
	  //Sleep(1); // CHECK if needed
	  WriteParam(i);// *after* memcpy so that mem is available 
	  Sptr += PKTSIZE;
	  if((rtn=ReadReply()) != i) // wait for: request next packet
		{
		  sprintf(ErrStr,"DMA error1: Target returns: 0x%x",rtn);
		  throw (char*)ErrStr;
		}
	}
  //		send residual
  memcpy((void *)tptr,(const void *)Sptr, resid*sizeof(int));
  //		LoadPacket(Sptr, mapped, resid)
  WriteParam(resid); // *after* memcpy so that mem is available 
  if((rtn=ReadReply()) != 0xfffffff0) // 4294967280
	{
	  sprintf(ErrStr,"DMA error2: Target returns: 0x%x expected DONE",rtn);
	  throw (char*)ErrStr;
	}
}
