# font generation

# Amiga source: http://spetsnaz.su/dmg/html/fonts.html
TOPAZ_FONT=TopazPlus_a1200_v1.0.ttf
TOPAZ_OPTS=-s 16 -Y 2

# C64 source: http://style64.org/c64-truetype
C64_FONT=C64_Elite_Mono_v1.0-STYLE.ttf
C64_OPTS=-m 0x20:0x20:0xee20 -m 0x40:0x20:0xee00 -m 0x60:0x20:0xef00 -m 0x80:0x40:0xee40 -m 0xc0:0x40:0xeec0 -m 0x00:0x20:0xef60

all: c64 topaz

# Topaz Amiga ROM font
topaz: ../src/topaz.inc topaz.png

../src/topaz.inc:
	./ttf_raster.py $(TOPAZ_FONT) -C $@ $(TOPAZ_OPTS)

topaz.png:
	./ttf_raster.py $(TOPAZ_FONT) -c 16 -P topaz.png $(TOPAZ_OPTS)

# C64 ROM font
c64: ../src/c64.inc c64.png
  
../src/c64.inc:
	./ttf_raster.py $(C64_FONT) -C $@ $(C64_OPTS)
	
c64.png:
	./ttf_raster.py $(C64_FONT) -c 16 -P c64.png $(C64_OPTS)

clean: clean_inc clean_png

clean_inc:
	rm ../src/topaz.inc
	rm ../src/c64.inc

clean_png:
	rm topaz.png
	rm c64.png
