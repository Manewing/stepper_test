import sys
import time
import serial
from thread import start_new_thread

def error(msg):
    sys.stderr.write(msg+"\n")

def echo(ser):
    while True:
        num = ser.inWaiting()
        data = ser.read(num)
        sys.stdout.write(data)
        sys.stdout.flush()

def write_ser(ser, line):
    line = line.replace("\n", "").replace("\r", "")
    ser.write(line)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        error("not enough arguments: serial port missing")
        sys.exit(-1)

    serial_name = sys.argv[1]
    try:
        ser = serial.Serial(serial_name)
    except:
        error("could not open serial port")
        sys.exit(-1)

    start_new_thread(echo, (ser,))

    while True:
        line = raw_input()
        elems = line.split(" ")
        if elems[0] == "file":
            with open(elems[1], "r") as f:
                lines = f.readlines()
            for line in lines:
                sys.stdout.write(line)
                write_ser(ser, line)
                time.sleep(2)
            continue
        write_ser(ser, line)
