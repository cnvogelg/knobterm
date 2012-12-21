from label import Label
from consts import *

class List:
  def __init__(self, x, y, w, title, entries=None):
    self.x = x
    self.y = y
    self.w = w
    self.h = len(entries) + 2
    x += 2
    w -= 4
    self.tlabel = Label(x,y,w,title, align=Label.ALIGN_CENTER, fg=COLOR_LIGHT_GREY)
    self.labels = []
    y += 1
    self.lx = x
    self.ly = y
    self.lw = w
    if entries != None:
      self.add_entries(entries)
    
  def add_entries(self, entries):
    y = self.ly
    for e in entries:
      l = Label(self.lx,y,self.lw,e)
      self.labels.append(l)
      y += 1
    self.ly = y
  
  def add_entry(self, entry):
    l = Label(self.lx,self.ly,self.lw,entry)
    self.ly += 1
  
  def draw(self, gc):
    gc.set_color_fg(COLOR_LIGHT_GREY)
    gc.draw_border(1, self.x, self.y, self.w-2, self.h-2)
    self.tlabel.draw(gc)
    for l in self.labels:
      l.draw(gc)
  
  def get_label(self, i):
    return self.labels[i]
