#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void enableRawMode();
void disableRawMode();

struct termios orig_termios;

int main() {
  enableRawMode();

  char c;
  while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw;

  tcgetattr(STDIN_FILENO, &raw);

  // bit flipping the control flag struct
  raw.c_lflag &= ~(ECHO | ICANON); //  echo -turns off each key being printed to the terminal
                          // ICANON - turns off cannonical mode
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
