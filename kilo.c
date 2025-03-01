/*** includes ***/

#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/*** data ***/

struct termios original_termios;


/*** terminal ***/

void die(const char * s) {
	perror(s);
	exit(1);
}

void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) {
		die("tcsetattr");
	}
}

void enableRawMode() {
	if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
		die("tcgetattr");
	}
	atexit(disableRawMode);
	struct termios raw = original_termios;
	raw.c_iflag &= ~( BRKINT | INPCK |  ICRNL |  IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag &= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN );
 	if( tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1 ) {
		die("tcsetattr");
	}
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
}

/*** init ***/

int main() {
	enableRawMode();
	while (1) {
		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
			die("read");
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d, ('%c') \r\n", c, c);
		}
		if (c == 'q')
			break;
	}
	return 0;
}
