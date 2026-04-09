#pragma once
#include <cmath>

// 自定义二维向量类
class MyVector
{
public:
	float x = 0;
	float y = 0;

	MyVector() = default;
	~MyVector() = default;
	MyVector(float x, float y);

	MyVector operator+(const MyVector& vec) const;
	MyVector operator-(const MyVector& vec) const;
	float operator*(const MyVector& vec) const;
	MyVector operator*(float val) const;
	void operator+=(const MyVector& vec);
	void operator-=(const MyVector& vec);
	void operator*=(float val);

	float length();
	MyVector normalize();                              // 返回当前向量的单位向量
};

