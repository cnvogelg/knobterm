"""knobterm init"""

def _filter_opts(opts, valid):
  res = {}
  for o in opts:
    if o in valid:
      res[o] = opts[o]
  return res

def KnobTerm(serial_port=None, **more):
  """a factory function to create either a serial or emu instance"""
  if serial_port not in (None,'emu'):
    from . import ser
    valid_opts = ('debug','serial_baud','timeout')
    opts = _filter_opts(more, valid_opts)
    return ser.KnobTerm(serial_port, **opts)
  else:
    from . import emu
    valid_opts = ('font_dir')
    opts = _filter_opts(more, valid_opts)
    return emu.KnobEmu(**opts)

# define colors
COLOR_BLACK        =0
COLOR_WHITE        =1
COLOR_RED          =2
COLOR_CYAN         =3
COLOR_PURPLE       =4
COLOR_GREEN        =5
COLOR_BLUE         =6
COLOR_YELLOW       =7
COLOR_ORANGE       =8
COLOR_BROWN        =9
COLOR_LIGHT_RED    =10
COLOR_DARK_GREY    =11
COLOR_GREY         =12
COLOR_LIGHT_GREEN  =13
COLOR_LIGHT_BLUE   =14
COLOR_LIGHT_GREY   =15

color_names = (
  'black',
  'white',
  'red',
  'cyan',
  'purple',
  'green',
  'blue',
  'yellow',
  'orange',
  'brown',
  'light red',
  'dark_grey',
  'grey',
  'light green',
  'light blue',
  'light grey'
)
