# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.4
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.



from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_pyC67', [dirname(__file__)])
        except ImportError:
            import _pyC67
            return _pyC67
        if fp is not None:
            try:
                _mod = imp.load_module('_pyC67', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _pyC67 = swig_import_helper()
    del swig_import_helper
else:
    import _pyC67
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


class TIMECAPS(_object):
    """Proxy of C++ TIMECAPS class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TIMECAPS, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TIMECAPS, name)
    __repr__ = _swig_repr
    __swig_setmethods__["wPeriodMin"] = _pyC67.TIMECAPS_wPeriodMin_set
    __swig_getmethods__["wPeriodMin"] = _pyC67.TIMECAPS_wPeriodMin_get
    if _newclass:wPeriodMin = _swig_property(_pyC67.TIMECAPS_wPeriodMin_get, _pyC67.TIMECAPS_wPeriodMin_set)
    __swig_setmethods__["wPeriodMax"] = _pyC67.TIMECAPS_wPeriodMax_set
    __swig_getmethods__["wPeriodMax"] = _pyC67.TIMECAPS_wPeriodMax_get
    if _newclass:wPeriodMax = _swig_property(_pyC67.TIMECAPS_wPeriodMax_get, _pyC67.TIMECAPS_wPeriodMax_set)
    def __init__(self): 
        """__init__(self) -> TIMECAPS"""
        this = _pyC67.new_TIMECAPS()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _pyC67.delete_TIMECAPS
    __del__ = lambda self : None;
TIMECAPS_swigregister = _pyC67.TIMECAPS_swigregister
TIMECAPS_swigregister(TIMECAPS)


def mmInitMMTimer():
  """mmInitMMTimer()"""
  return _pyC67.mmInitMMTimer()

def expflsh():
  """expflsh()"""
  return _pyC67.expflsh()

def mmSleep(*args):
  """mmSleep(int sleepMS)"""
  return _pyC67.mmSleep(*args)

def recoverC67InternalEvent():
  """recoverC67InternalEvent()"""
  return _pyC67.recoverC67InternalEvent()
PKTSIZE = _pyC67.PKTSIZE
WRITE_SHUTTER = _pyC67.WRITE_SHUTTER
READ_SHUTTER = _pyC67.READ_SHUTTER
TRIGGER_SPEC = _pyC67.TRIGGER_SPEC
DOWNLOAD_PROFILE = _pyC67.DOWNLOAD_PROFILE
INIT_PROFILE = _pyC67.INIT_PROFILE
COLLECT = _pyC67.COLLECT
EXPOSE = _pyC67.EXPOSE
FLASH = _pyC67.FLASH
READ_DIGITAL = _pyC67.READ_DIGITAL
WRITE_DIGITAL = _pyC67.WRITE_DIGITAL
ABORT = _pyC67.ABORT
POST_STATUS = _pyC67.POST_STATUS
INIT_DMA = _pyC67.INIT_DMA
READ_ACTUAL = _pyC67.READ_ACTUAL
SET_BASE_0 = _pyC67.SET_BASE_0
SET_BASE_1 = _pyC67.SET_BASE_1
SET_BASE_2 = _pyC67.SET_BASE_2
SET_BASE_3 = _pyC67.SET_BASE_3
MOVE_ABSOLUTE_0 = _pyC67.MOVE_ABSOLUTE_0
MOVE_ABSOLUTE_1 = _pyC67.MOVE_ABSOLUTE_1
MOVE_ABSOLUTE_2 = _pyC67.MOVE_ABSOLUTE_2
MOVE_ABSOLUTE_3 = _pyC67.MOVE_ABSOLUTE_3
READ_POSITION_0 = _pyC67.READ_POSITION_0
READ_POSITION_1 = _pyC67.READ_POSITION_1
READ_POSITION_2 = _pyC67.READ_POSITION_2
READ_POSITION_3 = _pyC67.READ_POSITION_3
READ_ACTUAL_0 = _pyC67.READ_ACTUAL_0
READ_ACTUAL_1 = _pyC67.READ_ACTUAL_1
READ_ACTUAL_2 = _pyC67.READ_ACTUAL_2
READ_ACTUAL_3 = _pyC67.READ_ACTUAL_3
UPDATE_NREPS = _pyC67.UPDATE_NREPS
UNKNOWN_COMMAND = _pyC67.UNKNOWN_COMMAND
DONE = _pyC67.DONE
READY = _pyC67.READY
NEXT = _pyC67.NEXT
NUMDAC = _pyC67.NUMDAC
MAXDAC = _pyC67.MAXDAC
ADC_OFFSET = _pyC67.ADC_OFFSET
class ProStr(_object):
    """Proxy of C++ ProStr class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ProStr, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ProStr, name)
    __repr__ = _swig_repr
    __swig_setmethods__["count"] = _pyC67.ProStr_count_set
    __swig_getmethods__["count"] = _pyC67.ProStr_count_get
    if _newclass:count = _swig_property(_pyC67.ProStr_count_get, _pyC67.ProStr_count_set)
    __swig_setmethods__["clock"] = _pyC67.ProStr_clock_set
    __swig_getmethods__["clock"] = _pyC67.ProStr_clock_get
    if _newclass:clock = _swig_property(_pyC67.ProStr_clock_get, _pyC67.ProStr_clock_set)
    __swig_setmethods__["InitDio"] = _pyC67.ProStr_InitDio_set
    __swig_getmethods__["InitDio"] = _pyC67.ProStr_InitDio_get
    if _newclass:InitDio = _swig_property(_pyC67.ProStr_InitDio_get, _pyC67.ProStr_InitDio_set)
    __swig_setmethods__["nDigital"] = _pyC67.ProStr_nDigital_set
    __swig_getmethods__["nDigital"] = _pyC67.ProStr_nDigital_get
    if _newclass:nDigital = _swig_property(_pyC67.ProStr_nDigital_get, _pyC67.ProStr_nDigital_set)
    __swig_setmethods__["nAnalog"] = _pyC67.ProStr_nAnalog_set
    __swig_getmethods__["nAnalog"] = _pyC67.ProStr_nAnalog_get
    if _newclass:nAnalog = _swig_property(_pyC67.ProStr_nAnalog_get, _pyC67.ProStr_nAnalog_set)
    def __init__(self): 
        """__init__(self) -> ProStr"""
        this = _pyC67.new_ProStr()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _pyC67.delete_ProStr
    __del__ = lambda self : None;
ProStr_swigregister = _pyC67.ProStr_swigregister
ProStr_swigregister(ProStr)
cvar = _pyC67.cvar
tc = cvar.tc
timer = cvar.timer

def Collect(doneCallback = None):
  """
    Collect(doneCallbackFct doneCallback = None) -> int
    Collect() -> int
    """
  return _pyC67.Collect(doneCallback)


def C67Open(*args):
  """C67Open(char executive)"""
  return _pyC67.C67Open(*args)

def C67Close():
  """C67Close()"""
  return _pyC67.C67Close()

def Profile_Set(*args):
  """
    Profile_Set(char STRING, unsigned int arg1, unsigned int arg2, 
        unsigned int arg3, unsigned int arg4, unsigned int array2d)
    """
  return _pyC67.Profile_Set(*args)

def ReadPosition(*args):
  """ReadPosition(int axis) -> float"""
  return _pyC67.ReadPosition(*args)

def ReadActual(*args):
  """ReadActual(int axis) -> float"""
  return _pyC67.ReadActual(*args)

def WriteShutter(*args):
  """WriteShutter(int value)"""
  return _pyC67.WriteShutter(*args)

def ReadShutter():
  """ReadShutter() -> int"""
  return _pyC67.ReadShutter()

def TriggerSpec():
  """TriggerSpec()"""
  return _pyC67.TriggerSpec()

def MoveAbsolute(*args):
  """MoveAbsolute(int axis, float value)"""
  return _pyC67.MoveAbsolute(*args)

def MoveAbsoluteADU(*args):
  """MoveAbsoluteADU(int axis, int value)"""
  return _pyC67.MoveAbsoluteADU(*args)

def DownloadProfile():
  """DownloadProfile()"""
  return _pyC67.DownloadProfile()

def Expose(*args):
  """Expose(int cameramask)"""
  return _pyC67.Expose(*args)

def MoveRelative(*args):
  """MoveRelative(int axis, float value)"""
  return _pyC67.MoveRelative(*args)

def Flash():
  """Flash()"""
  return _pyC67.Flash()

def ReadDigital():
  """ReadDigital() -> unsigned int"""
  return _pyC67.ReadDigital()

def WriteDigital(*args):
  """WriteDigital(int value)"""
  return _pyC67.WriteDigital(*args)

def Abort():
  """Abort()"""
  return _pyC67.Abort()

def UpdateNReps(*args):
  """UpdateNReps(int newCount)"""
  return _pyC67.UpdateNReps(*args)

def InitDMA():
  """InitDMA() -> unsigned int"""
  return _pyC67.InitDMA()

def SetBase(*args):
  """SetBase(int axis, float value)"""
  return _pyC67.SetBase(*args)

def ReturnFrameData(*args):
  """ReturnFrameData(unsigned char array00d)"""
  return _pyC67.ReturnFrameData(*args)

def MicronToADU(*args):
  """MicronToADU(int axis, float micron) -> unsigned int"""
  return _pyC67.MicronToADU(*args)

def ADUToMicron(*args):
  """ADUToMicron(int axis, unsigned int adu) -> float"""
  return _pyC67.ADUToMicron(*args)

def CollectionCleanup():
  """CollectionCleanup()"""
  return _pyC67.CollectionCleanup()

def GetFrameCount():
  """GetFrameCount() -> int"""
  return _pyC67.GetFrameCount()

def PrintCardInfo():
  """PrintCardInfo()"""
  return _pyC67.PrintCardInfo()

def PrintError(*args):
  """PrintError(char str)"""
  return _pyC67.PrintError(*args)

def GetBuffers():
  """GetBuffers()"""
  return _pyC67.GetBuffers()

def FreeBuffers():
  """FreeBuffers()"""
  return _pyC67.FreeBuffers()

def DmaToTarget(*args):
  """DmaToTarget(int Inbuf, int shared, int numwds)"""
  return _pyC67.DmaToTarget(*args)

def WriteCommand(*args):
  """WriteCommand(int Command)"""
  return _pyC67.WriteCommand(*args)

def WriteParam(*args):
  """WriteParam(int param)"""
  return _pyC67.WriteParam(*args)
# This file is compatible with both classic and new-style classes.


def InitProfile(*args):
  """
    InitProfile(int reps, HANDLE UsrEvent = 0)
    InitProfile(int reps) -> void
    """
  return _pyC67.InitProfile(*args)

def ReadReply(timeoutMS = 10):
  """
    ReadReply(int timeoutMS = 10) -> int
    ReadReply() -> int
    """
  return _pyC67.ReadReply(timeoutMS)

