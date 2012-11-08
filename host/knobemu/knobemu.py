import display
import font
import palette
import draw
import os

class KnobEmu:
  """The KnobTerm emulator"""
  font_names = ('topaz','c64')
  
  def __init__(self, font_dir=None):
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
    
  def _setup_fonts(self, font_dir):
    # set font path
    if font_dir == None:
      font_dir = os.path.join(os.path.dirname(__file__),"..","..","util")
    for f in self.font_names:
      font_path = os.path.join(font_dir, f) + ".png"
      if not os.path.exists(font_path):
        raise IOError("font not found: "+font_path)
      fo = font.Font(font_path, self.display.screen)
      self.fonts.append(fo)

  def handle_events(self):
    return self.display.handle_events()

  def _newline(self):
    self.cx = 0
    if (self.cy + self.fy) < self.height:
      self.cy += self.fy

  # ----- KnobTerm API -----

  def close(self):
    self.display.quit()
  
  def get_version(self):
    return "1.0"
  
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
    
  def color_fg(self, fg):
    self.fg = fg
    for f in self.fonts:
      f.set_color(palette.Palette,self.fg, self.bg)
      
  def color(self, fg, bg):
    self.fg = fg
    self.bg = bg
    for f in self.fonts:
      f.set_color(palette.Palette,self.fg, self.bg)

  def flags(self, f):
    # todo
    pass
    
  def font_scale(self, x, y):
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

  def font_map(self, num):
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