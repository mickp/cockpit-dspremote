import Pyro4
import dsp
import time
import traceback


dspServer = dsp.d("OMX_release.out")

print "providing dsp.d() as [pyroDSP] on port 7766"
print "Started program at",time.strftime("%A, %B %d, %I:%M %p")
try:
    daemon = Pyro4.Daemon(port = 7766, host = '192.168.1.4')
    Pyro4.Daemon.serveSimple({dspServer: 'pyroDSP'},
            daemon = daemon, ns = False, verbose = True)

except Exception, e:
    print "Error serving DSP:",e
    traceback.print_exc()
