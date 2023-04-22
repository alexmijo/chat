#include <iostream>
#include <cmath>

// Calculate the percentage of n-digit numbers that have m digits in a row using the formula
double percentage_formula(int n, int m) {
    double total_numbers = std::pow(10, n);
    double numbers_with_m_in_a_row = std::pow(10, n-m+1) * (n-m+2);
    return 100 * numbers_with_m_in_a_row / total_numbers;
}

// Calculate the percentage of n-digit numbers that have m digits in a row using brute force
double percentage_brute_force(int n, int m) {
    int count = 0;
    for (int i = 0; i <= std::pow(10, n)-1; i++) {
        int num = i;
        int consecutive_count = 0;
        int prev_digit = -1;
        int first_m_minus_one_digits_consecutive_count = 0;
        int first_m_minus_one_digits_prev_digit = -1;
        while (num > 0) {
            int digit = num % 10;
            if (digit == prev_digit) {
                consecutive_count++;
                if (consecutive_count == m) {
                    count++;
                    break;
                }
            } else {
                consecutive_count = 1;
            }
            if (first_m_minus_one_digits_prev_digit != -1) {
                if (digit == first_m_minus_one_digits_prev_digit) {
                    first_m_minus_one_digits_consecutive_count++;
                    if (first_m_minus_one_digits_consecutive_count == m-1 && consecutive_count == 1) {
                        count++;
                        break;
                    }
                } else {
                    first_m_minus_one_digits_consecutive_count = 0;
                }
            }
            prev_digit = digit;
            num /= 10;
            if (num == 0 && first_m_minus_one_digits_consecutive_count == 0 && m > 2) {
                // Check if the first m-1 digits form a run
                int first_m_minus_one_digits = i / 10;
                for (int j = 0; j < m-2; j++) {
                    int d1 = first_m_minus_one_digits % 10;
                    int d2 = (first_m_minus_one_digits / 10) % 10;
                    if (d1 != d2) {
                        break;
                    }
                    first_m_minus_one_digits /= 10;
                }
                if (first_m_minus_one_digits_consecutive_count == 0 && consecutive_count == 1) {
                    count++;
                }
            }
            first_m_minus_one_digits_prev_digit = digit;
        }
    }
    return 100 * count / std::pow(10, n);
}

int main() {
    int n = 4;
    int m = 2;
    double percentage_formula_result = percentage_formula(n, m);
    double percentage_brute_force_result = percentage_brute_force(n, m);
    std::cout << "n = " << n << ", m = " << m << std::endl;
    std::cout << "Percentage using formula: " << percentage_formula_result << "%" << std::endl;
    std::cout << "Percentage using brute force: " << percentage_brute_force_result << "%" << std::endl;
    return 0;
}
