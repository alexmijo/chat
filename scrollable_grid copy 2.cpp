#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

const int DEFAULT_ROWS = 20;
const int DEFAULT_COLS = 20;

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

enum class Color { RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, DEFAULT };

void colorPrint(char c, Color color) {
    switch (color) {
    case Color::RED:
        std::cout << "\033[1;31m";
        break;
    case Color::GREEN:
        std::cout << "\033[1;32m";
        break;
    case Color::BLUE:
        std::cout << "\033[1;34m";
        break;
    case Color::YELLOW:
        std::cout << "\033[1;33m";
        break;
    case Color::CYAN:
        std::cout << "\033[1;36m";
        break;
    case Color::MAGENTA:
        std::cout << "\033[1;35m";
        break;
    case Color::WHITE:
        std::cout << "\033[1;37m";
        break;
    case Color::DEFAULT:
    default:
        std::cout << "\033[0m";
        break;
    }
    std::cout << c << "\033[0m ";
}

void drawGrid(const std::vector<std::vector<char>> &grid, int row_offset, int col_offset,
              int row_editable_offset, int col_editable_offset, int display_rows,
              int display_cols) {
    clearScreen();
    for (int i = row_offset; i < row_offset + display_rows && i < grid.size(); ++i) {
        for (int j = col_offset; j < col_offset + display_cols && j < grid[i].size(); ++j) {
            if (i == row_offset + row_editable_offset && j == col_offset + col_editable_offset) {
                colorPrint(grid[i][j], Color::RED);
            } else if (grid[i][j] == '0') {
                colorPrint(grid[i][j], Color::GREEN);
            } else if (grid[i][j] == '1') {
                colorPrint(grid[i][j], Color::BLUE);
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
        outFile << totalRows << ' ' << totalCols << ' ' << displayRows << ' ' << displayCols
                << std::endl;
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

void changeAllOccurrences(std::vector<std::vector<char>> &grid, char old_char, char new_char) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            if (grid[i][j] == old_char) {
                grid[i][j] = new_char;
            }
        }
    }
}

void increaseAllDigits(std::vector<std::vector<char>> &grid) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            if (isdigit(grid[i][j]) && grid[i][j] != '9') {
                grid[i][j]++;
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

bool isLargerDigit(char current, char target) {
    return (current >= '0' && current <= '9') && (target >= '0' && target <= '9') &&
           (target > current);
}

int main() {
    int totalRows = 20;
    int totalCols = 20;
    int displayRows = DEFAULT_ROWS;
    int displayCols = DEFAULT_COLS;

    std::vector<std::vector<char>> grid(totalRows, std::vector<char>(totalCols, '.'));

    // Load starting state from the text file
    loadState(grid, "state.txt", totalRows, totalCols, displayRows, displayCols);

    int rowOffset = 0;
    int colOffset = 0;
    int rowEditableOffset = DEFAULT_ROWS / 2;
    int colEditableOffset = DEFAULT_COLS / 2;
    bool running = true;
    int changeAllModeStep = 0;
    char changeAllFrom;

    drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset, displayRows,
             displayCols);

    while (running) {
        int key = getch();
        switch (changeAllModeStep) {
        case 1:
            changeAllFrom = key;
            changeAllModeStep = 2;
            continue;
        case 2:
            changeAllOccurrences(grid, changeAllFrom, key);
            changeAllModeStep = 0;
            drawGrid(grid, rowOffset, colOffset, rowEditableOffset, colEditableOffset, displayRows,
                     displayCols);
            continue;
        }
        const char currLocChar = grid[rowOffset + rowEditableOffset][colOffset + colEditableOffset];

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
            changeAllModeStep = 1;
            break;
        case 'n':
            increaseAllDigits(grid);
            break;
        case 'w':
            if (rowOffset + rowEditableOffset == 0 ||
                isLargerDigit(
                    currLocChar,
                    grid[rowOffset + rowEditableOffset - 1][colOffset + colEditableOffset])) {
                continue;
            }
            if (rowEditableOffset > 0) {
                --rowEditableOffset;
                break;
            } else if (rowOffset > 0) {
                --rowOffset;
                break;
            }
            continue;
        case 's':
            if (rowOffset + rowEditableOffset == totalRows - 1 ||
                isLargerDigit(
                    currLocChar,
                    grid[rowOffset + rowEditableOffset + 1][colOffset + colEditableOffset])) {
                continue;
            }
            if (rowEditableOffset < displayRows - 1) {
                ++rowEditableOffset;
                break;
            } else if (rowOffset < totalRows - displayRows) {
                ++rowOffset;
                break;
            }
            continue;
        case 'a':
            if (colOffset + colEditableOffset == 0 ||
                isLargerDigit(
                    currLocChar,
                    grid[rowOffset + rowEditableOffset][colOffset + colEditableOffset - 1])) {
                continue;
            }
            if (colEditableOffset > 0) {
                --colEditableOffset;
                break;
            } else if (colOffset > 0) {
                --colOffset;
                break;
            }
            continue;
        case 'd':
            if (colOffset + colEditableOffset == totalCols - 1 ||
                isLargerDigit(
                    currLocChar,
                    grid[rowOffset + rowEditableOffset][colOffset + colEditableOffset + 1])) {
                continue;
            }
            if (colEditableOffset < displayCols - 1) {
                ++colEditableOffset;
                break;
            } else if (colOffset < totalCols - displayCols) {
                ++colOffset;
                break;
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
