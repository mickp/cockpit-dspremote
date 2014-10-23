import pyC67

import Pyro4
import threading
import traceback

def melError(xx):
    '''make Mel;s hex-error codes (ASCII) more readable by converting to
    (human readable) letter
    '''
    s= ''
    while xx>0:
        s='%c%s'%((xx&0xff),s)
        xx>>=8
    return s

class d:
    def __init__(self, fn):
        self.fn = fn
        self.startCollectThread()
        self.reInit()
        self.clientConnection = None
        self.MoveAbsolute(0, 10)
        self.WriteShutter(255)


    def receiveClient(self, uri):
        print "receiveClient=",uri
        self.clientConnection = Pyro4.Proxy(uri)
        self.collThread.setConnection(self.clientConnection)
        self.reInit()
        print "clientConnection=",self.clientConnection

    def Abort(self):
        pyC67.Abort()
        self.startCollectThread()
        
    def UpdateNReps(self, newCount):
        pyC67.UpdateNReps( newCount )

    def profileSet(self, string, digList, anaDeltaList0, anaDeltaList1, anaDeltaList2, anaDeltaList3 ):
        pyC67.Profile_Set(string, digList, anaDeltaList0, anaDeltaList1, anaDeltaList2, anaDeltaList3)


    def melError(self, code):
        '''code can be hex number or string (e.g.'4548') '''
        if type(code) is type('string'):
            return melError(int(code,16))
        else:
            return melError(code)
    def reInit(self):
        try:
            pyC67.C67Open( self.fn )
        except Exception, e:
            print "reInit failed:", e
            raise RuntimeError(" **** ERROR when C67Open( %s ) : %s" % (self.fn, e))
        
    def isCollecting(self):
        return pyC67.cvar.collecting

    def getLastCollectReturn(self):
        return self.collThread.collectReturn
    
    
    def ReadPosition(self, axis):
        return pyC67.ReadPosition(axis)
    def ReadActual(self, axis):
        return pyC67.ReadActual(axis)
        
    def WriteShutter(self, value):
        pyC67.WriteShutter(value)

    def ReadShutter(self):
        return pyC67.ReadShutter()
    def TriggerSpec(self):
        pyC67.TriggerSpec()

    def SetBase(self, axis, value):
        pyC67.SetBase(axis,value)

    def MoveAbsolute(self, axis, target):
        pyC67.MoveAbsolute(axis, target)

    def MoveAbsoluteADU(self, axis, target):
        pyC67.MoveAbsoluteADU(axis, target)

    def MoveRelative(self, axis, target):
        pyC67.MoveRelative(axis, target)

    def Expose(self, CamMask):
        pyC67.Expose(CamMask)

    def Flash(self):
        pyC67.Flash(CamMask)

    def ReadDigital(self):
        return pyC67.ReadDigital()
    def WriteDigital(self, value):
        pyC67.WriteDigital(value)

    def ReadCurPos(self, axis):
        return pyC67.ReadCurPos(axis)

    def InitProfile(self, reps):
        r = pyC67.InitProfile(reps)

    def DownloadProfile(self):
        pyC67.DownloadProfile()


    def trigSomething(self, doWhat):
        self.collThread.do(doWhat)

    def trigCollect(self):
        self.collThread.do('collect')

    def arcl(self, cameras, lightTimePairs):
        self.collThread.do(('arcl', (cameras, lightTimePairs)))


    def getframedata(self):
        numframe = pyC67.GetFrameCount()
        import numpy as N
        #from numarray import records as rec
        #fd=rec.array(formats="u4,u4,4f4",
        #             shape=numframe,
        #             names=('rep','step','adc'),
        #             aligned=1)
        #aa = na.array(sequence=fd._data, type=na.UInt8, copy=0, savespace=0, shape=numframe*6*4)
        fd = N.recarray(numframe,
                        formats="u4,u4,4f4",
                        names='rep,step,adc')
        #ProStr_aa[:] = 0
        if numframe>0:
            aa = N.ndarray(numframe*4*6, buffer=fd.data,dtype=N.uint8)
            pyC67.ReturnFrameData(aa)
        return fd

    def startCollectThread(self):
        pyC67.mmInitMMTimer()
        try:
            self.collThread.doEvent.doWhat = 'quit'
        except:
            pass
        self.collThread = CollectThread(self)
        self.collThread.start()


    #just for debugging
    def ReadReply(self, timeoutMS=10):
        return pyC67.ReadReply(timeoutMS)
    def WriteCommand(self, Command):
        pyC67.WriteCommand(Command)
    def WriteParam(self, param):
        pyC67.WriteParam(param)
        

##http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/65448
class CollectThread(threading.Thread):
    """
    A sample thread class
    """
        
    def __init__(self, dObject):
        """
        Constructor, setting initial variables
        """
        self.d = dObject
        self.doEvent = threading.Event()
        self._sleepperiod = 1.0
        self.doEvent.doWhat = None
        self.clientConnection = None

        threading.Thread.__init__(self, name="CollectThread")

    def do(self, what):
        if self.doEvent.isSet():
            message = "CollectThread busy doing %s; can't do %s" % (self.doEvent.doWhat, str(what))
            print message
            raise RuntimeError(message)
        self.doEvent.doWhat = what
        self.doEvent.set()
      
    def run(self):
        """
        overload of threading.thread.run()
        main control loop
        """
        print "%s starts" % (self.getName(),)
        
        count = 0
        while 1:
            self.doEvent.doWhat = None
            self.doEvent.clear()
            self.doEvent.wait( self._sleepperiod )
            if self.doEvent.doWhat=='quit':         
                break

            if self.doEvent.doWhat=='collect':
                try:
                    del self.collectReturn
                except:
                    pass
                try:
                    retVal = pyC67.Collect()  # frameCount
                except Exception, e: 
                    retVal = e
                self.collectReturn = retVal

                retVal = retVal, [pyC67.ReadPosition(i) for i in range(4)]
                self.clientConnection.receiveData("DSP done", retVal)
                        
            elif hasattr(self.doEvent.doWhat, '__len__') and self.doEvent.doWhat[0]=='arcl':
                try:
                    cameras, lightTimePairs = self.doEvent.doWhat[1]
                    if lightTimePairs:
                        # Expose all lights at the start, then drop them out
                        # as their exposure times come to an end.
                        # Sort so that the longest exposure time comes last.
                        lightTimePairs.sort(key = lambda a: a[1])
                        curDigital = cameras + sum([p[0] for p in lightTimePairs])
                        self.d.WriteDigital(curDigital)
                        print "Start with",curDigital
                        totalTime = lightTimePairs[-1][1]
                        curTime = 0
                        for line, runTime in lightTimePairs:
                            # Wait until we need to open this shutter.
                            waitTime = runTime - curTime
                            if waitTime > 0:
                                pyC67.mmSleep(waitTime)
                            curDigital -= line
                            self.d.WriteDigital(curDigital)
                            curTime += waitTime
                            print "At",curTime,"set",curDigital
                        if totalTime - curTime:
                            pyC67.mmSleep(totalTime - curTime)
                        print "Finally at",totalTime,"set",0
                        self.d.WriteDigital(0)
                    else:
                        self.d.Expose(cameras)
                except Exception, e:
                    print "Error in arcl:",e
                    traceback.print_exc()
                    raise RuntimeError("Error in arcl: %s", e)
                    
            elif self.doEvent.doWhat is None:
                continue
            else:
                print "do unknown:", self.doEvent.doWhat

                
        
        print "%s ends" % (self.getName(),)


    def setConnection(self, connection):
        self.clientConnection = connection
        

    def join(self,timeout=None):
        """
        Stop the thread
        """
        self.do('quit')
        
        threading.Thread.join(self, timeout)

