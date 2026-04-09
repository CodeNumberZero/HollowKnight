#include "MyVector.h"

MyVector::MyVector(float x, float y) : x(x), y(y)
{
}

MyVector MyVector::operator+(const MyVector& vec) const
{
	return MyVector(x + vec.x, y + vec.y);
}

MyVector MyVector::operator-(const MyVector& vec) const
{
	return MyVector(x - vec.x, y - vec.y);
}

float MyVector::operator*(const MyVector& vec) const
{
	return x * vec.x + y * vec.y;
}

MyVector MyVector::operator*(float val) const
{
	return MyVector(x * val, y * val);
}

void MyVector::operator+=(const MyVector& vec)
{
	x += vec.x;
	y += vec.y;
}

void MyVector::operator-=(const MyVector& vec)
{
	x -= vec.x;
	y -= vec.y;
}

void MyVector::operator*=(float val)
{
	x *= val;
	y *= val;
}

float MyVector::length()
{
	return std::sqrt(x * x + y * y);
}

MyVector MyVector::normalize()
{
	float len = length();
	if (len == 0) {
		return MyVector(0, 0);
	}
	return MyVector(x / len, y / len);
}


