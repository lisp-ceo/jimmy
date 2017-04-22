# jimmy editor

First came vi, then emacs, then jimmy - the extensible file editor.

- Terminals
  - Raw vs cannonical mode
    - Carriage-return-buffered input
  - `tcsetattr`/`tcgetattr`
    - Edit terminal features
- Errors
  - `errno`
  - `perror`
- Macro functions
- VT100 escape sequences
  - http://vt100.net/docs/vt100-ug/chapter3.html
  - ncurses supports more via querying the terminfo database to work out supported capabilities
- `sys/ioctl.h`
  - Terminal window size operations
- `free`, `realloc`, `memcpy`
- Feature testing macros
  - https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
  - `gcc -ansi` includes only C library features unless explicitly request by feature macros
    - larger standards will clobber eachother's function names
  - preprocessor directives declare dependencies in a self-contained way
    - for each macro defined, the compiler makes more facilites available
    -

