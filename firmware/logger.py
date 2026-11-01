"""Save the board's CSV stream to a timestamped file.
Type a label letter + Enter to change the label column.
usage: python logger.py COM5   (or /dev/ttyACM0)    needs: pip install pyserial"""
import sys, time, threading, serial
port = sys.argv[1]
ser = serial.Serial(port, 115200, timeout=1)
name = time.strftime("session_%Y%m%d_%H%M%S.csv")
def keys():
    for line in sys.stdin:
        for ch in line.strip(): ser.write(ch.encode())
threading.Thread(target=keys, daemon=True).start()
print("logging to", name, "| Ctrl-C stops")
with open(name, "w") as f:
    try:
        while True:
            l = ser.readline().decode(errors="ignore").strip()
            if l.count(",") == 5: f.write(l + "\n"); f.flush()
    except KeyboardInterrupt: pass
