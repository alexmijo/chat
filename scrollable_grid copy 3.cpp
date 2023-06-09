#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

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

void clearScreen() { std::system("clear"); }

void drawGrid(const std::vector<std::vector<char>> &grid, int rowOffset, int colOffset, int rowEditableOffset, int colEditableOffset, bool moveEditableMode) {
    clearScreen();
    for (int i = rowOffset; i < rowOffset + ROWS; ++i) {
        for (int j = colOffset; j < colOffset + COLS; ++j) {
            if (i == rowOffset + rowEditableOffset && j == colOffset + colEditableOffset) {
                if (moveEditableMode) {
                    std::cout << "\033[1;32m" << grid[i][j] << "\033[0m "; // Highlight the editable character in green
                } else {
                    std::cout << "\033[1;31m" << grid[i][j] << "\033[0m "; // Highlight the editable character in red
                }
            } else {
                std::cout << grid[i][j] << ' ';
            }
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
    int rowEditableOffset = ROWS / 2;
    int colEditableOffset = COLS / 2;
    bool moveEditableMode = false;
    bool running = true;

    drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset, moveEditableMode);

    while (running) {
        int key = getch();

        switch (key) {
        case 'q':
            // running = false;
            moveEditableMode = !moveEditableMode;
            break;
        case 'w':
            if (moveEditableMode) {
                if (rowEditableOffset > 0) {
                    --rowEditableOffset;
                }
            } else if (rowOffset > 0) {
                --rowOffset;
            }
            break;
        case 's':
            if (moveEditableMode) {
                if (rowEditableOffset < ROWS - 1) {
                    ++rowEditableOffset;
                }
            } else if (rowOffset < totalRows - ROWS) {
                ++rowOffset;
            }
            break;
        case 'a':
            if (moveEditableMode) {
                if (colEditableOffset > 0) {
                    --colEditableOffset;
                }
            } else if (colOffset > 0) {
                --colOffset;
            }
            break;
        case 'd':
            if (moveEditableMode) {
                if (colEditableOffset < COLS - 1) {
                    ++colEditableOffset;
                }
            } else if (colOffset < totalCols - COLS) {
                ++colOffset;
            }
            break;
        default:
            if (key >= 32 && key <= 126) { // Check if it's a printable ASCII character
                grid[rowOffset + rowEditableOffset][colOffset + colEditableOffset] =
                    static_cast<char>(key);
            }
            break;
        }
        drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset, moveEditableMode);
    }

    return 0;
}
