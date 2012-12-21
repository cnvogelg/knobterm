from list import List

class Menu:
  def __init__(self, x, y, w, title, entries):
    self.list = List(x,y,w, title, entries)
    self.n = len(entries)
  
  def _invert_pick(self, gc, i):
    l = self.list.get_label(i)
    l.invert()
    l.draw(gc)
  
  def begin(self, kt):
    self.pick = 0
    self.list.draw(kt)
    self._invert_pick(kt, self.pick)
  
  def end(self, kt):
    self._invert_pick(kt, self.pick)
  
  def handle(self, kt, ev):
    """handle one event of the menu.
       return False to abort menu, True to leave menu or None to stay
    """
    if ev == 'q':
      return False
    elif ev == 'b':
      return True
    elif ev == '-':
      if self.pick > 0:
        self._invert_pick(kt, self.pick)
        self.pick -= 1
        self._invert_pick(kt, self.pick)
    elif ev == '+':
      if self.pick < (self.n-1):
        self._invert_pick(kt, self.pick)
        self.pick += 1
        self._invert_pick(kt, self.pick)
    return None 
    
  def pick(self, kt):
    """pick a menu entry and return index of selection or None if menu was aborted"""
    result = None
    self.begin(kt)
    while True:
      ev = kt.wait_for_event()
      res = self.handle(kt, ev)
      if res == True:
        result = self.pick
        break
      elif res == False:
        break
    self.end(kt)
