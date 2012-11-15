from label import Label
from consts import *

class List:
  def __init__(self, x, y, w, title, entries):
    self.x = x
    self.y = y
    self.w = w
    self.h = len(entries) + 2
    x += 2
    w -= 4
    self.tlabel = Label(x,y,w,title, align=Label.ALIGN_CENTER, fg=COLOR_LIGHT_GREY)
    self.labels = []
    y += 1
    for e in entries:
      l = Label(x,y,w,e)
      self.labels.append(l)
      y += 1
  
  def draw(self, gc):
    gc.set_color_fg(COLOR_LIGHT_GREY)
    gc.draw_border(1, self.x, self.y, self.w-2, self.h-2)
    self.tlabel.draw(gc)
    for l in self.labels:
      l.draw(gc)
    