import pygame

class Font:
  def __init__(self, name, output, w=8, h=8):
    self.name = name
    self.w = w
    self.h = h
    surface = pygame.image.load(name).convert(8)
    size = surface.get_size()
    fw = size[0]
    fh = size[1]
    self.nx = fw // w
    self.ny = fh // h
    surface_x = pygame.transform.scale(surface, (fw*2, fh))
    surface_y = pygame.transform.scale(surface, (fw, fh*2))
    surface_xy = pygame.transform.scale(surface, (fw*2, fh*2))
    self.total = self.nx * self.ny
    self.char_rect = (w, h)
    self.surfs = (surface, surface_x, surface_y, surface_xy)
    self.off = 0
    self.sx = 1
    self.sy = 1
    self.cw = self.w
    self.ch = self.h
    self.output = output
  
  scales = ((1,1),(2,1),(1,2),(2,2))
  
  def set_scale(self, dbl_x, dbl_y):
    self.off = 0
    if dbl_x:
      self.off += 1
    if dbl_y:
      self.off += 2
    (self.sx, self.sy) = self.scales[self.off]
    self.cw = self.w * self.sx
    self.ch = self.h * self.sy
      
  def set_color(self, palette, fg, bg):
    self.set_color_rgb(palette[fg], palette[bg])
  
  def set_color_rgb(self, fg, bg):
    for s in self.surfs:
      s.set_palette_at(0, bg)
      s.set_palette_at(127, fg)
  
  def dc(self, tx, ty, ch):
    """draw char unscaled"""
    x = (ch % self.nx) * self.w
    y = (ch // self.ny) * self.h
    r = pygame.Rect(x,y,self.w,self.h)
    p = (tx * self.w, ty * self.h)
    self.output.blit(self.surfs[0], p, area=r)
  
  def draw_char(self, pos, ch):
    x = (ch % self.nx) * self.cw
    y = (ch // self.ny) * self.ch
    r = pygame.Rect(x,y,self.cw,self.ch)
    p = (pos[0] * self.w, pos[1] * self.h)
    self.output.blit(self.surfs[self.off], p, area=r)

  def draw_string(self, pos, s):
    x = pos[0]
    y = pos[1]
    for ch in s:
      self.draw_char((x,y), ord(ch))
      x += 1
