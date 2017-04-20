/*** includes **/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

/*** fn defs **/
void enableRawMode();
void disableRawMode();
void die(const char*);
char editorReadKey();
void editorProcessKeypress();
void editorRefreshScreen();
void editorDrawRows();
void initEditor();
int getWindowSize(int*, int*);

/*** defines ***/
// MACRO bitwise-AND's a value with 0b00011111
#define CTRL_KEY(k) ((k) & 0x1f)

/*** data **/

struct editorConfig {
  int screenrows;
  int screencols;
  struct termios orig_termios;
};

struct editorConfig E;



/*** init **/
int main()
{
  enableRawMode();
  initEditor();

  while(1) {
    /* char c = '\0'; */
    /* if(read(STDIN_FILENO, &c, 1) == -1 & errno != EAGAIN) die("read"); */
    /* if(iscntrl(c)){ */
    /*   printf("%d\r\n", c); // output processing is turned off, need return */
    /* } else { */
    /*   printf("%d ('%c')\r\n", c, c); */
    /* } */
    /* if(c == CTRL_KEY('q')) break; */
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}

void initEditor()
{
  if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

/*** terminal  **/
void enableRawMode()
{
  if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1 ) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = E.orig_termios;

  tcgetattr(STDIN_FILENO, &raw);

  // bit flipping the control flag struct
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  // ECHO -turns off each key being printed to the terminal
  // ICANON - turns off cannonical mode
  // ISIG - turns off sending of signals with keyboard
  // IEXTEN -  handle ctrl-o and ctrl-v
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  // IXON turns off the flow control for CTRL-S / CTRL-Q
  // ICRNL turns off converting carriage returns to newline
  // BRKINT
  // INPCK turns off converting carriage returns to newline
  raw.c_oflag &= ~(OPOST); // OPOST - Turn off all output processing
  raw.c_cflag &= ~(CS8);
  raw.c_cc[VMIN] = 0; // minimium number of bytes for read to return
  raw.c_cc[VTIME] = 1; // Timeout on waiting for read to return
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");

}

void disableRawMode()
{
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  perror(s);
  exit(1);
}

char editorReadKey()
{
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

int getWindowSize(int *rows, int *cols)
{
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** output **/
void editorRefreshScreen()
{
  write(STDOUT_FILENO, "\x1b[2J", 4); // writing 4 bytes,
                      // 1. \x1b - (escape character) - 27 in decimal
                      // 2. [ - (escape character)
                      // 3. 2 - (arg) - clear the entire display (1 is up to the cursor, 0 from the cursor up to the end)
                      // 4. J - (command)Erase In Display
  write(STDOUT_FILENO, "\x1b[H", 3); // return cursor to 1,1 - default arguments for cursor position
  editorDrawRows();

  write(STDOUT_FILENO, "\x1b[H", 3);
}

void editorDrawRows()
{
  int y;
  for (y = 0; y < E.screenrows; y++) { // get height of terminal?
    write(STDOUT_FILENO, "j\r\n", 3);
  }
}

/*** input **/

void editorProcessKeypress()
{
  char c = editorReadKey();

  switch(c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
  }
}
