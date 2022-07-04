/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#define MATRIX_OPERATOR_OVERLOADS
#include <type/Matrix.h>
#include <iostream>

// The tolerance for comparisons.
static constexpr double epsilon = 0.0001;
// Compare two doubles with tolerance.
bool safeCompare(double a, double b) {
    return abs(a - b) < epsilon;
}

SCENARIO("Constructing and inspecting a 4x4 matrix") {
    GIVEN(R"--(The following 4x4 matrix M:
"""
|  1  |  2  |  3  |  4  |
| 5.5 | 6.5 | 7.5 | 8.5 |
|  9  | 10  | 11  | 12  |
|13.5 |14.5 |15.5 |16.5 |
""" )--") {
        Matrix M {{
                { 1, 2, 3, 4 },
                { 5.5, 6.5, 7.5, 8.5 },
                { 9, 10, 11, 12 },
                { 13.5, 14.5, 15.5, 16.5}
        }};

        THEN("M[0,0] = 1") {
            REQUIRE(M.at(0, 0) == 1);
        }

        AND_THEN("M[0,3] = 4") {
            REQUIRE(M.at(0, 3) == 4);
        }

        AND_THEN("M[1,0] = 5.5") {
            REQUIRE(safeCompare(M.at(1, 0), 5.5));
        }

        AND_THEN("M[1,2] = 7.5") {
            REQUIRE(safeCompare(M.at(1, 2), 7.5));
        }

        AND_THEN("M[2,2] = 11") {
            REQUIRE(M.at(2, 2) == 11);
        }

        AND_THEN("M[3,0] = 13.5") {
            REQUIRE(safeCompare(M.at(3, 0), 13.5));
        }

        AND_THEN("M[3,2] = 15.5") {
            REQUIRE(safeCompare(M.at(3, 2), 15.5));
        }
    }
}

SCENARIO("A 2x2 matrix ought to be representable") {
    GIVEN(R"--(The following 2x2 matrix M:
"""
| -3  |  5  |
|  1  | -2  |
""" )--") {
        Matrix M {{
            { -3, 5 },
            { 1, -2 }
        }};

        THEN("M[0,0] = -3") {
            REQUIRE(M.at(0, 0) == -3);
        }

        AND_THEN("M[0,1] = 5") {
            REQUIRE(M.at(0, 1) == 5);
        }

        AND_THEN("M[1,0] = 1") {
            REQUIRE(M.at(1, 0) == 1);
        }

        AND_THEN("M[1,1] = -2") {
            REQUIRE(M.at(1, 1) == -2);
        }
    }
}

SCENARIO("A 3x3 matrix ought to be representable") {
    GIVEN(R"--(The following 3x3 matrix M:
"""
| -3  |  5  |  0  |
|  1  | -2  | -7  |
|  0  |  1  |  1  |
""" )--") {
        Matrix M {{
            { -3, 5, 0 },
            { 1, -2, -7},
            { 0, 1, 1 }
        }};

        THEN("M[0,0] = -3") {
            REQUIRE(M.at(0, 0) == -3);
        }

        AND_THEN("M[1,1] = -2") {
            REQUIRE(M.at(1, 1) == -2);
        }

        AND_THEN("M[2,2] = 1") {
            REQUIRE(M.at(2, 2) == 1);
        }
    }
}

SCENARIO("Matrix equality with two identical matrices") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| 1 | 2 | 3 | 4 |
| 5 | 6 | 7 | 8 |
| 9 | 8 | 7 | 6 |
| 5 | 4 | 3 | 2 |
""" )--") {
        Matrix A {{
            { 1, 2, 3, 4 },
            { 5, 6, 7, 8 },
            { 9, 8, 7, 6 },
            { 5, 4, 3, 2 }
        }};

        AND_GIVEN(R"--(The following 4x4 matrix B:
"""
| 1 | 2 | 3 | 4 |
| 5 | 6 | 7 | 8 |
| 9 | 8 | 7 | 6 |
| 5 | 4 | 3 | 2 |
""" )--") {
            Matrix B {{
                { 1, 2, 3, 4 },
                { 5, 6, 7, 8 },
                { 9, 8, 7, 6 },
                { 5, 4, 3, 2 }
            }};

            THEN("A = B") {
                REQUIRE(A == B);
            }
        }
    }
}

SCENARIO("Matrix equality with different matrices") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| 1 | 2 | 3 | 4 |
| 5 | 6 | 7 | 8 |
| 9 | 8 | 7 | 6 |
| 5 | 4 | 3 | 2 |
""" )--") {
        Matrix A {{
            { 1, 2, 3, 4 },
            { 5, 6, 7, 8 },
            { 9, 8, 7, 6 },
            { 5, 4, 3, 2 }
        }};

        AND_GIVEN(R"--(The following 4x4 matrix B:
"""
| 2 | 3 | 4 | 5 |
| 6 | 7 | 8 | 9 |
| 8 | 7 | 6 | 5 |
| 4 | 3 | 2 | 1 |
""" )--") {
            Matrix B {{
                { 2, 3, 4, 5 },
                { 6, 7, 8, 9 },
                { 8, 7, 6, 5 },
                { 4, 3, 2, 1 }
            }};

            THEN("A != B") {
                REQUIRE(A != B);
            }
        }
    }
}



SCENARIO("Multiplying two matrices") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| 1 | 2 | 3 | 4 |
| 5 | 6 | 7 | 8 |
| 9 | 8 | 7 | 6 |
| 5 | 4 | 3 | 2 |
""" )--") {
        Matrix A {{
            { 1, 2, 3, 4 },
            { 5, 6, 7, 8 },
            { 9, 8, 7, 6 },
            { 5, 4, 3, 2 }
        }};

        AND_GIVEN(R"--(The following 4x4 matrix B:
"""
| -2 |  1 |  2 |  3 |
|  3 |  2 |  1 | -1 |
|  4 |  3 |  6 |  5 |
|  1 |  2 |  7 |  8 |
""" )--") {
            Matrix B {{
                { -2, 1, 2, 3 },
                { 3, 2, 1, -1 },
                { 4, 3, 6, 5 },
                { 1, 2, 7, 8 }
            }};

            THEN(R"--(A * B is the following 4x4 matrix:
"""
| 20 | 22 | 50 | 48 |
| 44 | 54 |114 |108 |
| 40 | 58 |110 |102 |
| 16 | 26 | 46 | 42 |
""" )--") {
                Matrix expected {{
                    { 20, 22, 50, 48 },
                    { 44, 54, 114, 108 },
                    { 40, 58, 110, 102 },
                    { 16, 26, 46, 42 }
                }};

                Matrix result = A * B;

                REQUIRE(result == expected);
            }
        }
    }
}

SCENARIO("A matrix multiplied by a tuple") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| 1 | 2 | 3 | 4 |
| 2 | 4 | 4 | 2 |
| 8 | 6 | 4 | 1 |
| 0 | 0 | 0 | 1 |
""" )--") {
        Matrix A {{
            { 1, 2, 3, 4 },
            { 2, 4, 4, 2 },
            { 8, 6, 4, 1 },
            { 0, 0, 0, 1 }
        }};

        AND_GIVEN("The following tuple B: { 1, 2, 3, 1 }") {
            Tuple B = { 1, 2, 3, 1 };

            THEN("A * B = { 18, 24, 33, 1 }") {
                Tuple expected = {18, 24, 33, 1};

                Tuple result = Tuple(A * B);

                REQUIRE(result == expected);
            }
        }
    }
}


SCENARIO("Multiplying a Matrix by the Identity Matrix") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| 0 | 1 | 2  | 4  |
| 1 | 2 | 4  | 8  |
| 2 | 4 | 8  | 16 |
| 4 | 8 | 16 | 32 |
""" )--") {
        Matrix A {{
            { 0, 1, 2, 4 },
            { 1, 2, 4, 8 },
            { 2, 4, 8, 16 },
            { 4, 8, 16, 32 }
        }};

        THEN("A * Identity Matrix = A") {
            REQUIRE(A * Matrix::identity() == A);
        }
    }
}

SCENARIO("Transposing a Matrix") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| 0 | 9 | 3 | 0 |
| 9 | 8 | 0 | 8 |
| 1 | 8 | 5 | 3 |
| 0 | 0 | 5 | 8 |
""" )--") {
        Matrix A {{
            { 0, 9, 3, 0 },
            { 9, 8, 0, 8 },
            { 1, 8, 5, 3 },
            { 0, 0, 5, 8 }
        }};

        THEN(R"--(transpose(A) is the following 4x4 matrix:
"""
| 0 | 9 | 1 | 0 |
| 9 | 8 | 8 | 0 |
| 3 | 0 | 5 | 5 |
| 0 | 8 | 3 | 8 |
""" )--") {
            Matrix expected {{
                { 0, 9, 1, 0 },
                { 9, 8, 8, 0 },
                { 3, 0, 5, 5 },
                { 0, 8, 3, 8 }
            }};

            REQUIRE(Matrix::transpose(A) == expected);
        }
    }
}

SCENARIO("Calculating the determinant of a 2x2 matrix") {
    GIVEN(R"--(The following 2x2 matrix A:
"""
| 1  | 5 |
| -3 | 2 |
""")--") {
        Matrix A {{
            { 1, 5 },
            { -3, 2 }
        }};

        THEN("determinant(A) = 17") {
            REQUIRE(Matrix::determinant(A) == 17);
        }
    }
}

SCENARIO("The submatrix of a 3x3 matrix is a 2x2 matrix") {
    GIVEN(R"--(The following 3x3 matrix A:
"""
| 1  | 5 | 0  |
| -3 | 2 | 7  |
| 0  | 6 | -3 |
""")--") {
        Matrix A {{
            { 1, 5, 0 },
            { -3, 2, 7 },
            { 0, 6, -3 }
        }};

        THEN(R"--(sub(A, 0, 2) is the following 2x2 matrix:
"""
| -3 | 2 |
|  0 | 6 |
""")--") {
            Matrix expected {{
                { -3, 2 },
                { 0, 6 }
            }};

            REQUIRE(Matrix::sub(A, 0, 2) == expected);
        }
    }
}

SCENARIO("The submatrix of a 4x4 matrix is a 3x3 matrix") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| -6 | 1 | 1  | 6 |
| -8 | 5 | 8  | 6 |
| -1 | 0 | 8  | 2 |
| -7 | 1 | -1 | 1 |
""" )--") {
        Matrix A {{
            { -6, 1, 1, 6 },
            { -8, 5, 8, 6 },
            { -1, 0, 8, 2 },
            { -7, 1, -1, 1 }
        }};

        THEN(R"--(sub(A, 2, 1) is the following 3x3 matrix:
"""
| -6 | 1  | 6 |
| -8 | 8  | 6 |
| -7 | -1 | 1 |
""" )--") {
            Matrix expected {{
                { -6, 1, 6 },
                { -8, 8, 6 },
                { -7, -1, 1 }
            }};

            REQUIRE(Matrix::sub(A, 2, 1) == expected);
        }
    }
}

SCENARIO("Calculating the minor of a 3x3 matrix") {
    GIVEN(R"--(The following 3x3 matrix A:
"""
| 3 |  5 |  0 |
| 2 | -1 | -7 |
| 6 | -1 |  5 |
""" )--") {
        Matrix A {{
            { 3, 5, 0 },
            { 2, -1, -7 },
            { 6, -1, 5 }
        }};

        AND_GIVEN("The submatrix B: sub(A, 1, 0)") {
            Matrix B = Matrix::sub(A, 1, 0);

            THEN("determinant(B) = 25") {
                REQUIRE(Matrix::determinant(B) == 25);
            }

            AND_THEN("minor(A, 1, 0) = 25") {
                REQUIRE(Matrix::minor(A, 1, 0) == 25);
            }
        }
    }
}

SCENARIO("Calculating the cofactor of a 3x3 matrix") {
    GIVEN(R"--(The following 3x3 matrix A:
"""
| 3 |  5 |  0 |
| 2 | -1 | -7 |
| 6 | -1 |  5 |
""" )--") {
        Matrix A {{
            { 3, 5, 0 },
            { 2, -1, -7 },
            { 6, -1, 5 }
        }};

        THEN("minor(A, 0, 0) = -12") {
            REQUIRE(Matrix::minor(A, 0, 0) == -12);
        }

        AND_THEN("cofactor(A, 0, 0) = -12") {
            REQUIRE(Matrix::cofactor(A, 0, 0) == -12);
        }

        AND_THEN("minor(A, 1, 0) = 25") {
            REQUIRE(Matrix::minor(A, 1, 0) == 25);
        }

        AND_THEN("cofactor(A, 1, 0) = -25") {
            REQUIRE(Matrix::cofactor(A, 1, 0) == -25);
        }
    }
}

SCENARIO("Calculating the determinant of a 3x3 matrix") {
    GIVEN(R"--(The following 3x3 matrix A:
"""
|  1 | 2 |  6 |
| -5 | 8 | -4 |
|  2 | 6 |  4 |
""" )--") {
        Matrix A {{
            { 1, 2, 6 },
            { -5, 8, -4 },
            { 2, 6, 4 }
        }};

        THEN("cofactor(A, 0, 0) = 56") {
            REQUIRE(Matrix::cofactor(A, 0, 0) == 56);
        }

        AND_THEN("cofactor(A, 0, 1) = 12") {
            REQUIRE(Matrix::cofactor(A, 0, 1) == 12);
        }

        AND_THEN("cofactor(A, 0, 2) = -46") {
            REQUIRE(Matrix::cofactor(A, 0, 2) == -46);
        }

        AND_THEN("determinant(A) = -196") {
            REQUIRE(Matrix::determinant(A) == -196);
        }
    }
}

SCENARIO("Calculating the determinant of a 4x4 matrix") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| -2 | -8 |  3 |  5 |
| -3 |  1 |  7 |  3 |
|  1 |  2 | -9 |  6 |
| -6 |  7 |  7 | -9 |
""" )--") {
        Matrix A {{
            { -2, -8, 3, 5 },
            { -3, 1, 7, 3 },
            { 1, 2, -9, 6 },
            { -6, 7, 7, -9 }
        }};

        THEN("cofactor(A, 0, 0) = 690") {
            REQUIRE(Matrix::cofactor(A, 0, 0) == 690);
        }

        AND_THEN("cofactor(A, 0, 1) = 447") {
            REQUIRE(Matrix::cofactor(A, 0, 1) == 447);
        }

        AND_THEN("cofactor(A, 0, 2) = 210") {
            REQUIRE(Matrix::cofactor(A, 0, 2) == 210);
        }

        AND_THEN("cofactor(A, 0, 3) = 51") {
            REQUIRE(Matrix::cofactor(A, 0, 3) == 51);
        }

        AND_THEN("determinant(A) = -4071") {
            REQUIRE(Matrix::determinant(A) == -4071);
        }
    }
}

SCENARIO("Testing an invertible matrix for invertibility") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| 6 |  4 | 4 |  4 |
| 5 |  5 | 7 |  6 |
| 4 | -9 | 3 | -7 |
| 9 |  1 | 7 | -6 |
""" )--") {
        Matrix A {{
            { 6, 4, 4, 4 },
            { 5, 5, 7, 6 },
            { 4, -9, 3, -7 },
            { 9, 1, 7, -6 }
        }};

        THEN("determinant(A) = -2120") {
            REQUIRE(Matrix::determinant(A) == -2120);
        }

        AND_THEN("A is invertible") {
            REQUIRE(Matrix::determinant(A) != 0);
        }
    }
}

SCENARIO("Testing a non-invertible matrix for invertibility") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| -4 |  2 | -2 | -3 |
|  9 |  6 |  2 |  6 |
|  0 | -5 |  1 | -5 |
|  0 |  0 |  0 |  0 |
""" )--") {
        Matrix A {{
            { -4, 2, -2, -3 },
            { 9, 6, 2, 6 },
            { 0, -5, 1, -5 },
            { 0, 0, 0, 0 }
        }};

        THEN("determinant(A) = 0") {
            REQUIRE(Matrix::determinant(A) == 0);
        }

        AND_THEN("A is not invertible") {
            REQUIRE(Matrix::determinant(A) == 0);
        }
    }
}

SCENARIO("Calculating the inverse of a matrix") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
| -5 |  2 |  6 | -8 |
|  1 | -5 |  1 |  8 |
|  7 |  7 | -6 | -7 |
|  1 | -3 |  7 |  4 |
""" )--") {
        Matrix A {{
            { -5, 2, 6, -8 },
            { 1, -5, 1, 8 },
            { 7, 7, -6, -7 },
            { 1, -3, 7, 4 }
        }};

        AND_GIVEN("The inverse matrix of A: B") {
            Matrix B = Matrix::inverse(A);

            THEN("determinant(A) = 532") {
                REQUIRE(Matrix::determinant(A) == 532);
            }

            AND_THEN("cofactor(A, 2, 3) = -160") {
                REQUIRE(Matrix::cofactor(A, 2, 3) == -160);
            }

            AND_THEN("B[3,2] = -160/532") {
                REQUIRE(safeCompare(B.at(3, 2), (double) -160/532));
            }

            AND_THEN("cofactor(A, 3, 2) = 105") {
                REQUIRE(Matrix::cofactor(A, 3, 2) == 105);
            }

            AND_THEN("B[2,3] = 105/532") {
                REQUIRE(safeCompare(B.at(2, 3), (double) 105/532));
            }

            AND_THEN(R"--(B is the following 4x4 matrix:
"""
|  0.21805 |  0.45113 |  0.24060 | -0.04511 |
| -0.80827 | -1.45677 | -0.44361 |  0.52068 |
| -0.07895 | -0.22368 | -0.05263 |  0.19737 |
| -0.52256 | -0.81391 | -0.30075 |  0.30639 |
""" )--") {
                Matrix expected {{
                    { 0.21805, 0.45113, 0.24060, -0.04511 },
                    { -0.80827, -1.45677, -0.44361, 0.52068 },
                    { -0.07895, -0.22368, -0.05263, 0.19737 },
                    { -0.52256, -0.81391,  -0.30075, 0.30639 }
                }};

                REQUIRE(B == expected);
            }
        }
    }
}

SCENARIO("Calculating the inverse of a matrix 2") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
|  8 | -5 |  9 |  2 |
|  7 |  5 |  6 |  1 |
| -6 |  0 |  9 |  6 |
| -3 |  0 | -9 | -4 |
""" )--") {
        Matrix A {{
            { 8, -5, 9, 2 },
            { 7, 5, 6, 1 },
            { -6, 0, 9, 6 },
            { -3, 0, -9, -4 }
        }};


        THEN(R"--(inverse(A) is the following 4x4 matrix:
"""
| -0.15385 | -0.15385 | -0.28205 | -0.53846 |
| -0.07692 |  0.12308 |  0.02564 |  0.03077 |
|  0.35897 |  0.35897 |  0.43590 |  0.92308 |
| -0.69231 | -0.69231 | -0.76923 | -1.92308 |
""" )--") {
            Matrix expected {{
                { -0.15385, -0.15385, -0.28205, -0.53846 },
                { -0.07692, 0.12308, 0.02564, 0.03077 },
                { 0.35897, 0.35897, 0.43590, 0.92308 },
                { -0.69231, -0.69231, -0.76923, -1.92308 }
            }};

            REQUIRE(Matrix::inverse(A) == expected);
        }
    }
}

SCENARIO("Multiplying a matrix by its' inverse") {
    GIVEN(R"--(The following 4x4 matrix A:
"""
|  3 | -9 |  7 |  3 |
|  3 | -8 |  2 | -9 |
| -4 |  4 |  4 |  1 |
| -6 |  5 | -1 |  1 |
""" )--") {
        Matrix A {{
            { 3, -9, 7, 3 },
            { 3, -8, 2, -9 },
            { -4, 4, 4, 1 },
            { -6, 5, -1, 1}
        }};

        AND_GIVEN(R"--(The following 4x4 matrix B:
"""
| 8 |  2 | 2 | 2 |
| 3 | -1 | 7 | 0 |
| 7 |  0 | 5 | 4 |
| 6 | -2 | 0 | 5 |
""" )--") {
            Matrix B {{
                { 8, 2, 2, 2},
                { 3, -1, 7, 0 },
                { 7, 0, 5, 4 },
                { 6, -2, 0, 5 }
            }};

            AND_GIVEN("The composition C: A * B") {
                Matrix C = A * B;

                THEN("C * inverse(B) = A") {
                    REQUIRE(C * Matrix::inverse(B) == A);
                }
            }
        }
    }
}