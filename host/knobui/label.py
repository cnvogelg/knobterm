from consts import *

class Label:
  
  ALIGN_LEFT = 0
  ALIGN_CENTER = 1
  ALIGN_RIGHT = 2
  
  CROP_LEFT = 0
  CROP_RIGHT = 1
  
  SCALE_NONE = 0
  SCALE_X = 1
  SCALE_Y = 2
  SCALE_XY = 3
  
  def __init__(self, x, y, w, text, 
               align=ALIGN_LEFT, crop=CROP_RIGHT, scale=SCALE_NONE, 
               fg=COLOR_WHITE, bg=COLOR_BLACK):
    self.x = x
    self.y = y
    self.w = w
    self.align = align
    self.crop = crop
    self.scale = scale
    self.fg = fg
    self.bg = bg
    if scale in (self.SCALE_X, self.SCALE_XY):
      self.fx = 2
      self.w //= 2
    else:
      self.fx = 1
    self.pos = x
    self.set_text(text)
    
  def set_color(self, fg=None, bg=None):
    if fg != None:
      self.fg = fg
    if bg != None:
      self.bg = bg  
  
  def invert(self):
    fg = self.bg
    bg = self.fg
    self.fg = fg
    self.bg = bg
  
  def set_text(self, text):
    n = len(text)
    # crop?
    if n > self.w:
      if self.crop == self.CROP_LEFT:
        self.text = text[n-self.w:]
      else:
        self.text = text[0:n-self.w]
    else:
      self.text = text
    # align
    if n < self.w:
      o = self.w - n
      if self.align == self.ALIGN_RIGHT:
        pos = self.x + o
      elif self.align == self.ALIGN_CENTER:
        pos = self.x + (o // 2)
      else:
        pos = self.x
    else:
      pos = self.x
    self.pos = pos * self.fx
  
  def draw(self, gc):
    gc.set_font_scale(self.scale & self.SCALE_X, self.scale & self.SCALE_Y)
    gc.set_color(self.fg, self.bg)
    gc.goto(self.pos, self.y)
    gc.text(self.text)
