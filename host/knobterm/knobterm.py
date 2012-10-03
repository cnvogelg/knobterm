import serial
import time

class KnobTerm:
  def __init__(self, serial_port, serial_baud=57600, timeout=10, rtscts=0, dsrdtr=0, xonxoff=0):
    """create and open the serial link to the knob term"""
    self.ser = serial.Serial(serial_port, serial_baud, timeout=timeout)
    # reset and settle terminal
    time.sleep(2)
  
  def close(self):
    self.ser.close()
  
  def get_version(self):
    self.write("@@\n");
    self.write("@v\n");
    l = self.read_line()
    if l.startswith("@v"):
      return l[2:]
    else:
      return None
  
  def write(self, s):
    b = s.encode('latin-1')
    self.ser.write(b)
    
  def read_line(self):
    line = b""
    while True:
      c = self.ser.read()
      if len(c) == 0:
        raise IOError("can't read line")
      if c == b'\n':
        break
      line += c
    return line.decode('latin-1')
