# draw routines

NUM_BORDERS = 3
border_chars = [
  # corners: 0=TL, 1=TR, 2=BL, 3=BR,  bars: 4=HT, 5=HB, 6=VL, 7=VR
  ( 0xb0, 0xae, 0xad, 0xbd, 0x80, 0x80, 0x82, 0x82 ),
  ( 0x95, 0x89, 0x8a, 0x8b, 0x80, 0x80, 0x82, 0x82 ),
  ( 0x8f, 0x90, 0x8c, 0xba, 0xb7, 0xaf, 0xb4, 0xaa )
]

NUM_GRIDS = 1
grid_chars = [
  # tee: 0=HD, 1=HU, 2=VR, 3=VL, 4=X 5=HC 6=VC
  ( 0xb2, 0xb1, 0xab, 0xb3, 0x9b, 0x80, 0x82 )
]

def draw_border(font, t, x, y, w, h):
  if t >= NUM_BORDERS:
    t = 0
  bc = border_chars[t]
  
  font.dc(x,y,bc[0])
  font.dc(x+w+1,y,bc[1])
  font.dc(x,y+h+1,bc[2])
  font.dc(x+w+1,y+h+1,bc[3])
  
  for i in range(1,w+1):
    font.dc(x+i,y,bc[4])
    font.dc(x+i,y+h+1,bc[5])
  
  for i in range(1,h+1):
    font.dc(x,y+i,bc[6])
    font.dc(x+w+1,y+i,bc[7])

def draw_rect(font, ch, x, y, w, h):
  for j in range(h):
    for i in range(w):
      font.dc(x+i,y+j,ch);

def draw_grid_span(font, x, y, nx, dx, cl, cs, cr, ci):
  font.dc(x,y,cl); 
  px = x + 1;
  for i in range(nx):
    if i > 0:
      font.dc(px,y,ci)
      px += 1
    if cs != 0xff:
      for j in range(dx):
        font.dc(px,y,cs)
        px += 1
    else:
      px += dx;
  font.dc(px,y,cr)

def draw_grid(font, t, x, y, nx, ny, dx, dy):
  if t >= NUM_GRIDS:
    t = 0;
  bc = border_chars[t]
  gc = grid_chars[t]

  draw_grid_span(font, x, y, nx, dx, bc[0], bc[4], bc[1], gc[0])
  y+=1

  for j in range(ny):
    if j > 0:
      draw_grid_span(font, x, y, nx, dx, gc[2], gc[5], gc[3], gc[4])
      y+=1
    draw_grid_span(font, x, y, nx, dx, bc[6], ord(' '), bc[7], gc[6])
    y+=1
  
  draw_grid_span(font, x, y, nx, dx, bc[2], bc[5], bc[3], gc[1])

def draw_h_line(font, ch, x, y, w):
  for i in range(w):
    font.dc(x, y, ch)
    x+=1

def draw_v_line(font, ch, x, y, h):
  for i in range(h):
    font.dc(x, y, ch)
    y+=1
