#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

const int ROWS = 20;
const int COLS = 20;

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

void drawGrid(const std::vector<std::vector<char>> &grid, int rowOffset, int colOffset,
              int rowEditableOffset, int colEditableOffset, int displayRows, int displayCols) {
    clearScreen();
    for (int i = rowOffset; i < rowOffset + displayRows && i < grid.size(); ++i) {
        for (int j = colOffset; j < colOffset + displayCols && j < grid[i].size(); ++j) {
            if (i == rowOffset + rowEditableOffset && j == colOffset + colEditableOffset) {
                std::cout << "\033[1;31m" << grid[i][j]
                          << "\033[0m "; // Highlight the editable character in red
            } else if (grid[i][j] == '0') {
                std::cout << "\033[1;32m" << grid[i][j] << "\033[0m "; // Display 0s in green
            } else if (grid[i][j] == '1') {
                std::cout << "\033[1;34m" << grid[i][j] << "\033[0m "; // Display 1s in blue
            } else {
                std::cout << grid[i][j] << ' ';
            }
        }
        std::cout << std::endl;
    }
}

void resizeGrid(std::vector<std::vector<char>> &grid, int newRows, int newCols) {
    std::vector<std::vector<char>> newGrid(newRows, std::vector<char>(newCols, '.'));
    int minRows = std::min(newRows, int(grid.size()));
    int minCols = std::min(newCols, int(grid[0].size()));
    for (int i = 0; i < minRows; ++i) {
        for (int j = 0; j < minCols; ++j) {
            newGrid[i][j] = grid[i][j];
        }
    }
    grid = newGrid;
    std::cout << "Grid resized to " << newRows << " rows and " << newCols << " columns."
              << std::endl;
}

void saveState(const std::vector<std::vector<char>> &grid, const std::string &filename,
               int totalRows, int totalCols, int displayRows, int displayCols) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << totalRows << ' ' << totalCols << ' ' << displayRows << ' ' << displayCols << std::endl;
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

void loadState(std::vector<std::vector<char>> &grid, const std::string &filename, int &totalRows,
               int &totalCols, int &displayRows, int &displayCols) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        inFile >> totalRows >> totalCols >> displayRows >> displayCols;
        inFile.ignore(); // Ignore newline after dimensions
        resizeGrid(grid, totalRows, totalCols);
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

void changeDotsToAs(std::vector<std::vector<char>> &grid) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            if (grid[i][j] == '.') {
                grid[i][j] = 'A';
            }
        }
    }
}

int countLiveNeighbors(const std::vector<std::vector<char>> &grid, int row, int col) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0)
                continue;
            int newRow = row + i;
            int newCol = col + j;
            if (newRow >= 0 && newRow < grid.size() && newCol >= 0 && newCol < grid[0].size()) {
                if (grid[newRow][newCol] == '0') {
                    count++;
                }
            }
        }
    }
    return count;
}


void applyGameOfLifeRules(std::vector<std::vector<char>> &grid) {
    std::vector<std::vector<char>> newGrid = grid;
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            int liveNeighbors = countLiveNeighbors(grid, i, j);
            if (grid[i][j] == '0') {
                if (liveNeighbors < 2 || liveNeighbors > 3) {
                    newGrid[i][j] = '.';
                }
            } else {
                if (liveNeighbors == 3 && grid[i][j] != '1') {
                    newGrid[i][j] = '0';
                }
            }
        }
    }
    grid = newGrid;
}

int main() {
    int totalRows = 20;
    int totalCols = 20;
    int displayRows = ROWS;
    int displayCols = COLS;

    std::vector<std::vector<char>> grid(totalRows, std::vector<char>(totalCols, '.'));

    // Load starting state from the text file
    loadState(grid, "state.txt", totalRows, totalCols, displayRows, displayCols);

    int rowOffset = 0;
    int colOffset = 0;
    int rowEditableOffset = ROWS / 2;
    int colEditableOffset = COLS / 2;
    bool running = true;

    drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset, displayRows,
             displayCols);

    while (running) {
        int key = getch();

        switch (key) {
        case 'q':
            saveState(grid, "state.txt", totalRows, totalCols, displayRows, displayCols);
            break;
        case 'Q':
            loadState(grid, "state.txt", totalRows, totalCols, displayRows, displayCols);
            break;
        case 'r':
            std::cout << "Enter new number of rows: ";
            std::cin >> totalRows;
            std::cout << "Enter new number of columns: ";
            std::cin >> totalCols;
            resizeGrid(grid, totalRows, totalCols);
            break;
        case 'c':
            applyGameOfLifeRules(grid);
            break;
        case 'v':
            std::cout << "Enter new number of display rows: ";
            std::cin >> displayRows;
            std::cout << "Enter new number of display columns: ";
            std::cin >> displayCols;
            std::cin.ignore(); // Ignore any remaining newline
            break;
        case 'b':
            changeDotsToAs(grid);
            break;
        case 'w':
            if (rowEditableOffset > 0) {
                --rowEditableOffset;
            } else if (rowOffset > 0) {
                --rowOffset;
            }
            break;
        case 's':
            if (rowEditableOffset < displayRows - 1) {
                ++rowEditableOffset;
            } else if (rowOffset < totalRows - displayRows) {
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
            if (colEditableOffset < displayCols - 1) {
                ++colEditableOffset;
            } else if (colOffset < totalCols - displayCols) {
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
        drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset, displayRows,
                 displayCols);
    }

    return 0;
}
