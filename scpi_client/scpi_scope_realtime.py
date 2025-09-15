# scpi_minimal_realtime.py  (pip install pyqtgraph PyQt5)
import socket, numpy as np, pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore

IP, PORT   = "192.168.1.15", 5010
FS_HZ      = 125e6                       # sample rate of the adc
SCALE_VLSB = 0.976 / 8192.0              # V / LSB
UPDATE_MS  = 500                        
N_SAMPLES  = 10_000                      # length x

center = (N_SAMPLES - 1) / 2.0
t_us   = ((np.arange(N_SAMPLES) - center) / FS_HZ) * 1e6

# --- single persistent connection + line reader ---
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(1000000); s.connect((IP, PORT))
f = s.makefile('rb')
s.sendall(b"*IDN?\r\n"); print(f.readline().decode(errors="ignore").strip())

#---------------------------------------------------------------------------------------

# UI + plot (parameters of the graph) 

app = pg.mkQApp("SCPI Realtime")                 # create/reuse Qt app
pg.setConfigOption('background', 'k')            # black background (k=black y=yellow w=white etc)
pg.setConfigOption('foreground', 'w')            # white axes/text
w = pg.PlotWidget(title="scopech1 — realtime")
w.setLabel("bottom", "Time", "µs")
w.setLabel("left", "Voltage", "V")
w.showGrid(x=True, y=True)
curve = w.plot([], [])                           # the curve

w.setYRange(-1, 1, padding=0)                    # fix Y to [-1; +1] V
w.setXRange(t_us[0], t_us[-1], padding=0)        # fix X to full 80 µs span
w.show()

#----------------------------------------------------------------

#function to update the data on the graph

def update():
    s.sendall(b"scopech1?\r\n")             #scpi command
    line = f.readline()                     #read a line until \n
    if not line: return                     #if error we stop

    raw  = line.rstrip(b"\r\n").decode('ascii', 'ignore') #decode the lines and remove \r\n
    vals = np.fromstring(raw, sep=',', dtype=np.int32)
    if vals.size != N_SAMPLES: return              # ignore unexpected size (if not 10 000 samples)
    y = vals.astype(np.float64) * SCALE_VLSB
    curve.setData(t_us, y)


#------------------------------------

#Timer to call update again and again

timer = QtCore.QTimer(); 
timer.timeout.connect(update); 
timer.start(UPDATE_MS)
QtWidgets.QApplication.instance().exec()
