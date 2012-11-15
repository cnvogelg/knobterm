from consts import *

class Gauge:
  def __init__(self, x, y, w, h, max=100):
    self.x = x
    self.y = y
    self.w = w
    self.h = h
    self.value = 0
    self.max = max
    self.clow = COLOR_GREEN
    self.cmed = COLOR_YELLOW
    self.cmax = COLOR_LIGHT_RED
    w = self.w - 2
    self.step = int(w / 3)
    self.last_filled = 0
   
  def set_value(self, v):
    if v >= 0 and v <= self.max:
      self.value = v
  
  def draw(self, gc, full=False):
    w = self.w - 2
    h = self.h - 2
    x = self.x
    y = self.y
    
    # in full mode draw border
    if full:
      gc.set_color(COLOR_LIGHT_GREY, COLOR_BLACK)
      gc.draw_border(0,x,y,w,h)
      self.last_filled = 0

    # move inside gauge
    y += 1
    x += 1
    filled = int(self.value * w / self.max)
    b_x = x + filled
    left = filled
    if left > 0:
      # low range bar
      dlow = min(self.step, left)
      gc.set_color_fg(self.clow)
      for i in range(h):
        gc.draw_h_line('*',x,y+i,dlow)
      # med range bar
      left -= self.step
      if left > 0:
        x += self.step
        dmed = min(self.step, left)
        gc.set_color_fg(self.cmed)
        for i in range(h):
          gc.draw_h_line('*',x,y+i,dmed)
        # high range bar
        left -= self.step
        if left > 0:
          x += self.step
          gc.set_color_fg(self.cmax)
          for i in range(h):
            gc.draw_h_line('*',x,y+i,left)
        
    if self.last_filled > filled:
      # do we see old bars?
      blank = self.last_filled - filled      
      for i in range(h):
        gc.draw_h_line(' ',b_x,y+i,blank)
    
    self.last_filled = filled