# set_trigger.py — send a TRIG:LEV value to Red Pitaya
#
# Trigger level encoding (14-bit two’s complement, ±8191 ≡ ±0.976 V)
#
# Hex        -> Voltage (V)
# 0x3E00     -> -0.976
# 0x3900     -> -0.75
# 0X3200     -> -0.5
# 0x2100     ->  0.000
# 0x1100     ->  0.5
# 0x0800     ->  0.75
# 0x0300     ->  0.976


import socket
import sys

IP, PORT = "192.168.1.15", 5010   # adjust to your board

if len(sys.argv) < 2:
    print("Usage: python set_trigger.py <value>")
    print("Example: python set_trigger.py 0x1000   or   python set_trigger.py -800")
    sys.exit(1)

TRIG_VAL = sys.argv[1]

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((IP, PORT))
    cmd = f"TRIG:LEV {TRIG_VAL}\r\n".encode("ascii")
    s.sendall(cmd)
    print(f"[OK] Trigger level sent: {TRIG_VAL}")
