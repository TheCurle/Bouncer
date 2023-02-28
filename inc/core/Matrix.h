/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/


#define _USE_MATH_DEFINES
#include <cmath>

#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>

#include <core/Tuple.hpp>

#pragma once

bool safeCompare(double a, double b);

/**
 * Represents an arbitrary-dimensional matrix of arbitrary size.
 *
 * Size is set during construction or assignment.
 * Copy assignment and move assignment are both valid.
 *
 * Uses a single backing array to store its' data.
*/

struct Matrix {
    // The dimension of the matrix. Only square matrices can be represented this way.
    size_t size{};
    std::unique_ptr<double[]> data;

    // Retrieve the Identity matrix. Transforms and multiplications to this are effectively null.
    static Matrix identity() {
        return Matrix {{
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 }
        }};
    }

    // Fetch a matrix that will translate points around by the given coordinates.
    static Matrix translation(double x, double y, double z) {
        return Matrix {{
            {1, 0, 0, x},
            {0, 1, 0, y},
            {0, 0, 1, z},
            {0, 0, 0, 1}
        }};
    }

    // Fetch a matrix that will scale points by the given factors.
    static Matrix scaling(double x, double y, double z) {
        return Matrix {{
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
            {0, 0, 0, 1}
        }};
    }

    // Fetch a matrix that will scale points clockwise around the X axis by the given factor (interpreted in radians)
    static Matrix rotation_x(double factor) {
        return Matrix {{
            {1, 0, 0, 0},
            {0, std::cos(factor), -std::sin(factor), 0},
            {0, std::sin(factor), std::cos(factor), 0},
            {0, 0, 0, 1}
        }};
    }

    // Fetch a matrix that will scale points clockwise around the X axis by the given factor (interpreted in radians)
    static Matrix rotation_y(double factor) {
        return Matrix {{
            {std::cos(factor), 0, std::sin(factor), 0},
            {0, 1, 0, 0},
            {-std::sin(factor), 0, std::cos(factor), 0},
            {0, 0, 0, 1}
        }};
    }

    // Fetch a matrix that will scale points clockwise around the X axis by the given factor (interpreted in radians)
    static Matrix rotation_z(double factor) {
        return Matrix {{
            {std::cos(factor), -std::sin(factor), 0, 0},
            {std::sin(factor), std::cos(factor), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        }};
    }

    // Fetch a matrix that will skew / shear points in the given magnitudes.
    static Matrix shearing(double xy, double xz, double yx, double yz, double zx, double zy) {
        return Matrix {{
            {1, xy, xz, 0},
            {yx, 1, yz, 0},
            {zx, zy, 1, 0},
            {0, 0, 0, 1}
        }};
    }

    static Matrix rotation(Quat& q) {
        double x = q.x, y = q.y, z = q.z, s = q.w;
        return Matrix { {
            { 1 - 2*y*y - 2*z*z, 2*x*y - 2*s*z, 2*x*z + 2*s*y, 0 },
            { 2*x*y + 2*s*z, 1 - 2*x*x - 2*z*z, 2*y*z - 2*s*x, 0 },
            { 2*x*z - 2*s*y, 2*y*z + 2*s*x, 1 - 2*x*x - 2*y*y, 0 },
            { 0, 0, 0, 1 }
        }};
    }

    // Calculate the determinant of this matrix, via the Laplace Expansion.
    // Used to check whether the matrix is invertible.
    static double determinant(const Matrix& in) {
        // We can only directly check a 2x2 matrix.
        if (in.size == 2) {
            return ((in.at(0, 0) * in.at(1, 1)) - (in.at(0, 1) * in.at(1, 0)));
        } else {
            // The Laplace Expansion provides a recursive algorithm for calculating the determinants of larger matrices.
            double total = 0;
            for (size_t i = 0; i < in.size; i++) {
                total += in.at(0, i) * Matrix::cofactor(in, 0, i);
            }
            return total;
        }
    }

    // Get a copy of the matrix with the given column and row removed.
    static Matrix sub(const Matrix &in, size_t row, size_t col) {
        Matrix out(in.size - 1, in.size - 1);

        size_t outCounter = 0;
        for (size_t size = 0; size < in.size * in.size; size++) {
            // If position lands on the passed column, skip
            if ((size_t) size % in.size == col) continue;
            // If position lands on the passed row, skip
            if ((size_t) size >= (in.size * row) && (size_t) size < (in.size * (row + 1))) continue;
            // else add to the matrix
            out.data[outCounter++] = in.data[size];
        }

        return out;
    }

    // Calculate the determinant of the submatrix with the given column and row removed.
    // Part of the implementation of the Laplace Expansion for the determinant of arbitrary size matrices.
    static double minor(const Matrix &in, size_t row, size_t col) {
        Matrix subMatrix = Matrix::sub(in, row, col);
        return Matrix::determinant(subMatrix);
    }

    // Calculate the minor, invert positivity if row + col is odd.
    // Part of the implementation of the Laplace Expansion for the determinant of arbitrary size matrices.
    static double cofactor(const Matrix &in, size_t row, size_t col) {
        double minor = Matrix::minor(in, row, col);

        if ((row + col) % 2 == 0)
            return minor;
        else
            return -minor;
    }

    // A simple data swapping routine that operates on rows of a linear array.
    // Operates in-place.
    static void swapRows(const Matrix& in, int row1, int row2) {
        for (size_t i = 0; i < in.size; ++i) {
            double temp = in.data[row1 * in.size + i];
            in.data[row1 * in.size + i] = in.data[row2 * in.size + i];
            in.data[row2 * in.size + i] = temp;
        }
    }

    // A fast, highly parallel function to calculate the inverse of a matrix.
    // See inverse below for a more visual explanation of what is going on here.
    static Matrix fastInverse(const Matrix& input_matrix ) {
        int size = input_matrix.size;

        Matrix ident = Matrix::identity();
        for (int i = 0; i < size; i++) {
            if (input_matrix.at(i, i) == 0) {
                // swap nearest subsequent row s.t input_matrix[i][i] != 0 after swapping
                for (int j = i + 1; j < size; j++) {
                    if (input_matrix.at(j, i) != 0.0) {
                        swapRows(input_matrix, i, j);
                        break;
                    }
                    if (j == size - 1) {
                        return ident;
                    }
                }
            }
            double scale = input_matrix.at(i, i);
#pragma omp parallel for default(none) shared(size, input_matrix, scale, ident, i)
            for (int col = 0; col < size; col++) {
                input_matrix.at(i, col) = input_matrix.at(i, col) / scale;
                ident.at(i, col) = ident.at(i, col) / scale;
            }
            if (i < size - 1) {
#pragma omp parallel for default(none) shared(size, input_matrix, scale, ident, i)
                for (int row = i + 1; row < size; row++) {
                    double factor = input_matrix.at(row, i);
                    for (int col = 0; col < size; col++) {
                        input_matrix.at(row, col) -= factor * input_matrix.at(i, col);
                        ident.at(row, col) -= factor * ident.at(i, col);
                    }
                }
            }
        }

        for (int zeroing_col = size - 1; zeroing_col >= 1; zeroing_col--) {
#pragma omp parallel for default(none) shared(zeroing_col, input_matrix, size, ident)
            for (int row = zeroing_col - 1; row >= 0; row--) {
                double factor = input_matrix.at(row, zeroing_col);
                for (int col = 0; col < size; col++) {
                    input_matrix.at(row, col) -= factor * input_matrix.at(zeroing_col, col);
                    ident.at(row, col) -= factor * ident.at(zeroing_col, col);
                }
            }
        }
        return ident;
    }


    // Calculate the matrix that reverses the multiplication of the given matrix. Essentially "1/x" but for matrices.
    static Matrix inverse(const Matrix& in) {
        const double determinant = Matrix::determinant(in);
        Matrix cofactors(in.size, in.size);

        // The algorithm boils down to:
        // 1. Create a matrix filled with the cofactors of each position of the current matrix.
        // 2. Divide each cofactor by the determinant of the matrix.
        // 3. Transpose the matrix (replace each row with each column).

        // This loop will do it all in one go; notice that the width and height is different in each access.
        // This will cause problems if the determinant is 0 for whatever reason, though.
        for (size_t height = 0; height < in.size; height++) {
            for (size_t width = 0; width < in.size; width++) {
                cofactors.data[width * in.size + height] = Matrix::cofactor(in, height, width) / determinant;
            }
        }

        return cofactors;
    }

    // Transpose the matrix; columns become rows, and rows become columns.
    static Matrix transpose(const Matrix& in) {
        Matrix out(in.size, in.size);

        for (size_t i = 0; i < in.size; ++i) {
            for (size_t j = 0; j < in.size; ++j) {
                out.data[j * in.size + i] = in.data[i * in.size + j];
            }
        }

        return out;
    }

    Matrix() = default;
    ~Matrix() = default;

    // Size constructor, use for filling in the data later.
    Matrix(int width, int height) {
        (void) height;
        size = width;
        data = std::make_unique<double[]>(width * height);
    }

    // Value constructor. Accepts any value core that can accept a vector of vector.
    // Example: {{ {0, 2}, {1, 0} }}
    explicit Matrix(std::vector<std::vector<double>> newData) {
        size = newData.size();
        data = std::make_unique<double[]>(size * size);
        for (size_t y = 0; y < size; y++) {
            for (size_t x = 0; x < size; x++) {
                data[y * size + x] = newData[y][x];
            }
        }
    }

    Matrix(const Matrix& matrix) : size(matrix.size), data(std::make_unique<double[]>(matrix.size * matrix.size)) {
        std::copy(matrix.data.get(), matrix.data.get() + matrix.size * matrix.size, data.get());
    }

    Matrix& operator=(const Matrix& matrix) {
        size = matrix.size;
        data = std::make_unique<double[]>(matrix.size * matrix.size);

        std::copy(matrix.data.get(), matrix.data.get() + matrix.size * matrix.size, data.get());
        return *this;
    }

    // Override the data of this Matrix with given values.
    Matrix& operator=(std::vector<std::vector<double>> newData) {
        size = newData.size();
        for (size_t y = 0; y < size; y++) {
            for (size_t x = 0; x < size; x++) {
                data[y * size + x] = newData[y][x];
            }
        }
        return *this;
    }

    // A simpler way to retrieve the data at known coordinates.
    [[nodiscard]] inline double& at(int width, int height) const {
        return data[width * size + height];
    }

    // Compare the equality of two matrices directly.
    bool operator==(const Matrix& otherMatrix) const {
        for (size_t i = 0; i < size * size; ++i) {
            if (!safeCompare(data[i], otherMatrix.data[i]))
                return false;
        }

        return true;
    }

    // Inverted equality check.
    bool operator!=(const Matrix& otherMatrix) {
        return !(*this == otherMatrix);
    }

    // Matrix multiplication. By-The-Books.
    Matrix operator*(const Matrix& otherMatrix) {
        Matrix result(size, size);

        for (size_t row = 0; row < size; row++) {
            for (size_t column = 0; column < size; column++) {
                for(size_t idx = 0; idx < size; idx++) {
                    result.at(row, idx) += at(row, column) * otherMatrix.at(column, idx);
                }
            }
        }

        return result;
    }

    Tuple operator*(const Tuple& tup) const {
        double x =
                tup.x * at(0, 0) +
                tup.y * at(0, 1) +
                tup.z * at(0, 2) +
                tup.w * at(0, 3);

        double y =
                tup.x * at(1, 0) +
                tup.y * at(1, 1) +
                tup.z * at(1, 2) +
                tup.w * at(1, 3);

        double z =
                tup.x * at(2, 0) +
                tup.y * at(2, 1) +
                tup.z * at(2, 2) +
                tup.w * at(2, 3);

        double w =
                tup.x * at(3, 0) +
                tup.y * at(3, 1) +
                tup.z * at(3, 2) +
                tup.w * at(3, 3);

        return {x, y, z, w };
    }

    // Matrix division by a double.. By-The-Books.
    Matrix operator/( const double& dbl) const {
        Matrix result(size, size);

        for (size_t row = 0; row < size; row++) {
            for (size_t column = 0; column < size; column++) {
                result.data[row * size + column] =
                        at(row, column) / dbl;
            }
        }

        return result;
    }

    Point toTranslation() const {
        return { data[3], data[7], data[11] };
    }

    Matrix& setTranslation(const Point& p) {
        data[3] = p.x;
        data[7] = p.y;
        data[11] = p.z;
        return *this;
    }
};

std::ostream& operator<<(std::ostream& stream, const Matrix& matrix);

// A simple wrapper class that allows you to print tables (such as matrices) to stdout
struct TableFormat {
    int width;
    char fill;
    TableFormat(): width(14), fill(' ') {}
    template<typename T>
    TableFormat& operator<<(const T& data) {
        std::cout << data << std::setw(width) << std::setfill(fill);
        return *this;
    }
    TableFormat& operator<<(std::ostream&(*out)(std::ostream&)) {
        std::cout << out;
        return *this;
    }
};
