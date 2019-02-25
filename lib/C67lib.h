/* C67lib include file */
#include "Target.h"

#include "OMX_commands.h"

// //seb - this might be changed to allow cusomized printing
// //seb 
//#define DebugPrint printf
#define DebugPrint //


#define TARGET 0
#define NUMDAC 4
#define MAXDAC 0xffff
#define ADC_OFFSET 4


typedef struct
{
  int count;			 // number of clocks in profile 
  float	clock;					 // microseconds between interrupts
  unsigned int	InitDio;		 // Initial Digital I/O states
  int  nDigital;		 // number of digital (time,values) pairs
  int  nAnalog[NUMDAC];// number of analog (time,values) pairs
} ProStr;

typedef struct
{
	unsigned int	rep;		// cycle
	unsigned int	step;		// step in cycle
	unsigned int	pair0;		// ADC pair 0
	unsigned int	pair1;		// ADC pair 1
}	Xframe;

typedef struct
{
	unsigned int	rep;		// cycle
	unsigned int	step;		// step in cycle
	float	adc0;
	float	adc1;
	float	adc2;
	float	adc3;
}	Fframe;

typedef struct
{
	int	single;				//TRUE if reps < 0
	HANDLE	usrEvent;		// signal if single
	HANDLE	myEvent;		// signal at end
	unsigned int	count;	// number of frames
	Xframe	*fptr;		// pointer to frame buffer
	int	*dmasrc;			// pointer to hostbuffer
} ContextStr;

typedef union
{
		int x;
		float f;
} MailboxValue;

typedef struct _timeValuePair
{ 
  int   step;
  int   val;
} timeValuePair;

// User Routines
void 	C67Open(char *executive);
void	C67Close();
void	Profile_Set(const char *bytes, const int len,
					unsigned int *DigList, int nx, int ndig,
					unsigned int *AnaList0, int nx0, int nAna0,
					unsigned int *AnaList1, int nx1, int nAna1,
					unsigned int *AnaList2, int nx2, int nAna2,
					unsigned int *AnaList3, int nx3, int nAna3);
unsigned int	ReadPosition(int axis);		// reads DAC
unsigned int	ReadActual(int axis);		// reads ADC
void	WriteShutter(int value);
int		ReadShutter();
void	TriggerSpec();
//20060829  void 	SetBaseAbsolute(int axis,float value);
// void 	SetAmplitude(int axis, unsigned int value);
void 	MoveAbsolute(int axis, unsigned int value);
void 	DownloadProfile();
void    *InitProfile(int reps, HANDLE UsrEvent);

//seb
typedef void doneCallbackFct(unsigned long);
//seb int		Collect();
 int		Collect(doneCallbackFct *doneCallback);
void 	Expose(int cameramask);	// cameramask 0x1 - 0xf for all cameras
void 	MoveRelative(int axis, int value);
void 	Flash();
unsigned int	ReadDigital();
void 	WriteDigital(int value);
void	Abort();
void UpdateNReps(int newCount);
void	PostStatus(void *ssptr); // not implemented
int		GetFrameCount();
void	ReturnFrameData(void *userbuf);


// Utilities
void	PrintCardInfo();
void	PrintError(char *str);  //seb - this might be changed to allow cusomized printing
int		*InitDma();
int     IsCollecting();
void 	GetBuffers();
void	FreeBuffers();
void	DmaToTarget(int *Inbuf, int *shared, int numwds);
void	PutHdr( ProStr *p);
CARDINFO	*GetCardInfo();
unsigned int	*InitDMA();
//unsigned int	MicronToADU(int axis,float micron);
//float	ADUToMicron(int axis,unsigned int adu);
void	WriteCommand(int	Command);
void	WriteParam(int	param);
int		ReadReply(int timeoutMS=10);
void	CollectionCleanup();
void	XframeToFframe(void *in, void *out);
void 	SetBase(int axis, float value);



