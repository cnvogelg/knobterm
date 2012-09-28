#!/usr/bin/env python2.7

import Image, ImageFont, ImageDraw
import sys
import argparse

# ----- functions -----

def parse_num(s):
  if s.startswith('0x'):
    return int(s[2:],16)
  else:
    return int(s)

def raster_char(draw, font, c, w, h):
  draw.rectangle((0,0,w,h), fill=0)
  if c != 0:
    draw.text((0,0), unichr(c), font=font, fill=1)

def copy_char(image, map_image, xp, yp, gx, gy, sax, say):
  sy = 0
  ty = yp
  for y in xrange(gy):
    sx = 0
    tx = xp
    for x in xrange(gx):
      v = image.getpixel((sx,sy))
      map_image.putpixel((tx,ty),v)
      sx += sax
      tx += 1
    sy += say
    ty += 1

def convert_raw(raw_data, map_size, gx, gy):
  res = ""
  gw = gx / 8 # bytes per char in each scan line
  step = map_size * gw
  for i in range(map_size):
    src_off = gw * i
    for y in range(gy):
      off = src_off + y * step
      for b in range(gw):
        res += raw_data[off]
        off += 1
  return res

def write_hex(f, data, per_line=8, form="0x%02x, "):
  n=len(data)
  lines=n / per_line
  remainder=n % per_line
  off = 0
  for l in range(lines):
    line = ""
    for i in range(per_line):
      line += form % ord(data[off])
      off += 1
    ch = "   "
    if l > 31 and l < 128:
      ch = "'%c'" % chr(l)  
    line += " /* %3d/0x%02x %s */\n" % (l,l,ch)
    f.write(line)

  if remainder > 0:
    line = ""
    for i in range(remainder):
      line += form % ord(data[off])
      off += 1
    line += "\n"
    f.write(line)

masks = (0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01)

def tobits(i):
  res = ""
  for m in masks:
    if (i & m) == m:
      res += "*"
    else:
      res += "."
  return res

def write_ascii(f, data, map_size, gx, gy):
  gw = gx / 8
  off = 0
  for i in range(map_size):
    f.write("Code: %3d/0x%02x\n" % (i,i))
    for y in range(gy):
      line = ""
      for x in range(gw):
        d = ord(data[off])
        line += tobits(d) + " %02x  " % d 
        off += 1
      line += "\n"
      f.write(line)

# ----- main -----

parser = argparse.ArgumentParser()
parser.add_argument('font', help="TrueType font file")
parser.add_argument('-s', '--font-size', type=int, default=8, help="set font size")
parser.add_argument('-x', '--grid-x', type=int, default=8, help="width of character grid")
parser.add_argument('-y', '--grid-y', type=int, default=8, help="height of character grid")
parser.add_argument('-X', '--sample-x', type=int, default=1, help="sample factor in x direction")
parser.add_argument('-Y', '--sample-y', type=int, default=1, help="sample factor in y direction")
parser.add_argument('-t', '--map-size', type=int, default=256, help="set number of chars in map")
parser.add_argument('-c', '--columns', type=int, default=0, help="columns in PNG image")
parser.add_argument('-m', '--mapping', action='append', help="map characters: <offset>:<num>:<unicode offset>")
parser.add_argument('-R', '--raw-file', default=None, help="save raw data dump")
parser.add_argument('-C', '--c-file', default=None, help="save hexdump data dump in C format")
parser.add_argument('-A', '--ascii-file', default=None, help="save ASCII dump")
parser.add_argument('-P', '--png-file', default=None, help="save PNG image")
args = parser.parse_args()

# parse args
font_name = args.font
font_size = args.font_size
map_size  = args.map_size
gx = args.grid_x
gy = args.grid_y
sx = args.sample_x
sy = args.sample_y

# parse mapping
mapping = []
if args.mapping == None:
  mapping.append((0,256,0))
else:
  for m in args.mapping:
    a = m.split(':')
    mapping.append(map(lambda x : parse_num(x), a))

# determine size of final character map
image_width = map_size * gx
image_height = gy
image_size = image_width * image_height
byte_size = image_size / 8
box_w = sx * gx
box_h = sy * gy

# png image size
cols = args.columns
rows = 1
if cols == 0:
  png_w = image_width
  png_h = image_height
  cols = map_size
else:
  rows = map_size / cols
  png_w = gx * cols
  png_h = gy * rows

print("Font File:       '%s'" % font_name)
print("Font Size:       %d" % font_size)
print("Character Map:   %d" % map_size)
print("Image Size:      %d x %d x %d = %d x %d" % (map_size, gx, gy, image_width, image_height))
print("Raw Bytes:       %d" % byte_size)
print("Raster Box Size: %d x %d" % (box_w, box_h))
print("Sampling:        %d x %d" % (sx, sy))
print("PNG Size:        %d x %d" % (png_w, png_h))

# create image
map_image = Image.new("1",(image_width,image_height))
image = Image.new("1", (box_w, box_h))
png_image = Image.new("1", (png_w, png_h))
draw = ImageDraw.Draw(image)
font = ImageFont.truetype(font_name, font_size)

# raster all mappings
for m in mapping:
  begin = m[0]
  num = m[1]
  uni_offset = m[2]
  print("Mapping: [0x%02x ... 0x%02x] codepoint +@0x%04x" % (begin, begin+num-1, uni_offset))
  code = begin
  xp = code * gx
  yp = 0
  for i in xrange(num):
    raster_char(draw, font, uni_offset + i, box_w, box_h)
    copy_char(image, map_image, xp, yp, gx, gy, sx, sy)
    px = (code % cols) * gx
    py = (code / cols) * gy
    copy_char(image, png_image, px, py, gx, gy, sx, sy)
    code += 1
    xp += gx

# export font data
if args.png_file != None:
  print("Writing PNG: '%s'" % args.png_file)
  png_image.save(args.png_file)
    
# raw data
raw_data = map_image.tostring()
raw_data = convert_raw(raw_data, map_size, gx, gy)
raw_size = len(raw_data)
raw_file = args.raw_file
if raw_file != None:
  print("Writing RAW: '%s' (%d bytes)" % (raw_file, raw_size))
  f = open(raw_file,"wb")
  f.write(raw_data)
  f.close()

# c-file
c_file = args.c_file
if c_file != None:
  print("Writing C: '%s' (%d bytes)" % (c_file, raw_size))
  f = open(c_file,"w")
  write_hex(f, raw_data, per_line=gy)
  f.close()

# ASCII file
ascii_file = args.ascii_file
if ascii_file != None:
  print("Writing ASCII: '%s" % ascii_file)
  f = open(ascii_file,"w")
  write_ascii(f, raw_data, map_size, gx, gy)
  f.close()
