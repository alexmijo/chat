#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <cstdlib>
#include <cmath>

using namespace std;

int _getch() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

void clear_screen() {
    cout << "\033[2J\033[1;1H";
}

int main() {
    const int width = 40;
    const int height = 20;
    int x = width / 2;
    int y = height / 2;
    int dx = 1;
    int dy = 1;

    fd_set readfds;
    struct timeval tv;
    int retval;

    while (true) {
        clear_screen();

        // Update position
        x += dx;
        y += dy;

        // Check for collisions with screen boundaries and reverse direction if needed
        if (x == 0 || x == width - 2) {
            dx = -dx;
        }
        if (y == 0 || y == height - 2) {
            dy = -dy;
        }

        // Draw the screen
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    cout << "#";
                } else if (i == y && j == x) {
                    cout << "O";
                } else {
                    cout << " ";
                }
            }
            cout << "\n";
        }

        // Wait before updating the screen again
        this_thread::sleep_for(chrono::milliseconds(100));

        // Check for keyboard input
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

        if (retval > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            break;
        }
    }

    return 0;
}
