from . import display
from . import font
from . import palette
from . import draw
import os
import time
import pygame

class KnobEmu:
  """The KnobTerm emulator"""
  font_names = ('topaz','c64')
  
  def __init__(self, font_dir=None, data_dir=None):
    self.display = display.Display()
    self.fonts = []
    self._setup_fonts(font_dir)
    self.cx = 0
    self.cy = 0
    self.width = 40
    self.height = 30
    self.fg = 1
    self.bg = 0
    self.fi = 0 # font index
    self.fx = 1
    self.fy = 1
    for f in self.fonts:
      f.set_color(palette.Palette,self.fg,self.bg)
    self.draw_font = self.fonts[1]
    # setup data dir
    if data_dir == None:
      data_dir = os.path.join(os.path.dirname(__file__),"..","..","..","data")
    self.data_dir = data_dir
    
  def _setup_fonts(self, font_dir):
    # set font path
    if font_dir == None:
      font_dir = os.path.join(os.path.dirname(__file__),"..","..","..","util")
    for f in self.font_names:
      font_path = os.path.join(font_dir, f) + ".png"
      if not os.path.exists(font_path):
        raise IOError("font not found: "+font_path)
      fo = font.Font(font_path, self.display.screen)
      self.fonts.append(fo)

  def _newline(self):
    self.cx = 0
    if (self.cy + self.fy) < self.height:
      self.cy += self.fy

  # ----- KnobTerm API -----

  def open(self):
    return True

  def close(self):
    self.display.quit()
  
  def get_version(self):
    return "1.0"
  
  def get_size(self):
    return (40,30)
  
  def text(self, t):
    f= self.fonts[self.fi]
    for ch in t:
      if ch == '\n':
        self._newline()
      else:
        f.draw_char((self.cx,self.cy),ord(ch))
        self.cx += self.fx
        if self.cx == self.width:
          self._newline()
    
    self.display.show()

  def sync(self):
    self.display.show()

  def goto(self, x, y):
    self.cx = x
    self.cy = y
    
  def set_color_fg(self, fg):
    self.fg = fg
    for f in self.fonts:
      f.set_color(palette.Palette,self.fg, self.bg)
  
  def set_color_bg(self, bg):
    self.bg = bg
    for f in self.fonts:
      f.set_color(palette.Palette,self.fg, self.bg)
  
  def set_color(self, fg, bg):
    self.fg = fg
    self.bg = bg
    for f in self.fonts:
      f.set_color(palette.Palette,self.fg, self.bg)

  def set_flags(self, f):
    # todo
    pass
    
  def set_font_scale(self, x, y):
    for f in self.fonts:
      f.set_scale(x, y)
    if x:
      self.fx = 2
    else:
      self.fx = 1
    if y:
      self.fy = 2
    else:
      self.fy = 1

  def set_font_map(self, num):
    self.fi = num

  def erase(self, col):
    self.display.screen.fill(palette.Palette[col])
    self.display.show()

  def draw_border(self, t, x, y, w, h):
    draw.draw_border(self.draw_font, t, x, y, w, h)
    self.display.show()
  
  def draw_rect(self, ch, x, y, w, h):
    draw.draw_rect(self.draw_font, ord(ch), x, y, w, h)
    self.display.show()
  
  def draw_grid(self, t, x, y, nx, ny, dx, dy):
    draw.draw_grid(self.draw_font, t, x, y, nx, ny, dx, dy)
    self.display.show()
  
  def draw_h_line(self, ch, x, y, l):
    draw.draw_h_line(self.draw_font, ord(ch), x, y, l)
    self.display.show()

  def draw_v_line(self, ch, x, y, l):
    draw.draw_v_line(self.draw_font, ord(ch), x, y, l)
    self.display.show()

  def chunk_define(self, t, x, y, w, h):
    self.chunk = (t, x, y, w, h)
  
  def chunk_draw(self, s, h, data):
    delay = 0.0016 # measured delta per char
    (t, x, y, w, h) = self.chunk
    if t == 1:
      # color + text
      off = 0
      yp = y
      cur_fg = self.fg
      cur_bg = self.bg
      for l in range(h):
        xp = x
        for i in range(w):
          col = ord(data[off])
          ch = ord(data[off+1])
          fg = (col / 16) & 0xf
          bg = col & 0xf
          if (fg != cur_fg) or (bg != cur_bg):
            self.draw_font.set_color(palette.Palette,fg,bg)
            cur_fg = fg
            cur_bg = bg
          self.draw_font.dc(xp,yp,ch)
          off += 2
          xp += 1
        yp += 1
        # simulate duration of chunk line transfer
        time.sleep(delay * w)
      self.draw_font.set_color(palette.Palette,self.fg, self.bg)
    else:
      # text only
      yp = y
      off = 0
      for l in range(h):
        xp = x
        for i in range(w):
          ch = data[off]
          self.draw_font.dc(xp,yp,ord(ch))
          xp += 1
          off += 1
        yp += 1
        # simulate duration of chunk line transfer
        time.sleep(delay * w)
    self.display.show()

  def _simulate_rgb565(self,surface):
    w = surface.get_width()
    h = surface.get_height()
    for y in range(h):
      for x in range(w):
        c = surface.get_at((x,y))
        # need swizzle here?! bug?
        r = c.b & ~0x7
        g = c.g & ~0x3
        b = c.r & ~0x7
        surface.set_at((x,y),pygame.Color(r,g,b,255))

  def picture_load(self, x, y, name):
    path = os.path.join(self.data_dir,name)
    if not os.path.exists(path):
      return 0x11 # PIC_OPEN_ERROR
    else:
      surface = pygame.image.load(path)
      self._simulate_rgb565(surface)
      self.display.screen.blit(surface,(x*8,y*8))
      self.display.show()
      return 0

  # event handling
  
  def _make_event(self, t):
    if t in ('+','-'):
      return {'type':t,'value':1}
    else:
      return {'type':t,'value':0}
  
  def get_next_event(self, timeout=0):
    """return the next event or None if no event is available"""
    t = self.display.get_next_event(timeout)
    if t != None:
      return self._make_event(t)
    else:
      return None

  def wait_for_event(self):
    """wait until an event occurs and return it"""
    t = self.display.wait_for_event()
    return self._make_event(t)
