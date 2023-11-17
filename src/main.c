#include <gba_video.h>
#include <gba_console.h>

int main() {


    consoleDemoInit();

    // ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[10;10HHello World!");

}