// Chris Rook. Northumbria University
// 24/04/2012	Added magnitudeSquared.
// Modified 04/11/2018
// Changed rotatedBy to use angles clockwise
// Modified 06/09/2019
//	Added type double use of *, / , *= and /=
//   Also int for division
// 5/5/2023 Changed angle units to degrees, ready for Hornet Engine
// Added functions to restrict the angle to 0 to 360 and to calculate
// the shortest distance between angles


#ifndef VECTOR_H
#define VECTOR_H
#include <cmath>

class Vector2D
{
public:
	static const double PI;// = 3.1415927f;
	double XValue;
	double YValue;

	Vector2D();

	Vector2D(double x, double y);

	Vector2D(const Vector2D& copyVector);

	// Returns a unit vector in the direction of the current vector
	Vector2D unitVector() const;

	// Vector addition
	Vector2D operator+ (const Vector2D& addVector) const;

	// Vector subtraction
	Vector2D operator- (const Vector2D& minusVector) const;

	// Scalar multiplication
	Vector2D operator* (float multiplier) const;

	// Scalar multiplication
	Vector2D operator* (double multiplier) const;

	// Scalar multiplication
	Vector2D operator* (int multiplier) const;

	// Scalar division (same effect as multiplying by 1/divisor)
	Vector2D operator/ (float divisor) const;

	// Scalar division (same effect as multiplying by 1/divisor)
	Vector2D operator/ (double divisor) const;

	// Scalar division (same effect as multiplying by 1/divisor)
	Vector2D operator/ (int divisor) const;

	// Vector addition
	void operator+=(const Vector2D& addVector);

	// Vector subtraction
	void operator-=(const Vector2D& addVector);

	// Scalar multiplication
	void operator*=(float multiplier);

	// Scalar multiplication
	void operator*=(double multiplier);

	// Scalar multiplication
	void operator*=(int multiplier);

	// Scalar division
	void operator/=(float divisor);

	// Scalar division
	void operator/=(double divisor);

	// Scalar division
	void operator/=(int divisor);

	// Dot operator
	double operator* (const Vector2D& otherVector) const;

	// Negative of a vector - same as vector, but in opposite direction
	Vector2D operator-() const;

	// Comparison of vectors
	bool operator==(const Vector2D& otherVector) const;

	// Returns the angle of the vector from the (0,1) direction, in degrees
	double angle() const;

	// Sets the components of the vector
	void set(double x, double y);

	// Copies from another vector. Effectively identical to the = operator
	void set(const Vector2D& copyVector);

	// Returns the magnitude of the vector
	double magnitude() const;

	// Returns the square of the magnitude of the vector
	// (hence avoiding a square root.)
	double magnitudeSquared() const;

	// Sets the vector, using the angle (in degrees) from the (0,1) direction and the magnitude
	void setBearing(double angle, double magnitude);

	// Returns a vector perpendicular to the current vector
	Vector2D perpendicularVector() const;

	// Returns true if the two vectors are parallel
	bool isParallelTo(const Vector2D& otherVector) const;

	// Returns true if the two vectors are perpendicular
	bool isPerpendicularTo(const Vector2D& otherVector) const;

	// Returns a vector of identical magnitude, but rotated by the specified angle (in degrees)
	Vector2D rotatedBy(double angle) const;


	// Converts an angle in radians to an angle in degrees
	static inline double RadiansToDegrees(double radians)
	{
		return radians * 180.0/PI;
	}

	// Converts an angle in degrees to an angle in radians
	static inline double DegreesToRadians(double degrees)
	{
		return degrees * PI/180.0;
	}

	// Restricts the angle to the range 0 to 360.
	// For example, if an angle is 375 this will be changed to 15 degrees
	// If an angle is -20 degrees this will change to 340 degrees
	static inline void Restrict(double& degrees)
	{
		if (degrees >= 360.0)
		{
			degrees = std::fmod(degrees, 360.0);
		}
		if (degrees < 0)
		{
			degrees = 360.0+std::fmod(degrees, 360.0);
		}
	}

	// This gives the shortest angle difference between two angles
	// If the first number is larger than the second number, the result will be negative.
	// The angles are restricted, so an angle of 365 degrees is treated as an angle of 5 degrees.
	// The result will be a number in the range -180 to +180.
	// Example:  AngleDifference(20, 45 ) = 25
	// Example:  AngleDifference(45, 20 ) = -25
	// Example:  AngleDifference(400, 40 ) = 0
	// Example:  AngleDifference(400, -390 ) = -10
	// Example:  AngleDifference(359, 1 ) = 2

	static inline double AngleDifference(double from, double to)
	{
		Restrict(from);
		Restrict(to);

		double answer = to - from;
		if (answer > 180.0)
		{
			answer = answer - 360.0;
		}
		if (answer <= -180.0)
		{
			answer = answer + 360.0;
		}
	}

};


// Global vector operators for when you want to use 5 x vector
// instead of vector x 5
Vector2D operator*(float multiplier, Vector2D v);
Vector2D operator*(int multiplier, Vector2D v);
Vector2D operator*(double multiplier, Vector2D v);


#endif
