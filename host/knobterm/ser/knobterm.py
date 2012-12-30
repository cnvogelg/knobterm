from __future__ import print_function
import serial
import time
import sys

class KnobTerm:
  def __init__(self, serial_port, serial_baud=57600, serial_timeout=1, debug=False):
    """create and open the serial link to the knob term"""
    self.serial_port = serial_port
    self.serial_baud = serial_baud
    self.serial_timeout = serial_timeout
    self.debug = debug
  
  def _write(self, s):
    if sys.version < '3':
      b = s
    else:
      b = s.encode('latin-1')
    self.ser.write(b)
    if self.debug:
      o = s.replace('\n','\\n')
      print("W: '%s'" % o)
    
  def _read_line(self, block=True, timeout=None):
    if not block:
      if self.ser.inWaiting() == 0:
        return None
    line = b""
    start_time = time.time()
    while True:
      c = self.ser.read()
      if len(c) == 0:
        # timeout handling
        if timeout != None:
          delta = time.time() - start_time
          if delta >= timeout:
            return None
      elif c == b'\n':
        break
      else:
        line += c
    if sys.version < '3':
      return line
    else:
      return line.decode('latin-1')

  # ----- KnobTerm API -----

  def open(self, timeout=5):
    # reset and wait for init command
    self.ser = serial.Serial(self.serial_port, self.serial_baud, timeout=self.serial_timeout)
    l = self._read_line(timeout=timeout)
    if l == None:
      return False
    if l != "@i":
      return False
    return True

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

  def sync(self, timeout=None):
    self._write('@s;')
    res = self._read_line(timeout=timeout)
    return res == '@s'

  def goto(self, x, y):
    cmd = "@g%02x%02x;" % (x,y)
    self._write(cmd)
    
  def set_color_fg(self, fg):
    cmd = "@c%x;" % (fg)
    self._write(cmd)

  def set_color_bg(self, bg):
    cmd = "@c-%x;" % (bg)
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
  
  def chunk_define(self, t, x, y, w, h):
    cmd = "@dc%c%02x%02x%02x%02x;" % (chr(65+t),x,y,w,h)
    self._write(cmd)
    res = self._read_line()
    return res == "@d00"
  
  def chunk_draw(self, s, h, data):
    cmd = "@dC;"
    self._write(cmd)
    off = 0
    for y in range(h):
      line = data[off:off+s]
      off += s
      self._write(line)
      res = self.ser.read()
      if res != b'#':
        raise IOError("Error writing chunk")
  
  def picture_load(self, x, y, name):
    cmd = "@pl%02x%02x%s;" % (x,y,name)
    self._write(cmd)
    res = self._read_line()
    if res == '@p00':
      return 0
    elif res[0:3] == '@Ep':
      return int(res[3:],16)
    else:
      raise IOError("Error loading picture")
  
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

