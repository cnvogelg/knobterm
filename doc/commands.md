commands.txt - Control commands of knobterm
===========================================

1. Introduction
---------------

Commands are started with the `@` char and ended with `;`, `\n` (new line) or
another command `@`.

Examples:

    hello@v;world           ; command "@v;" embedded in stream
    @v\n                    ; single line command
    @v@m\n                  ; multiple commands in a row
    @v;@m;                  ; similar command sequence
    hello @@ world          ; @@ -> gives a single @

2. Command Reference
--------------------

* `v` - query version
 
   * syntax: `@v`
   * reply:  ``@v<major>.<minor>``
     
  returns the firmware version of knobterm

* `c` - set font color

  * syntax: `@c` `@c<fg_col>` `@c<fg_col><bg_col>`
  * reply: `@c<fg_col><bg_col>`
  
  colors are defined as hex values from 0..9 a..f
  
  first command queries the current set color in the console
  second command only sets the foreground color
  third command sets the foreground and background color

* `f` - set font flags

  * syntax: `@f` `@f<cmd>` `@f<hex_byte>`
  * reply: `@f<hex_byte`
  
  set flags to set size of font
  
  cmd:
  * `x` double x size
  * `y` double y size
  * `b` double x and y size
  * `n` normal size
  * `A` select font #0
  * `B` select font #1

