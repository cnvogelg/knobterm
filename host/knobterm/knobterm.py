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
    self.write("@v\n");
    l = self.read_line()
    if l.startswith("@v"):
      return l[2:]
    else:
      return None
  
  def write(self, s):
    b = s.encode('latin-1')
    self.ser.write(b)
    
  def read_line(self, block=True):
    if not block:
      if self.ser.inWaiting() == 0:
        return None
    line = b""
    while True:
      c = self.ser.read()
      if len(c) == 0:
        raise IOError("can't read line")
      if c == b'\n':
        break
      line += c
    return line.decode('latin-1')

  def goto(self, x, y):
    cmd = "@g%02x%02x;" % (x,y)
    self.write(cmd)

  def erase(self, col):
    cmd = "@e%x;" % (col)
    self.write(cmd)
    res = self.read_line()
    return res == "@e00"

  def draw_border(self, type, x, y, w, h, col):
    cmd = "@c%02x@db%c%02x%02x%02x%02x;" % (col, chr(65+type),x,y,w,h)
    self.write(cmd)
    res = self.read_line()
    return res == "@d00"
  
  def draw_rect(self, ch, x, y, w, h, col):
    cmd = "@c%02x@dr%c%02x%02x%02x%02x;" % (col,ch,x,y,w,h)
    self.write(cmd)
    res = self.read_line()
    return res == "@d00"
  
  def draw_grid(self, type, x, y, nx, ny, dx, dy, col):
    cmd = "@c%02x@dg%c%02x%02x%02x%02x%02x%02x;" % (col, chr(65+type),x,y,nx,ny,dx,dy)
    self.write(cmd)
    res = self.read_line()
    return res == "@d00"
    

