/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <math.h>

#pragma once

/**
 * Represents a 4-dimensional coordinate.
 * The parent of all tuple structures
 * 
 * Not to be used directly. Use a subclass.
 */
struct TupleContainer {
    double x;
    double y;
    double z;
    double w;
};

/**
 * Represents a manipulable type.
 * Suitable for vectors, whether position or velocity.
 * 
 * Provides many operator overloads for utility.
 */
class Tuple : public TupleContainer {
public:
    // Value constructor. Set all values implicitly.
    Tuple(double a, double b, double c, double d) {
        x = a; y = b; z = c; w = d;
    }

    // Returns true if this tuple represents a location.
    bool isPoint() { return w == 1; }
    // Returns true if this tuple represents a direction and magnitude vector.
    bool isVector() { return w == 0; }

    // Overloaded equality operator, to account for the weirdness of the 4 dimensions.
    bool operator==(const Tuple& other) const {
        return safeCompare(other.x, x) && safeCompare(other.y, y) &&
                 safeCompare(other.z, z) && safeCompare(other.w, w);
    }

    // Overloaded addition operator. Have to add each element individually.
    Tuple operator+(const Tuple& other) {
        return Tuple(other.x + x, other.y + y, other.z + z, other.w + w);
    }

    // Overloaded multiplication operator. Have to multiply each element individually.
    Tuple operator*(const double& dub) {
        return Tuple(x * dub, y * dub, z * dub, w * dub);
    }
    
    // Overloaded division operator. Have to divide each element individually.
    Tuple operator/(const double& dub) {
        return Tuple(x / dub, y / dub, z / dub, w / dub);
    }
    // Overloaded negation operator. Have to negate each element individually.
    Tuple operator-() {
        return Tuple(-x, -y, -z, -w);
    }

    // Return the absolute magnitude of this vector from start-finish.
    double magnitude() {
        return sqrt((x*x) + (y*y) + (z*z) + (w*w));
    }

    // Default constructor for subclasses
    Tuple() { x = y = z = w = 0; }

    // Compare two doubles with tolerance.
    static bool safeCompare(double a, double b) {
        return abs(a - b) < epsilon;
    }

    // The tolerance for comparisons.
    static constexpr double epsilon = 0.00001;
};


/**
 * Represents a 3-dimensional direction and magnitude.
 */
class Vector : public Tuple {
    public:
    
    // Overloaded equality operator to safe-compare the components.
    bool operator==(const Tuple& other) const {
        return safeCompare(other.x, x) && safeCompare(other.y, y) &&
                 safeCompare(other.z, z) && safeCompare(other.w, w);
    }

    // Vector subtraction overload.
    Vector operator-(const Vector& other) {
        return Vector(x - other.x, y - other.y, z - other.z);
    }

    // Vector dot-product overload.
    double operator*(const Vector& other) {
        return x * other.x +
                 y * other.y + 
                 z * other.z + 
                 w * other.w;
    } 

    // Calculate the cross-product of this and the passed vector.
    Vector cross(const Vector& other) {
        return Vector(y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x);
    }

    // Tuple copy-constructor.
    Vector(const Tuple& other) {
        x=other.x; y=other.y; z=other.y; w=0;
    }

    // Vector value constructor.
    Vector(double a, double b, double c) {
        x=a; y=b; z=c; w=0;
    }
    
    // Normalize this vector to a unit vector.
    Tuple normalize() {
        double mag = magnitude();
        if(mag < Tuple::epsilon)
            return Tuple(0, 0, 0, 0);

        return Tuple(x / mag, y / mag, z / mag, w);
    }
};

/**
 * Represents an absolute 3-dimensional position.
 */
class Point : public Tuple {
    public:

    // Overloaded equality operator to safe-compare the components.
    bool operator==(const Tuple& other) const {
        return safeCompare(other.x, x) && safeCompare(other.y, y) &&
                 safeCompare(other.z, z) && safeCompare(other.w, w);
    }

    // Point - Point subtraction overload. 
    // Returns a vector that leads from one point to the other.
    Vector operator-(const Point& other) const {
        return Vector(x - other.x, y - other.y, z - other.z);
    }
    
    // Point - Vector subtraction overload.
    // Returns the Point calculated by moving this Point by the given Vector.
    Point operator-(const Vector& other) const {
        return Point(x - other.x, y - other.y, z - other.z);
    }

    // Point value constructor.
    Point(double a, double b, double c) {
        x=a; y=b; z=c; w=1;
    }

    // Tuple copy-constructor.
    Point(const Tuple& other) {
        x=other.x; y=other.y; z=other.z; w=1;
    }
};
