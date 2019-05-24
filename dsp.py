import configparser
import os
import Pyro4
import threading
import traceback
import time
import uuid

import ctypes
from numpy.ctypeslib import ndpointer

try:
    pyC67 = ctypes.CDLL("C67")
except:
    fn = os.path.join(os.path.dirname(__file__), "C67.dll")
    pyC67 = ctypes.CDLL(fn)

pyC67.Profile_Set.argtypes = [
    ctypes.c_char_p,
    ctypes.c_int,
    ndpointer(ctypes.c_uint, flags="C_CONTIGUOUS"),
    ctypes.c_int,
    ctypes.c_int,
    ndpointer(ctypes.c_uint, flags="C_CONTIGUOUS"),
    ctypes.c_int,
    ctypes.c_int,
    ndpointer(ctypes.c_uint, flags="C_CONTIGUOUS"),
    ctypes.c_int,
    ctypes.c_int,
    ndpointer(ctypes.c_uint, flags="C_CONTIGUOUS"),
    ctypes.c_int,
    ctypes.c_int,
    ndpointer(ctypes.c_uint, flags="C_CONTIGUOUS"),
    ctypes.c_int,
    ctypes.c_int,
    ]

LOGGING = True

CONFIG_NAME = 'dsp'
PATH = os.path.dirname(os.path.abspath(__file__))
Pyro4.config.SERIALIZER = 'pickle'
Pyro4.config.SERIALIZERS_ACCEPTED.remove('serpent')
Pyro4.config.SERIALIZERS_ACCEPTED.add('pickle')

def melError(xx):
    '''make Mel;s hex-error codes (ASCII) more readable by converting to
    (human readable) letter
    '''
    s= ''
    while xx>0:
        s='%c%s'%((xx&0xff),s)
        xx>>=8
    return s


class Logger(object):
    def __init__(self):
        import datetime
        self.fh = None
        self.dt = datetime.datetime

    def log(self, msg):
        if not LOGGING:
            return # do nothing for now
        if not self.fh:
            self.open('dsplog')
        self.fh.write(self.dt.strftime(self.dt.now(),'%Y-%m-%d %H:%M:%S.%f:  '))
        self.fh.write(msg + '\n')
        self.fh.flush()

    def open(self, filename):
        path = os.path.dirname(os.path.abspath(__file__))
        self.fh = open(os.path.join(path, str(filename) + '.txt'), 'w')

    def close(self):
        self.fh.close()
        self.fh = None

_logger = Logger()

@Pyro4.expose
class d:
    def __init__(self, fn="OMX_release.out"):
        self._profileInited = False
        self.fn = fn
        self.clientConnection = None
        self.reInit()
        self.WriteShutter(255)

    def receiveClient(self, uri):
        _logger.log('Setting client to %s' % (uri))
        self.clientConnection = Pyro4.Proxy(uri)
        self.collThread.setConnection(self.clientConnection)
        #self.reInit()

    def Abort(self):
        _logger.log('Abort')
        was_collecting = self.isCollecting()
        try:
            pyC67.Abort()
        except:
            pass
        if was_collecting:
            try:
                self.collThread.do('quit')
            except:
                pass
            time.sleep(0.05)
            self.collThread.join()
            self.reInit()
        
    def UpdateNReps(self, newCount):
        pyC67.UpdateNReps( newCount )

    def profileSet(self, descriptionDict, digList, anaDeltaList0, anaDeltaList1, anaDeltaList2, anaDeltaList3):
        import numpy as np
        description = np.rec.array(
            None,
            formats="u4, f4, u4, u4, 4u4",
            names=('count', 'clock', 'InitDio', 'nDigital', 'nAnalog'),
            aligned=True, shape=1)
        description['count'] = descriptionDict['count']
        description['clock'] = descriptionDict['clock']
        description['InitDio'] = descriptionDict['InitDio']
        description['nDigital'] = descriptionDict['nDigital']
        description['nAnalog'] = descriptionDict['nAnalog']

        #pyC67.Profile_Set(description.tostring(), digList, anaDeltaList0, anaDeltaList1, anaDeltaList2, anaDeltaList3)
        dstr = description.tostring()
        pyC67.Profile_Set(dstr, len(dstr),
                          digList, digList.shape[1], digList.shape[0],
                          anaDeltaList0, anaDeltaList0.shape[1], anaDeltaList0.shape[0],
                          anaDeltaList1, anaDeltaList1.shape[1], anaDeltaList1.shape[0],
                          anaDeltaList2, anaDeltaList2.shape[1], anaDeltaList2.shape[0],
                          anaDeltaList3, anaDeltaList3.shape[1], anaDeltaList3.shape[0])

    def melError(self, code):
        '''code can be hex number or string (e.g.'4548') '''
        if type(code) is type('string'):
            return melError(int(code,16))
        else:
            return melError(code)

    def reInit(self):
        try:
            pyC67.C67Open( self.fn.encode() )
        except Exception as e:
            # print "reInit failed:", e
            raise RuntimeError(" **** ERROR when C67Open( %s ) : %s" % (self.fn, e))
        # Restart the collect thread.
        self.startCollectThread()
        
    def isCollecting(self):
        return pyC67.IsCollecting()

    def getLastCollectReturn(self):
        return self.collThread.collectReturn
    
    
    def ReadPosition(self, axis):
        if self.isCollecting():
            return 0.0
        else:
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
        _logger.log('Move axis %d to %s V' % (axis, target))
        pyC67.MoveAbsolute(axis, target)

    def MoveAbsoluteADU(self, axis, target):
        _logger.log('Move axis %d to %s ADU' % (axis, target))
        pyC67.MoveAbsolute(axis, target)

    def MoveRelative(self, axis, target):
        _logger.log('Move axis %d by %s V' % (axis, target))
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
        _logger.log('InitProfile')
        r = pyC67.InitProfile(reps)
        self._profileInited = True

    def DownloadProfile(self):
        _logger.log('DownloadProfile')
        self._profileInited = False
        pyC67.DownloadProfile()

    def trigSomething(self, doWhat):
        self.collThread.do(doWhat, uid=uuid.uuid1())

    def trigCollect(self):
        # If trigCollect is called without first downloading and initing a profile,
        # this process dies abruptly (presumably hitting some access violation in 
        # the low-level code?). Don't let this happen.
        if not self._profileInited:
            raise Exception("Need to InitProfile before trigCollect.")
        self.collThread.do('collect', uid=uuid.uuid1())
        self._profileInited = False

    def arcl(self, cameras, lightTimePairs):
        self.collThread.do(('arcl', (cameras, lightTimePairs)), uid=uuid.uuid1())


    def getframedata(self):
        numframe = pyC67.GetFrameCount()
        import numpy as N
        fd = N.recarray(numframe,
                        formats="u4,u4,4f4",
                        names='rep,step,adc')
        #ProStr_aa[:] = 0
        if numframe>0:
            aa = N.ndarray(numframe*4*6, buffer=fd.data,dtype=N.uint8)
            pyC67.ReturnFrameData(aa)
        return fd

    def startCollectThread(self):
        try:
            self.collThread.doEvent.doWhat = 'quit'
        except:
            pass
        self.collThread = CollectThread(self)
        self.collThread.start()
        if self.clientConnection:
            self.collThread.setConnection(self.clientConnection)


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
        self.doEvent.uid = None
        self.clientConnection = None
        self.eventLock = threading.Lock()

        threading.Thread.__init__(self, name="CollectThread")


    def do(self, what, uid=None):
        _logger.log("%s: Do %s received." % (uid, what))
        # Wait for any previous event to finish.
        while self.doEvent.isSet():
            sleep(0.05)

        with self.eventLock:
            self.doEvent.doWhat = what
            self.doEvent.uid = uid
            self.doEvent.set()

      
    def run(self):
        """
        overload of threading.thread.run()
        main control loop
        """
        # print "%s starts" % (self.getName(),)
        
        count = 0
        while 1:
            if not self.doEvent.wait(self._sleepperiod):
                # Timeout - skip back to start of loop.
                continue

            # There is an event to process.
            with self.eventLock:
                _logger.log("%s: Handling event..." % self.doEvent.uid)
                self.doEvent.clear()
                if self.doEvent.doWhat=='quit':  
                    _logger.log("%s: ... in quit block." % self.doEvent.uid)
                    break

                if self.doEvent.doWhat=='collect':
                    _logger.log("%s: ... entered collect block." % self.doEvent.uid)
                    try:
                        del self.collectReturn
                    except:
                        pass
                    try:
                        _logger.log("%s: ... calling pyC67.Collect() ..." % self.doEvent.uid)
                        retVal = pyC67.Collect()  # frameCount
                        _logger.log("%s: ... pyC67.Collect() returned ..." % self.doEvent.uid)
                    except Exception as e:
                        _logger.log("%s: ... exception in collect block." % self.doEvent.uid)
                        retVal = e
                    self.collectReturn = retVal

                    _logger.log("%s: ... calling pyC67.ReadPosition(i) for i in range(4) ..." % self.doEvent.uid)
                    try:
                        retVal = retVal, [pyC67.ReadPosition(i) for i in range(4)]
                    except Exception as e:
                        retval = e
                    _logger.log("%s: ... pyC67.ReadPosition() calls done ..." % self.doEvent.uid)
                    self.clientConnection.receiveData("DSP done", retVal)
                    _logger.log("%s: ... leaving collect block" % self.doEvent.uid)
                            
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
                            totalTime = lightTimePairs[-1][1]
                            curTime = 0
                            for line, runTime in lightTimePairs:
                                # Wait until we need to open this shutter.
                                waitTime = runTime - curTime
                                if waitTime > 0:
                                    time.sleep(waitTime / 1000.)
                                curDigital -= line
                                self.d.WriteDigital(curDigital)
                                curTime += waitTime
                            if totalTime - curTime:
                                try:
                                    time.sleep( (totalTime - curTime) / 1000.)
                                except:
                                    self.loggler.log('error in pyC67.mmSleep')
                                    _logger.log(traceback.format_exc())
                            try:
                                self.d.WriteDigital(0)
                            except:
                                _logger.log('error in self.d.WriteDigital(0)')
                                _logger.log(traceback.format_exc())
                        else:
                            self.d.Expose(cameras)
                    except Exception as e:
                        _logger.log('Error in arcl ... ' + str(e))
                        _logger.log(traceback.format_exc())
                        raise RuntimeError("Error in arcl: %s", e)
                        
                elif self.doEvent.doWhat is None:
                    pass
                else:
                    pass
                    _logger.log('Unknown doEvent.doWhat: %s' % self.doEvent.doWhat)

    def setConnection(self, connection):
        self.clientConnection = connection
        

    def join(self,timeout=None):
        """
        Stop the thread
        """
        self.do('quit')
        
        threading.Thread.join(self, timeout)


class Server(object):
    def __init__(self):
        self.server = None
        self.daemon_thread = None
        self.config = None
        self.run_flag = True

    def __del__(self):
        self.run_flag = False
        if self.daemon_thread:
            self.daemon_thread.join()


    def run(self):
        import readconfig
        config = readconfig.config

        blob = config.get(CONFIG_NAME, 'blob')
        host = config.get(CONFIG_NAME, 'ipAddress')
        port = config.getint(CONFIG_NAME, 'port')

        self.server = d(os.path.join(PATH, blob))

        daemon = Pyro4.Daemon(port=port, host=host)

        # Start the daemon in a new thread.
        self.daemon_thread = threading.Thread(
            target=Pyro4.Daemon.serveSimple,
            args = ({self.server: 'pyroDSP'},),
            kwargs = {'daemon': daemon, 'ns': False}
            )
        self.daemon_thread.start()

        # Wait until run_flag is set to False.
        while self.run_flag:
            time.sleep(1)

        # Do any cleanup.
        daemon.shutdown()

        self.daemon_thread.join()


    def stop(self):
        self.run_flag = False

def main ():
    ## Only run when called as a script --- do not run on include.
    #  This way, we can use an interactive shell to test out the class.
    server = Server()
    server_thread = threading.Thread(target = server.run)
    server_thread.start()

    try:
        while True:
            time.sleep(1)
    except (KeyboardInterrupt, SystemExit):
        server.stop()
        server_thread.join()

if __name__ == "__main__":
    main()
