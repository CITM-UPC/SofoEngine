#pragma once

/// Math
#define PI		 3.1415926535897932384f
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// Invalid Index
#define INVALID_INDEX static_cast<unsigned long long>(-1)

/// Convert degrees to radians.
template <class T>
inline T ToRadians(const T degrees)
{
    return DEGTORAD * degrees;
}

/// Convert radians to degrees.
template <class T>
inline T ToDegrees(const T radians)
{
    return RADTODEG * radians;
}