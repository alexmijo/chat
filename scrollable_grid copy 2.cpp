#include <array>
#include <cmath>
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
std::vector<std::vector<int>> turbo_colormap = {
    {48, 18, 59},    {50, 21, 67},    {51, 24, 74},   {52, 27, 81},   {53, 30, 88},
    {54, 33, 95},    {55, 36, 102},   {56, 39, 109},  {57, 42, 115},  {58, 45, 121},
    {59, 47, 128},   {60, 50, 134},   {61, 53, 139},  {62, 56, 145},  {63, 59, 151},
    {63, 62, 156},   {64, 64, 162},   {65, 67, 167},  {65, 70, 172},  {66, 73, 177},
    {66, 75, 181},   {67, 78, 186},   {68, 81, 191},  {68, 84, 195},  {68, 86, 199},
    {69, 89, 203},   {69, 92, 207},   {69, 94, 211},  {70, 97, 214},  {70, 100, 218},
    {70, 102, 221},  {70, 105, 224},  {70, 107, 227}, {71, 110, 230}, {71, 113, 233},
    {71, 115, 235},  {71, 118, 238},  {71, 120, 240}, {71, 123, 242}, {70, 125, 244},
    {70, 128, 246},  {70, 130, 248},  {70, 133, 250}, {70, 135, 251}, {69, 138, 252},
    {69, 140, 253},  {68, 143, 254},  {67, 145, 254}, {66, 148, 255}, {65, 150, 255},
    {64, 153, 255},  {62, 155, 254},  {61, 158, 254}, {59, 160, 253}, {58, 163, 252},
    {56, 165, 251},  {55, 168, 250},  {53, 171, 248}, {51, 173, 247}, {49, 175, 245},
    {47, 178, 244},  {46, 180, 242},  {44, 183, 240}, {42, 185, 238}, {40, 188, 235},
    {39, 190, 233},  {37, 192, 231},  {35, 195, 228}, {34, 197, 226}, {32, 199, 223},
    {31, 201, 221},  {30, 203, 218},  {28, 205, 216}, {27, 208, 213}, {26, 210, 210},
    {26, 212, 208},  {25, 213, 205},  {24, 215, 202}, {24, 217, 200}, {24, 219, 197},
    {24, 221, 194},  {24, 222, 192},  {24, 224, 189}, {25, 226, 187}, {25, 227, 185},
    {26, 228, 182},  {28, 230, 180},  {29, 231, 178}, {31, 233, 175}, {32, 234, 172},
    {34, 235, 170},  {37, 236, 167},  {39, 238, 164}, {42, 239, 161}, {44, 240, 158},
    {47, 241, 155},  {50, 242, 152},  {53, 243, 148}, {56, 244, 145}, {60, 245, 142},
    {63, 246, 138},  {67, 247, 135},  {70, 248, 132}, {74, 248, 128}, {78, 249, 125},
    {82, 250, 122},  {85, 250, 118},  {89, 251, 115}, {93, 252, 111}, {97, 252, 108},
    {101, 253, 105}, {105, 253, 102}, {109, 254, 98}, {113, 254, 95}, {117, 254, 92},
    {121, 254, 89},  {125, 255, 86},  {128, 255, 83}, {132, 255, 81}, {136, 255, 78},
    {139, 255, 75},  {143, 255, 73},  {146, 255, 71}, {150, 254, 68}, {153, 254, 66},
    {156, 254, 64},  {159, 253, 63},  {161, 253, 61}, {164, 252, 60}, {167, 252, 58},
    {169, 251, 57},  {172, 251, 56},  {175, 250, 55}, {177, 249, 54}, {180, 248, 54},
    {183, 247, 53},  {185, 246, 53},  {188, 245, 52}, {190, 244, 52}, {193, 243, 52},
    {195, 241, 52},  {198, 240, 52},  {200, 239, 52}, {203, 237, 52}, {205, 236, 52},
    {208, 234, 52},  {210, 233, 53},  {212, 231, 53}, {215, 229, 53}, {217, 228, 54},
    {219, 226, 54},  {221, 224, 55},  {223, 223, 55}, {225, 221, 55}, {227, 219, 56},
    {229, 217, 56},  {231, 215, 57},  {233, 213, 57}, {235, 211, 57}, {236, 209, 58},
    {238, 207, 58},  {239, 205, 58},  {241, 203, 58}, {242, 201, 58}, {244, 199, 58},
    {245, 197, 58},  {246, 195, 58},  {247, 193, 58}, {248, 190, 57}, {249, 188, 57},
    {250, 186, 57},  {251, 184, 56},  {251, 182, 55}, {252, 179, 54}, {252, 177, 54},
    {253, 174, 53},  {253, 172, 52},  {254, 169, 51}, {254, 167, 50}, {254, 164, 49},
    {254, 161, 48},  {254, 158, 47},  {254, 155, 45}, {254, 153, 44}, {254, 150, 43},
    {254, 147, 42},  {254, 144, 41},  {253, 141, 39}, {253, 138, 38}, {252, 135, 37},
    {252, 132, 35},  {251, 129, 34},  {251, 126, 33}, {250, 123, 31}, {249, 120, 30},
    {249, 117, 29},  {248, 114, 28},  {247, 111, 26}, {246, 108, 25}, {245, 105, 24},
    {244, 102, 23},  {243, 99, 21},   {242, 96, 20},  {241, 93, 19},  {240, 91, 18},
    {239, 88, 17},   {237, 85, 16},   {236, 83, 15},  {235, 80, 14},  {234, 78, 13},
    {232, 75, 12},   {231, 73, 12},   {229, 71, 11},  {228, 69, 10},  {226, 67, 10},
    {225, 65, 9},    {223, 63, 8},    {221, 61, 8},   {220, 59, 7},   {218, 57, 7},
    {216, 55, 6},    {214, 53, 6},    {212, 51, 5},   {210, 49, 5},   {208, 47, 5},
    {206, 45, 4},    {204, 43, 4},    {202, 42, 4},   {200, 40, 3},   {197, 38, 3},
    {195, 37, 3},    {193, 35, 2},    {190, 33, 2},   {188, 32, 2},   {185, 30, 2},
    {183, 29, 2},    {180, 27, 1},    {178, 26, 1},   {175, 24, 1},   {172, 23, 1},
    {169, 22, 1},    {167, 20, 1},    {164, 19, 1},   {161, 18, 1},   {158, 16, 1},
    {155, 15, 1},    {152, 14, 1},    {149, 13, 1},   {146, 11, 1},   {142, 10, 1},
    {139, 9, 2},     {136, 8, 2},     {133, 7, 2},    {129, 6, 2},    {126, 5, 2},
    {122, 4, 3}};

std::array<int, 3> interpolate(double t, const std::vector<std::vector<int>> &colormap) {
    size_t idx1 = t * (colormap.size() - 1);
    size_t idx2 = std::min(idx1 + 1, colormap.size() - 1);
    double frac = t * (colormap.size() - 1) - idx1;

    std::array<int, 3> result;
    for (size_t i = 0; i < 3; ++i) {
        result[i] = std::round((1.0 - frac) * colormap[idx1][i] + frac * colormap[idx2][i]);
    }
    return result;
}

void colorPrint(char c, Color color = Color::DEFAULT, bool mandatoryColor = false) {
    if (c >= '0' && c <= '9' && !mandatoryColor) {
        int val = c - '0';    // get the numerical value of the digit
        double t = val / 9.0; // normalize the value to the range [0, 1]
        auto rgb = interpolate(t, turbo_colormap);

        std::cout << "\033[38;2;" << rgb[0] << ";" << rgb[1] << ";" << rgb[2] << "m" << c
                  << "\033[0m ";
        return;
    }

    // handle other colors
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
                colorPrint(grid[i][j], Color::RED, true);
            } else {
                colorPrint(grid[i][j]);
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

void cyclicIncreaseAllDigits(std::vector<std::vector<char>> &grid) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            if (isdigit(grid[i][j])) {
                if (grid[i][j] == '9') {
                    grid[i][j] = '0';
                } else {
                    grid[i][j]++;
                }
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

void applyWeirdGameOfLifeRules(std::vector<std::vector<char>> &grid) {
    std::vector<std::vector<char>> newGrid = grid;
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            int liveNeighbors = countLiveNeighbors(grid, i, j);
            if (grid[i][j] == '0') {
                if (liveNeighbors < 2 || liveNeighbors > 3) {
                    newGrid[i][j] = '.';
                }
            } else if (liveNeighbors == 3) {
                if (grid[i][j] >= '1' && grid[i][j] <= '9') {
                    int c = grid[i][j] - '0';
                    int randomValue = rand() % (c + 1);
                    if (randomValue == 0) {
                        newGrid[i][j] = '0';
                    } else {
                        newGrid[i][j] = grid[i][j];
                    }
                } else {
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

void dropThrough(std::vector<std::vector<char>> &grid, char droppingChar, char droppedThroughChar) {
    for (int c = 0; c < grid[0].size(); c++) {
        int topRow = -1;
        for (int r = 0; r < grid.size(); r++) {
            if (grid[r][c] == droppingChar) {
                if (topRow == -1) {
                    topRow = r;
                }
            } else if (grid[r][c] == droppedThroughChar) {
                if (topRow != -1) {
                    grid[topRow][c] = droppedThroughChar;
                    grid[r][c] = droppingChar;
                    topRow = -1;
                }
            } else {
                topRow = -1;
            }
        }
    }
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
    int changeDropModeStep = 0;
    char droppingChar;
    char droppedThroughChar;

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
        switch (changeDropModeStep) {
        case 2:
            droppingChar = key;
            changeDropModeStep = 3;
            continue;
        case 3:
            droppedThroughChar = key;
            changeDropModeStep = 1;
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
        case 'x':
            if (changeDropModeStep == 0) {
                continue;
            }
            dropThrough(grid, droppingChar, droppedThroughChar);
            break;
        case 'z':
            changeDropModeStep = 2;
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
        case 'm':
            applyWeirdGameOfLifeRules(grid);
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
            cyclicIncreaseAllDigits(grid);
            break;
        case 'N':
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
