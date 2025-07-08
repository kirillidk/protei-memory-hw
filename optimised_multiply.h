#pragma once

#include "matrix.h"

inline matrix optimised_multiply(matrix const& a, matrix const& b) {
    auto [a_cols, a_rows] = a.size();
    auto [b_cols, b_rows] = b.size();

    // инициализируем результирующую матрицу нулями по дефолту
    matrix result(a_rows, b_cols, true);

    // будем вычислять элементы новой матрицы с помощью блоков 512 x 512
    // элементов, каждый блок целиком поместится в кеш, и мы сможем оптимально
    // переиспользовать уже загруженные в кеш линии подстроки матриц

    const size_t BLOCK_SIZE = 512;

    for (size_t i = 0; i < a_rows; i += BLOCK_SIZE) {
        for (size_t j = 0; j < b_cols; j += BLOCK_SIZE) {
            for (size_t k = 0; k < b_rows; k += BLOCK_SIZE) {
                size_t i_max = std::min(i + BLOCK_SIZE, a_rows);
                size_t j_max = std::min(j + BLOCK_SIZE, b_cols);
                size_t k_max = std::min(k + BLOCK_SIZE, b_rows);

                for (size_t ii = i; ii < i_max; ++ii) {
                    for (size_t kk = k; kk < k_max; ++kk) {
                        // a[ii][kk] остается в кэше в течение цикла jj

                        for (size_t jj = j; jj < j_max; ++jj) {
                            // b[kk][jj] и result[ii][jj] доступны
                            // последовательно в рамках своих кеш линий и мы
                            // минимизируем кеш миссы до новой итерации
                            result[ii][jj] += a[ii][kk] * b[kk][jj];
                        }
                    }
                }
            }
        }
    }

    return result;
}
