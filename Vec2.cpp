#pragma once
#include "Vec2.h"
#include <math.h>

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
	:x(xin),y(yin) {

}

bool Vec2::operator == (const Vec2& rhs) const {
	if (this->x == rhs.x && this->y == rhs.y) {
		return true;	
	}
	return false;
}

bool Vec2::operator != (const Vec2& rhs) const {
	if (this->x == rhs.x && this->y == rhs.y) {
		return false;
	}
	return true;
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
	return Vec2(this->x + rhs.x, this->y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
	return Vec2(this->x - rhs.x, this->y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const {
	return Vec2(this->x * val, this->y * val);
}

Vec2 Vec2::operator / (const float val) const {
	return Vec2(this->x / val, this->y / val);
}

void Vec2::operator += (const Vec2& rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
}

void Vec2::operator *= (const float val) {
	this->x *= val;
	this->y *= val;
}

void Vec2::operator /= (const float val) {
	this->x /= val;
	this->y /= val;
}

std::ostream& operator << (std::ostream& os, const Vec2& rhs) {
	os << "x=" << rhs.x << " y=" << rhs.y;
	return os;
}