from __future__ import print_function
import serial
import time

class KnobTerm:
  def __init__(self, serial_port, serial_baud=57600, timeout=10, debug=False):
    """create and open the serial link to the knob term"""
    self.ser = serial.Serial(serial_port, serial_baud, timeout=timeout)
    self.debug = debug
    # reset and wait for init command
    l = self._read_line()
    if l == None:
      raise IOError("no reply from terminal")
    if l != "@i":
      raise IOError("invalid init reply")
  
  def _write(self, s):
    b = s.encode('latin-1')
    self.ser.write(b)
    if self.debug:
      o = s.replace('\n','\\n')
      print("W: '%s'" % o)
    
  def _read_line(self, block=True):
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

  # ----- KnobTerm API -----

  def close(self):
    self.ser.close()
  
  def get_version(self):
    self._write("@qv\n")
    l = self._read_line()
    if l.startswith("@qv"):
      return l[3:]
    else:
      return None
  
  def get_size(self):
    self._write("@qs\n")
    l = self._read_line()
    # result: @qsWW,HH
    if l.startswith("@qs"):
      w = int(l[3:5],16)
      h = int(l[6:8],16)
      return (w,h)
    else:
      return None
  
  def text(self, t):
    t = t.replace('@','@@')
    self._write(t)
    self.sync()

  def sync(self):
    self._write('@s;')
    res = self._read_line()
    return res == '@s'

  def goto(self, x, y):
    cmd = "@g%02x%02x;" % (x,y)
    self._write(cmd)
    
  def set_color_fg(self, fg):
    cmd = "@c%x;" % (fg)
    self._write(cmd)
    
  def set_color(self, fg, bg):
    cmd = "@c%x%x;" % (fg, bg)
    self._write(cmd)

  def set_flags(self, f):
    cmd = "@f%02x;" % (f)
    self._write(cmd)
    
  def set_font_scale(self, x, y):
    n = 'n'
    if x and y:
      n = 'b'
    elif x:
      n = 'x'
    elif y:
      n = 'y'
    cmd = "@f%s;" % n
    self._write(cmd)

  def set_font_map(self, num):
    n = chr(65 + num)
    cmd = "$@f%s;" % n
    self._write(cmd)

  def erase(self, col):
    cmd = "@e%x;" % (col)
    self._write(cmd)
    res = self._read_line()
    return res == "@e00"

  def draw_border(self, type, x, y, w, h):
    cmd = "@db%c%02x%02x%02x%02x;" % (chr(65+type),x,y,w,h)
    self._write(cmd)
    res = self._read_line()
    return res == "@d00"
  
  def draw_rect(self, ch, x, y, w, h):
    cmd = "@dr%c%02x%02x%02x%02x;" % (ch,x,y,w,h)
    self._write(cmd)
    res = self._read_line()
    return res == "@d00"
  
  def draw_grid(self, type, x, y, nx, ny, dx, dy):
    cmd = "@dg%c%02x%02x%02x%02x%02x%02x;" % (chr(65+type),x,y,nx,ny,dx,dy)
    self._write(cmd)
    res = self._read_line()
    return res == "@d00"
  
  def draw_h_line(self, ch, x, y, l):
    cmd = "@dh%c%02x%02x%02x;" % (ch, x, y, l)
    self._write(cmd)
    res = self._read_line()
    return res == "@d00"

  def draw_v_line(self, ch, x, y, l):
    cmd = "@dv%c%02x%02x%02x;" % (ch, x, y, l)
    self._write(cmd)
    res = self._read_line()
    return res == "@d00"
  
  # input query
  
  def _parse_event(self, line):
    if not line.startswith('@i'):
      raise IOError("Invalid event")
    if len(line) != 5:
      raise IOError("Invalid event")
    t = line[2]
    v = int(line[3:])
    if t == '?':
      return None
    else:
      return {'type':t,'value':v}
  
  def get_next_event(self, timeout=0):
    if timeout == 0:
      cmd = "@ig;"
    else:
      t = int(timeout * 10) # in 100ms units
      if t > 255:
        t = 255
      cmd = "@iw%02x;" % t
    self._write(cmd)
    res = self._read_line()
    return self._parse_event(res)

  def wait_for_event(self):
    cmd = "@iw00;"
    self._write(cmd)
    res = self._read_line()
    return self._parse_event(res)

