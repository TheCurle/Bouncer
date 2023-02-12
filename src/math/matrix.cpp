#include <core/Matrix.h>

std::ostream& operator<<(std::ostream& stream, const Matrix& matrix) {
    TableFormat formatter;

    for (size_t row = 0; row < matrix.size; row++) {
        for (size_t column = 0; column < matrix.size; column++) {
            formatter << matrix.at(row, column);
        }
        formatter << std::endl;
    }

    return stream;
}