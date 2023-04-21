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

void drawGrid(const std::vector<std::vector<char>>& grid, int rowOffset, int colOffset, int curRow, int curCol) {
    clearScreen();
    for (int i = rowOffset; i < rowOffset + ROWS; ++i) {
        for (int j = colOffset; j < colOffset + COLS; ++j) {
            if (i == curRow && j == curCol) {
                std::cout << "\033[1;31m" << grid[i][j] << "\033[0m ";  // Highlight the editable character in red
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
    int curRow = ROWS / 2;
    int curCol = COLS / 2;
    bool running = true;

    drawGrid(grid, rowOffset, colOffset, curRow, curCol);

    while (running) {
        int key = getch();

        if (key == 27) {  // Escape character: start of arrow key sequence
            key = getch();
            if (key == 91) {
                key = getch();
                bool ctrl_pressed = (getch() == 53);

                if (ctrl_pressed) {  // Arrow key with Ctrl pressed
                    switch (key) {
                        case 'A':  // Up arrow
                            if (curRow > 0) {
                                --curRow;
                            }
                            break;
                        case 'B':  // Down arrow
                            if (curRow < totalRows - 1) {
                                ++curRow;
                            }
                            break;
                        case 'C':  // Right arrow
                            if (curCol < totalCols - 1) {
                                ++curCol;
                            }
                            break;
                        case 'D':  // Left arrow
                            if (curCol > 0) {
                                --curCol;
                            }
                            break;
                    }
                } else {  // Arrow key without Ctrl pressed
                    switch (key) {
                        case 'A':  // Up arrow
                            if (rowOffset > 0) {
                                --rowOffset;
                            }
                            break;
                        case 'B':  // Down arrow
                            if (rowOffset < totalRows - ROWS) {
                                ++rowOffset;
                            }
                            break;
                        case 'C':  // Right arrow
                            if (colOffset < totalCols - COLS) {
                                ++colOffset;
                            }
                            break;
                        case 'D':  // Left arrow
                            if (colOffset > 0) {
                                --colOffset;
                            }
                            break;
                    }
                }
            }
        } else {
            switch (key) {
                case 'q':
                    running = false;
                    break;
                default:
                    if (key >= 32 && key <= 126) {  // Check if it's a printable ASCII character
                        grid[curRow][curCol] = static_cast<char>(key);
                    }
                    break;
            }
        }
        drawGrid(grid, rowOffset, colOffset, curRow, curCol);
    }

    return 0;
}

