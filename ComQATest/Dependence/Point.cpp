

#include "Point.h"

using namespace MadTomDev::Base;

// ############################# Point(D) #############################

Point::Point(double x, double y)
{
    this->x = x;
    this->y = y;
}
Point::Point(const char *xCmaY)
{
    FromStr(xCmaY);
}
char *Point::ToStr()
{
    char result[48];
    sprintf(result, "%g,%g", x, y);
    return result;
}
bool Point::FromStr(const char *xCmaY)
{
    bool result = false;
    try
    {
        double a, b;
        sscanf(xCmaY, "%g,%g", a, b);
        x = a;
        y = b;
        result = true;
    }
    catch (...)
    {
    }
    return result;
}
void Point::operator=(Point &b)
{
    this->x = b.x;
    this->y = b.y;
}
Point Point::operator+(Point &b)
{
    return Point(this->x + b.x, this->y + b.y);
}
Point Point::operator-(Point &b)
{
    return Point(this->x - b.x, this->y - b.y);
}
Point &Point::operator+=(Point &b)
{
    this->x += b.x;
    this->y += b.y;
    return *this;
}
Point &Point::operator-=(Point &b)
{
    this->x -= b.x;
    this->y -= b.y;
    return *this;
}
bool Point::operator==(Point &b)
{
    return this->x == b.x && this->y == b.y;
}
bool Point::operator!=(Point &b)
{
    return !(*this == b);
}

// ############################# PointL #############################

PointL::PointL(long x, long y)
{
    this->x = x;
    this->y = y;
}
PointL::PointL(const char *xCmaY)
{
    FromStr(xCmaY);
}
char *PointL::ToStr()
{
    char result[48];
    sprintf(result, "%g,%g", x, y);
    return result;
}
bool PointL::FromStr(const char *xCmaY)
{
    bool result = false;
    try
    {
        long a, b;
        sscanf(xCmaY, "%g,%g", a, b);
        x = a;
        y = b;
        result = true;
    }
    catch (...)
    {
    }
    return result;
}
void PointL::operator=(PointL &b)
{
    this->x = b.x;
    this->y = b.y;
}
PointL PointL::operator+(PointL &b)
{
    return PointL(this->x + b.x, this->y + b.y);
}
PointL PointL::operator-(PointL &b)
{
    return PointL(this->x - b.x, this->y - b.y);
}
PointL &PointL::operator+=(PointL &b)
{
    this->x += b.x;
    this->y += b.y;
    return *this;
}
PointL &PointL::operator-=(PointL &b)
{
    this->x -= b.x;
    this->y -= b.y;
    return *this;
}
bool PointL::operator==(PointL &b)
{
    return this->x == b.x && this->y == b.y;
}
bool PointL::operator!=(PointL &b)
{
    return !(*this == b);
}

// ############################# PointT #############################

template <typename T>
PointT<T>::PointT(T x, T y)
{
    this->x = x;
    this->y = y;
}
template <typename T>
void PointT<T>::operator=(PointT &b)
{
    this->x = b.x;
    this->y = b.y;
}
template <typename T>
PointT<T> PointT<T>::operator+(PointT &b)
{
    return PointT(this->x + b.x, this->y + b.y);
}
template <typename T>
PointT<T> PointT<T>::operator-(PointT &b)
{
    return PointT(this->x - b.x, this->y - b.y);
}
template <typename T>
PointT<T> &PointT<T>::operator+=(PointT &b)
{
    this->x += b.x;
    this->y += b.y;
    return *this;
}
template <typename T>
PointT<T> &PointT<T>::operator-=(PointT &b)
{
    this->x -= b.x;
    this->y -= b.y;
    return *this;
}
template <typename T>
bool PointT<T>::operator==(PointT &b)
{
    return this->x == b.x && this->y == b.y;
}
template <typename T>
bool PointT<T>::operator!=(PointT &b)
{
    return !(*this == b);
}