/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_all.hpp>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>

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

    // Overloaded equality operator, to account for the weirdness of the 4 dimensions.
    bool operator==(const TupleContainer& other) const {
        return safeCompare(other.x, x) && safeCompare(other.y, y) &&
                 safeCompare(other.z, z) && safeCompare(other.w, w);
    }
    
    // Compare two doubles with tolerance.
    static bool safeCompare(double a, double b) {
        return std::abs(a - b) < epsilon;
    }

    // The tolerance for comparisons.
    static constexpr double epsilon = 0.00001;
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
    Tuple(double a, double b, double c, double d) : TupleContainer() {
        x = a; y = b; z = c; w = d;
    }

    explicit Tuple(std::vector<double> data) : TupleContainer() {
        x = data[0]; y = data[1]; z = data[2]; w = data[3];
    }

    Tuple(Tuple& other) {
        x = other.x; y = other.y; z = other.z; w = other.w;
    }

    // Returns true if this tuple represents a location.
    bool isPoint() { return w == 1; }
    // Returns true if this tuple represents a direction and magnitude vector.
    bool isVector() { return w == 0; }

    // Overloaded addition operator. Have to add each element individually.
    Tuple operator+(const Tuple& other) {
        return {other.x + x, other.y + y, other.z + z, other.w + w};
    }

    // Overloaded multiplication operator. Have to multiply each element individually.
    Tuple operator*(const double& dub) {
        return {x * dub, y * dub, z * dub, w * dub};
    }
    
    // Overloaded division operator. Have to divide each element individually.
    Tuple operator/(const double& dub) {
        return {x / dub, y / dub, z / dub, w / dub};
    }
    // Overloaded negation operator. Have to negate each element individually.
    Tuple operator-() {
        return {-x, -y, -z, -w};
    }

    // Return the absolute magnitude of this vector from start-finish.
    double magnitude() {
        return sqrt((x*x) + (y*y) + (z*z) + (w*w));
    }

    // Default constructor for subclasses
    Tuple() : TupleContainer() { x = y = z = w = 0; }
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
        return {x - other.x, y - other.y, z - other.z};
    }

    // Vector dot-product overload.
    double operator*(const Vector& other) {
        return x * other.x +
                 y * other.y +
                 z * other.z;
    }

    // Multiply components by scalar
    Vector operator*(const double& other) {
        return { x * other, y * other, z * other };
    }

    // Calculate the cross-product of this and the passed vector.
    Vector cross(const Vector& other) {
        return {y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x};
    }

    // Tuple copy-constructor.
    explicit Vector(const Tuple& other) {
        x=other.x; y=other.y; z=other.z; w=0;
    }

    // Vector value constructor.
    Vector(double a, double b, double c) {
        x=a; y=b; z=c; w=0;
    }

    // Vector copy-constructor
    Vector(Vector& other) : Tuple(other) {
        x = other.x; y = other.y; z = other.z; w = 0;
    }

    // Normalize this vector to a unit vector.
    Tuple normalize() {
        double mag = magnitude();
        if(mag < Tuple::epsilon)
            return {0, 0, 0, 0};

        return {x / mag, y / mag, z / mag, w};
    }

    // Reflect this vector around the given normal.
    Vector reflect(Vector normal) {
        return *this - normal * 2 * (*this * normal);
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
        return {x - other.x, y - other.y, z - other.z};
    }
    
    // Point - Vector subtraction overload.
    // Returns the Point calculated by moving this Point by the given Vector.
    Point operator-(const Vector& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    // Point value constructor.
    Point(double a, double b, double c) {
        x=a; y=b; z=c; w=1;
    }

    // Tuple copy-constructor.
    explicit Point(const Tuple& other) {
        x=other.x; y=other.y; z=other.z; w=1;
    }

    Point(Point& other) : Tuple(other) {
        x = other.x; y = other.y; z = other.z; w = 1;
    }
};

// Specializations to allow Catch to print these types

namespace Catch {
    template < >
    struct StringMaker<Point> {
        static std::string convert(Point const &p) {
            std::stringstream buf;
            buf << "Point (" << p.x << ", " << p.y << ", " << p.z << ")";
            return buf.str();
        }
    };

    template < >
    struct StringMaker<Vector> {
        static std::string convert(Vector const &v) {
            std::stringstream buf;
            buf << "Vector (" << v.x << ", " << v.y << ", " << v.z << ")";
            return buf.str();
        }
    };

    template < >
    struct StringMaker<Tuple> {
        static std::string convert(Tuple const &t) {
            std::stringstream buf;
            buf << "Tuple (" << t.x << ", " << t.y << ", " << t.z << ", " << t.w << ")";
            return buf.str();
        }
    };
}