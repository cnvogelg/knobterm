commands.txt - Control commands of knobterm
===========================================

1. Introduction
---------------

### Command Syntax

Commands are started with the `@` char and ended with `;`, `\n` (new line) or
another command `@`.

Examples:

    hello@v;world           ; command "@v;" embedded in stream
    @v\n                    ; single line command
    @v@m\n                  ; multiple commands in a row
    @v;@m;                  ; similar command sequence
    hello @@ world          ; @@ -> gives a single @

### Syntax Description

Type uses in syntax:

    n   hex nybble: one char 0..9 a..f    e.g.  0 a 
    b   hex byte: two char 0..9 a..f      e.g.  01 42 2a
    c   a single character

Use description in syntax:

    fg  foreground color nybble
    bg  background color nybble

  Composed usage in syntax:

    <n:fg>    nybble with foreground color   e.g.  0 f

2. Command Reference
--------------------

### System Commands

* `v` - query version
 
   * syntax: `@v`
   * reply:  ``@v<n:major>.<n:minor>``
     
  returns the firmware version of knobterm

### Font Color and Flags

* `c` - set font color

  * syntax: `@c`, `@c<n:fg>`, `@c<n:fg><n:bg>`
  * reply: `@c<n:fg><n:bg>`, none, none
  
  colors are defined as hex values from 0..9 a..f
  
  first command queries the current set color in the console.
  second command only sets the foreground color.
  third command sets the foreground and background color.

* `f` - set font flags

  * syntax: `@f`, `@f<c:cmd>`, `@f<b:flags>`
  * reply: `@f<b:flags`, none, none
  
  set flags to set size of font
  
  cmd:
  * `x` double x size
  * `y` double y size
  * `b` double x and y size
  * `n` normal size
  * `A` select font #0
  * `B` select font #1

* `e` - erase the screen

  * syntax: `@e<n:bg>`
  * reply: `@e00`
  
  clear the screen with the given color.
  
  you have to wait for the incoming reply message!
  otherwise the draw operation is not finished and the terminal is
  not ready for new commands, yet.

* `g` - goto screen position

  * syntax: `@g<b:x><b:y>`
  * reply: none
  
  move the cursor to the given position

### Draw Commands

* `db` - draw a border

  * syntax: `@db<c:style><b:x><b:y><b:w><b:h>` 
  * reply: `@d00`
  
  draw a border of the given style
  
  note: width and height give the interior size of the border
  
  styles:
  * `A` - blocky corners
  * `B` - round corners
  * `C` - larger border

* `dr` - draw a rectangle

  * syntax: `@dr<c:char<b:x><b:y><b:w><b:h>`
  * reply: `@d00`
  
  draw a rectangle of characters with the current color and flags


  