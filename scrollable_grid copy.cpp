#include <iostream>
#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>

const int ROWS = 10;
const int COLS = 10;

// Function to get single character input without waiting for the Enter key
int getch() {
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

void clearScreen() {
    std::system("clear");
}

void drawGrid(const std::vector<std::vector<char>>& grid, int rowOffset, int colOffset) {
    clearScreen();
    for (int i = rowOffset; i < rowOffset + ROWS; ++i) {
        for (int j = colOffset; j < colOffset + COLS; ++j) {
            std::cout << grid[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

int main() {
    int totalRows = 20;
    int totalCols = 20;

    std::vector<std::vector<char>> grid(totalRows, std::vector<char>(totalCols, '.'));

    int rowOffset = 0;
    int colOffset = 0;
    bool running = true;

    drawGrid(grid, rowOffset, colOffset);

    while (running) {
        int key = getch();

        switch (key) {
            case 'q':
                running = false;
                break;
            case 'w':
                if (rowOffset > 0) {
                    --rowOffset;
                }
                break;
            case 's':
                if (rowOffset < totalRows - ROWS) {
                    ++rowOffset;
                }
                break;
            case 'a':
                if (colOffset > 0) {
                    --colOffset;
                }
                break;
            case 'd':
                if (colOffset < totalCols - COLS) {
                    ++colOffset;
                }
                break;
            default:
                if (key >= 32 && key <= 126) {  // Check if it's a printable ASCII character
                    grid[rowOffset][colOffset] = static_cast<char>(key);
                }
                break;
        }
        drawGrid(grid, rowOffset, colOffset);
    }

    return 0;
}
