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
    
  def handle_events(self):
    ev = pygame.event.wait()
    return ev.type in (pygame.QUIT, pygame.KEYDOWN)

  def quit(self):
    pygame.display.quit()