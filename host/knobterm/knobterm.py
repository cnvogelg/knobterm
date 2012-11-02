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

  def sync(self):
    self.write('@s;')
    res = self.read_line()
    return res == '@s'

  def goto(self, x, y):
    cmd = "@g%02x%02x;" % (x,y)
    self.write(cmd)
    
  def color_fg(self, fg):
    cmd = "@c%x;" % (fg)
    self.write(cmd)
    
  def color(self, fg, bg):
    cmd = "@c%x%x;" % (fg, bg)
    self.write(cmd)

  def flags(self, f):
    cmd = "@f%02x;" % (f)
    self.write(cmd)
    
  def font_scale(self, x, y):
    n = 'n'
    if x and y:
      n = 'b'
    elif x:
      n = 'x'
    elif y:
      n = 'y'
    cmd = "@f%s;" % n
    self.write(cmd)

  def font_map(self, num):
    n = chr(65 + num)
    cmd = "$@f%s;" % n
    self.write(cmd)

  def erase(self, col):
    cmd = "@e%x;" % (col)
    self.write(cmd)
    res = self.read_line()
    return res == "@e00"

  def draw_border(self, type, x, y, w, h):
    cmd = "@db%c%02x%02x%02x%02x;" % (chr(65+type),x,y,w,h)
    self.write(cmd)
    res = self.read_line()
    return res == "@d00"
  
  def draw_rect(self, ch, x, y, w, h):
    cmd = "@dr%c%02x%02x%02x%02x;" % (ch,x,y,w,h)
    self.write(cmd)
    res = self.read_line()
    return res == "@d00"
  
  def draw_grid(self, type, x, y, nx, ny, dx, dy):
    cmd = "@dg%c%02x%02x%02x%02x%02x%02x;" % (chr(65+type),x,y,nx,ny,dx,dy)
    self.write(cmd)
    res = self.read_line()
    return res == "@d00"
    

