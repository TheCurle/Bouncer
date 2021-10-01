/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <math.h>

/**
 * Represents a 4-dimensional coordinate.
 * The parent of all position and direction markers.
 * 
 * Not to be used directly. Instantiate one of the children.
 */
struct Tuple {
    
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

    // Overloaded negation operator. Have to negate each element individually.
    Tuple operator-() {
        return Tuple(-x, -y, -z, -w);
    }

    double x;
    double y;
    double z;
    double w;

    // Default constructor for subclasses
    Tuple() { x = y = z = w = 0; }
    

    // Compare two doubles with tolerance.
    bool safeCompare(double a, double b) const {
        return abs(a - b) < epsilon;
    }

    private:
    double epsilon = 0.001;
};


/**
 * Represents a 3-dimensional direction and magnitude.
 */
class Vector : public Tuple {
    public:
    
    bool operator==(const Tuple& other) const {
        return safeCompare(other.x, x) && safeCompare(other.y, y) &&
                 safeCompare(other.z, z) && safeCompare(other.w, w);
    }

    Vector operator-(const Vector& other) {
        return Vector(x - other.x, y - other.y, z - other.z);
    }

    Vector(double a, double b, double c) {
        x=a; y=b; z=c; w=0;
    }
};

/**
 * Represents an absolute 3-dimensional position.
 */
class Point : public Tuple {
    public:

    bool operator==(const Tuple& other) const {
        return safeCompare(other.x, x) && safeCompare(other.y, y) &&
                 safeCompare(other.z, z) && safeCompare(other.w, w);
    }

    Vector operator-(const Point& other) const {
        return Vector(x - other.x, y - other.y, z - other.z);
    }
    
    Point operator-(const Vector& other) const {
        return Point(x - other.x, y - other.y, z - other.z);
    }

    Point(double a, double b, double c) {
        x=a; y=b; z=c; w=1;
    }
};
