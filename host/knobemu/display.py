import pygame

class Display:
  def __init__(self):
    pygame.init()
    pygame.display.set_caption("KnobEmu")
    self.screen = pygame.display.set_mode((320, 240), 0, 16)
    pygame.event.pump()
    self.show()
  
  def show(self):
    pygame.display.flip()
    
  def wait_for_event(self):
    while True:
      ev = pygame.event.wait()
      event = self._parse_event(ev)
      if event != None:
        return event

  def get_next_event(self, timeout=0):
    """return the next event or None if no event is available"""
    ev = self._get_next_event()
    if ev != None:
      return ev
    if timeout > 0:
      if timeout <= self.min_poll_interval:
        # timeout interval is small enough -> wait and poll again
        time.sleep(timeout)
        return self._get_next_event()
      else:
        # split timeout interval into min poll interval chunks
        num = int(timeout / self.min_poll_interval)
        for i in range(num):
          time.sleep(timeout)
          ev = self._get_next_event()
          if ev != None:
            return ev
        return None
    else:
      return None
  
  def _get_next_event(self):
    ev = pygame.event.poll()
    if ev.type == pygame.NOEVENT:
      return None
    return self._parse_event(ev)
  
  def _parse_event(self, ev):
    if ev.type == pygame.QUIT:
      return 'q'
    elif ev.type == pygame.KEYDOWN:
      # q)uit or escape
      if ev.unicode == 'q':
        return 'q'
      elif ev.key == pygame.K_ESCAPE:
        return 'q'
      # cursor up/right
      elif ev.key in (pygame.K_DOWN, pygame.K_RIGHT):
        return '+'
      # cursor down/left
      elif ev.key in (pygame.K_UP, pygame.K_LEFT):
        return '-'
      # return or space is button
      elif ev.key in (pygame.K_SPACE, pygame.K_RETURN):
        return 'b'
    else:
      return None

  def quit(self):
    pygame.display.quit()