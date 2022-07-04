/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/


#define _USE_MATH_DEFINES
#include <math.h>

#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>

#include <type/Tuple.hpp>

#pragma once

struct Matrix;
Matrix operator/(Matrix thisMatrix, double dbl);
extern Matrix identityMatrix;

bool safeCompare(double a, double b);

/**
 * Represents a 2-dimensional matrix of arbitrary size.
 *
 * Set the size during constructor OR with the method call, not both.
 * Setting the size twice is invalid and will throw.
 *
 * 0-width in either dimension is also invalid and will throw.
*/

struct Matrix {
    size_t size;
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

    // Calculate the determinant of this matrix.
    static double determinant(Matrix in) {
        if (in.size == 2) {
            return ((in.at(0, 0) * in.at(1, 1)) - (in.at(0, 1) * in.at(1, 0)));
        } else {
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

        //double at(int width, int height) {
        //    return data[width * size + height];
        //}
        size_t outCounter = 0;
        for (size_t size = 0; size < in.size * in.size; size++) {
            // If position lands on a column
            if ((size_t) size % in.size == col) continue;
            // If position lands on a row
            if ((size_t) size >= (in.size * row) && (size_t) size <= (in.size * (row + 1))) continue;
            // else add to the matrix
            out.data[outCounter++] = in.data[size];
        }

        return out;
    }

    // Calculate the determinant of the submatrix with the given column and row removed.
    static double minor(const Matrix &in, size_t row, size_t col) {
        Matrix subMatrix = Matrix::sub(in, row, col);
        return Matrix::determinant(subMatrix);
    }

    // Calculate the minor, invert positivity if row + col is odd.
    static double cofactor(const Matrix &in, size_t row, size_t col) {
        double minor = Matrix::minor(in, row, col);

        if ((row + col) % 2 == 0)
            return minor;
        else
            return -minor;
    }

    // Calculate the matrix that reverses the multiplication of the given matrix. Essentially "1/x" but for matrices.
    static Matrix inverse(Matrix in) {
        const double determinant = Matrix::determinant(in);

        Matrix cofactors(in.size, in.size);
        for (size_t height = 0; height < in.size; height++) {
            for (size_t width = 0; width < in.size; width++) {
                cofactors.data[width * in.size + height] = Matrix::cofactor(in, height, width) / determinant;
            }
        }

        return cofactors;
    }

    // Transpose the matrix; columns become rows, and rows become columns.
    static Matrix transpose(Matrix in) {
        Matrix out(in.size, in.size);

        for (size_t i = 0; i < in.size; ++i) {
            for (size_t j = 0; j < in.size; ++j) {
                out.data[j * in.size + i] = in.data[i * in.size + j];
            }
        }

        return out;
    }

    // No-args constructor. Set the size later.
    Matrix() = default;
    ~Matrix() = default;

    // Size constructor, use for filling in the data later.
    Matrix(int width, int height) {
        (void) height;
        size = width;
        data = std::make_unique<double[]>(width * height);
    }

    // Value constructor. Accepts any value type that can accept a vector of vector.
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
        std::copy(matrix.data.get(), matrix.data.get() + matrix.size, data.get());
    }

    Matrix& operator=(const Matrix& matrix) {
        size = matrix.size;
        data = std::make_unique<double[]>(matrix.size * matrix.size);

        std::copy(matrix.data.get(), matrix.data.get() + matrix.size, data.get());
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
    inline double& at(int width, int height) const {
        return data[width * size + height];
    }
};

bool operator==(const Matrix& thisMatrix, const Matrix& otherMatrix);
bool operator!=(const Matrix& thisMatrix, const Matrix& otherMatrix);
Matrix operator*(const Matrix& thisMatrix, const Matrix& otherMatrix);
Matrix operator/(Matrix& thisMatrix, double dbl);
Tuple operator*(const Matrix& mat, const Tuple& tup);
std::ostream& operator<<(std::ostream& stream, const Matrix& matrix);

//#define MATRIX_OPERATOR_OVERLOADS
#ifdef MATRIX_OPERATOR_OVERLOADS
// Compare the equality of two matrices directly.
bool operator==(const Matrix& thisMatrix, const Matrix& otherMatrix) {
    for (size_t i = 0; i < thisMatrix.size * thisMatrix.size; ++i) {
        if (!safeCompare(thisMatrix.data[i], otherMatrix.data[i]))
            return false;
    }

    return true;
}

// Inverted equality check.
bool operator!=(const Matrix& thisMatrix, const Matrix& otherMatrix) {
    return !(thisMatrix== otherMatrix);
}

// Matrix multiplication. By-The-Books.
Matrix operator*(const Matrix& thisMatrix, const Matrix& otherMatrix) {
    Matrix result(thisMatrix.size, thisMatrix.size);

    for (size_t row = 0; row < thisMatrix.size; row++) {
        for (size_t column = 0; column < thisMatrix.size; column++) {
            result.data[row * thisMatrix.size + column] =
                    thisMatrix.at(row, 0) * otherMatrix.at(0, column) +
                    thisMatrix.at(row, 1) * otherMatrix.at(1, column) +
                    thisMatrix.at(row, 2) * otherMatrix.at(2, column) +
                    thisMatrix.at(row, 3) * otherMatrix.at(3, column);
        }
    }

    return result;
}

Tuple operator*(const Matrix& mat, const Tuple& tup) {
    double x =
            tup.x * mat.at(0, 0) +
            tup.y * mat.at(0, 1) +
            tup.z * mat.at(0, 2) +
            tup.w * mat.at(0, 3);

    double y =
            tup.x * mat.at(1, 0) +
            tup.y * mat.at(1, 1) +
            tup.z * mat.at(1, 2) +
            tup.w * mat.at(1, 3);

    double z =
            tup.x * mat.at(2, 0) +
            tup.y * mat.at(2, 1) +
            tup.z * mat.at(2, 2) +
            tup.w * mat.at(2, 3);

    double w =
            tup.x * mat.at(3, 0) +
            tup.y * mat.at(3, 1) +
            tup.z * mat.at(3, 2) +
            tup.w * mat.at(3, 3);

    return {x, y, z, w };
}

// Matrix division by a double.. By-The-Books.
Matrix operator/(const Matrix& thisMatrix, const double& dbl) {
    Matrix result(thisMatrix.size, thisMatrix.size);

    for (size_t row = 0; row < thisMatrix.size; row++) {
        for (size_t column = 0; column < thisMatrix.size; column++) {
            result.data[row * thisMatrix.size + column] =
                    thisMatrix.at(row, column) / dbl;
        }
    }

    return result;
}

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

// Matrix stream operator. Allows you to print a Matrix directly into stdout.
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
#endif