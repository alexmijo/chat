#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <fstream>


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

void drawGrid(const std::vector<std::vector<char>> &grid, int rowOffset, int colOffset, int rowEditableOffset, int colEditableOffset) {
    clearScreen();
    for (int i = rowOffset; i < rowOffset + ROWS; ++i) {
        for (int j = colOffset; j < colOffset + COLS; ++j) {
            if (i == rowOffset + rowEditableOffset && j == colOffset + colEditableOffset) {
                std::cout << "\033[1;31m" << grid[i][j] << "\033[0m "; // Highlight the editable character in red
            } else {
                std::cout << grid[i][j] << ' ';
            }
        }
        std::cout << std::endl;
    }
}

void saveState(const std::vector<std::vector<char>>& grid, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                outFile << grid[i][j];
            }
            outFile << std::endl;
        }
        std::cout << "State saved to " << filename << std::endl;
        outFile.close();
    }
}

void loadState(std::vector<std::vector<char>>& grid, const std::string& filename) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        std::string line;
        int i = 0;
        while (std::getline(inFile, line) && i < grid.size()) {
            for (int j = 0; j < line.length() && j < grid[i].size(); ++j) {
                grid[i][j] = line[j];
            }
            ++i;
        }
        std::cout << "State loaded from " << filename << std::endl;
        inFile.close();
    }
}

void resizeGrid(std::vector<std::vector<char>>& grid, int newRows, int newCols) {
    std::vector<std::vector<char>> newGrid(newRows, std::vector<char>(newCols, '.'));
    int minRows = std::min(newRows, int(grid.size()));
    int minCols = std::min(newCols, int(grid[0].size()));
    for (int i = 0; i < minRows; ++i) {
        for (int j = 0; j < minCols; ++j) {
            newGrid[i][j] = grid[i][j];
        }
    }
    grid = newGrid;
    std::cout << "Grid resized to " << newRows << " rows and " << newCols << " columns." << std::endl;
}

int main() {
    int totalRows = 20;
    int totalCols = 20;

    std::vector<std::vector<char>> grid(totalRows, std::vector<char>(totalCols, '.'));

    int rowOffset = 0;
    int colOffset = 0;
    int rowEditableOffset = ROWS / 2;
    int colEditableOffset = COLS / 2;
    bool running = true;

    drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset);

    while (running) {
        int key = getch();

        switch (key) {
        case 'q':
            saveState(grid, "state.txt");
            break;
        case 'Q':
            loadState(grid, "state.txt");
            break;
        case 'r':
            std::cout << "Enter new number of rows: ";
            std::cin >> totalRows;
            std::cout << "Enter new number of columns: ";
            std::cin >> totalCols;
            resizeGrid(grid, totalRows, totalCols);
            break;
        case 'w':
            if (rowEditableOffset > 0) {
                --rowEditableOffset;
            } else if (rowOffset > 0) {
                --rowOffset;
            }
            break;
        case 's':
            if (rowEditableOffset < ROWS - 1) {
                ++rowEditableOffset;
            } else if (rowOffset < totalRows - ROWS) {
                ++rowOffset;
            }
            break;
        case 'a':
            if (colEditableOffset > 0) {
                --colEditableOffset;
            } else if (colOffset > 0) {
                --colOffset;
            }
            break;
        case 'd':
            if (colEditableOffset < COLS - 1) {
                ++colEditableOffset;
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
        drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset);
    }

    return 0;
}
