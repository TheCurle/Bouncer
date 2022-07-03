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
    std::vector<std::vector<double>> data;

    // Retrieve the Identity matrix. Transforms and multiplications to this are effectively null.
    static Matrix identity() {
        return identityMatrix;
    }

    // Fetch a matrix that will translate points around by the given coordinates.
    static Matrix translation(double x, double y, double z) {
        return Matrix {{
            { 1, 0, 0, x },
            { 0, 1, 0, y },
            { 0, 0, 1, z },
            { 0, 0, 0, 1 }
        }};
    }

    // Fetch a matrix that will scale points by the given factors.
    static Matrix scaling(double x, double y, double z) {
        return Matrix {{
            { x, 0, 0, 0 },
            { 0, y, 0, 0 },
            { 0, 0, z, 0 },
            { 0, 0, 0, 1 }
        }};
    }

    // Fetch a matrix that will scale points clockwise around the X axis by the given factor (interpreted in radians)
    static Matrix rotation_x(double factor) {
        return Matrix {{
            { 1, 0, 0, 0 },
            { 0, std::cos(factor), -std::sin(factor), 0 },
            { 0, std::sin(factor), std::cos(factor), 0 },
            { 0, 0, 0, 1 }
        }};
    }

    // Fetch a matrix that will scale points clockwise around the X axis by the given factor (interpreted in radians)
    static Matrix rotation_y(double factor) {
        return Matrix {{
            { std::cos(factor), 0, std::sin(factor), 0 },
            { 0, 1, 0, 0 },
            { -std::sin(factor), 0, std::cos(factor), 0 },
            { 0, 0, 0, 1 }
        }};
    }

    // Fetch a matrix that will scale points clockwise around the X axis by the given factor (interpreted in radians)
    static Matrix rotation_z(double factor) {
        return Matrix {{
            { std::cos(factor), -std::sin(factor), 0, 0 },
            { std::sin(factor), std::cos(factor), 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 }
        }};
    }

    // Fetch a matrix that will skew / shear points in the given magnitudes.
    static Matrix shearing(double xy, double xz, double yx, double yz, double zx, double zy) {
        return Matrix {{
            { 1, xy, xz, 0 },
            { yx, 1, yz, 0 },
            { zx, zy, 1, 0 },
            { 0, 0, 0, 1 }
        }};
    }

    // Calculate the determinant of this matrix.
    static double determinant(Matrix in) {
        if (in.data.size() == 2) {
            return ((in[0][0] * in[1][1]) - (in[0][1] * in[1][0]));
        }

        else {
            double total = 0;
            for (size_t i = 0; i < in.data[0].size(); i++) {
                total += in.data[0][i] * Matrix::cofactor(in, 0, i);
            }
            return total;
        }
    }

    // Get a copy of the matrix with the given column and row removed.
    static Matrix sub(const Matrix& in, size_t row, size_t col) {
        Matrix out;
        (void) col;

        out.data = in.data;

        out.data.erase(std::next(out.data.begin(), row));
        for (std::vector<double>& data : out.data) {
            data.erase(std::next(data.begin(), col));
        }

        return out;
    }

    // Calculate the determinant of the submatrix with the given column and row removed.
    static double minor(const Matrix& in, size_t row, size_t col) {
        Matrix subMatrix = Matrix::sub(in, row, col);
        return Matrix::determinant(subMatrix);
    }

    // Calculate the minor, invert positivity if row + col is odd.
    static double cofactor(const Matrix& in, size_t row, size_t col) {
        double minor = Matrix::minor(in, row, col);

        if ((row + col) % 2 == 0)
            return minor;
        else
            return -minor;
    }

    // Calculate the matrix that reverses the multiplication of the given matrix. Essentially "1/x" but for matrices.
    static Matrix inverse(Matrix in) {
        const double determinant = Matrix::determinant(in);

        Matrix cofactors(in.data.size(), in.data[0].size());
        for (size_t height = 0; height < in.data.size(); height++) {
            for (size_t width = 0; width < in.data[0].size(); width++) {
                cofactors.data[width].data()[height] = Matrix::cofactor(in, height, width) / determinant;
            }
        }

        return cofactors;
    }

    // Transpose the matrix; columns become rows, and rows become columns.
    static Matrix transpose(Matrix in) {
        Matrix out(in.data.size(), in.data[0].size());

        for (size_t i = 0; i < in.data.size(); ++i) {
            for (size_t j = 0; j < in.data[0].size(); ++j) {
                out.data[i].data()[j] = in[j][i];
            }
        }

        return out;
    }

    // No-args constructor. Set the size later.
    Matrix() = default;

    // Size constructor, use for filling in the data later.
    Matrix(int width, int height) {
        data.resize(width);
        for (int i = 0; i < width; ++i) {
            data[i].resize(height);
        }
    }

    // Value constructor. Accepts any value type that can accept a vector of vector.
    // Example: {{ {0, 2}, {1, 0} }}
    explicit Matrix(std::vector<std::vector<double>> newData) {
        this->data = std::move(newData);
    }

    // Override the data of this Matrix with given values.
    Matrix& operator=(std::vector<std::vector<double>> newData) {
        this->data = std::move(newData);
        return *this;
    }

    // Index the data without needing a .data[] access.
    std::vector<double> operator[](int index) {
        return data[index];
    }

    // A simpler way to retrieve the data at known coordinates.
    double at(int width, int height) {
        return data[width][height];
    }


};

bool operator==(const Matrix& thisMatrix, const Matrix& otherMatrix);
bool operator!=(const Matrix& thisMatrix, const Matrix& otherMatrix);
Matrix operator*(const Matrix& thisMatrix, const Matrix& otherMatrix);
Matrix operator/(Matrix thisMatrix, double dbl);
Tuple operator*(Matrix mat, const Tuple& tup);
std::ostream& operator<<(std::ostream& stream, const Matrix& matrix);

//#define MATRIX_OPERATOR_OVERLOADS
#ifdef MATRIX_OPERATOR_OVERLOADS
// Compare the equality of two matrices directly.
bool operator==(const Matrix& thisMatrix, const Matrix& otherMatrix) {
    for (size_t i = 0; i < thisMatrix.data.size(); ++i) {
        for (size_t j = 0; j < thisMatrix.data[0].size(); ++j) {
            if (! safeCompare(thisMatrix.data[i][j], otherMatrix.data[i][j]))
                return false;
        }
    }

    return true;
}

// Inverted equality check.
bool operator!=(const Matrix& thisMatrix, const Matrix& otherMatrix) {
    return !(thisMatrix== otherMatrix);
}

// Matrix multiplication. By-The-Books.
Matrix operator*(const Matrix& thisMatrix, const Matrix& otherMatrix) {
    Matrix result(thisMatrix.data.size(), thisMatrix.data[0].size());

    for (size_t row = 0; row < thisMatrix.data.size(); row++) {
        for (size_t column = 0; column < thisMatrix.data[0].size(); column++) {
            result.data.at(row).data()[column] =
                    thisMatrix.data[row][0] * otherMatrix.data[0][column] +
                    thisMatrix.data[row][1] * otherMatrix.data[1][column] +
                    thisMatrix.data[row][2] * otherMatrix.data[2][column] +
                    thisMatrix.data[row][3] * otherMatrix.data[3][column];
        }
    }

    return result;
}

Tuple operator*(Matrix mat, const Tuple& tup) {
    double x =
            tup.x * mat[0][0] +
            tup.y * mat[0][1] +
            tup.z * mat[0][2] +
            tup.w * mat[0][3];

    double y =
            tup.x * mat[1][0] +
            tup.y * mat[1][1] +
            tup.z * mat[1][2] +
            tup.w * mat[1][3];

    double z =
            tup.x * mat[2][0] +
            tup.y * mat[2][1] +
            tup.z * mat[2][2] +
            tup.w * mat[2][3];

    double w =
            tup.x * mat[3][0] +
            tup.y * mat[3][1] +
            tup.z * mat[3][2] +
            tup.w * mat[3][3];

    return {x, y, z, w };
}

// Matrix division by a double.. By-The-Books.
Matrix operator/(const Matrix& thisMatrix, const double& dbl) {
    Matrix result(thisMatrix.data.size(), thisMatrix.data[0].size());

    for (size_t row = 0; row < thisMatrix.data.size(); row++) {
        for (size_t column = 0; column < thisMatrix.data[0].size(); column++) {
            result.data.at(row).data()[column] =
                    thisMatrix.data.at(row).at(column) / dbl;
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

    for (size_t row = 0; row < matrix.data.size(); row++) {
        for (size_t column = 0; column < matrix.data[0].size(); column++) {
            formatter << matrix.data[row][column];
        }
        formatter << std::endl;
    }

    return stream;
}
#endif

inline Matrix identityMatrix = Matrix(std::vector<std::vector<double>> {{
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
}});