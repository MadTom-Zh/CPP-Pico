
#ifndef MADTOMDEV_BASE_POINT_H
#define MADTOMDEV_BASE_POINT_H

#include <stdio.h>
#include <stdlib.h>
//#include <map>
//#include <vector>

using namespace std;

namespace MadTomDev
{
    namespace Base
    {
        class Point
        {
        private:
        public:
            double x,y;
            Point(double x=0, double y=0);
            Point(const char * xCmaY);
            char * ToStr();
            bool FromStr(const char * xCmaY);
            void operator=(Point & b);
            Point operator+(Point & b);
            Point operator-(Point & b);
            Point & operator +=(Point & b);
            Point & operator -=(Point & b);
            bool operator == (Point & b);
            bool operator != (Point & b);
        };

        class PointL
        {
        private:
        public:
            long x,y;
            PointL(long x=0, long y=0);
            PointL(const char * xCmaY);
            char * ToStr();
            bool FromStr(const char * xCmaY);
            void operator=(PointL & b);
            PointL operator+(PointL & b);
            PointL operator-(PointL & b);
            PointL & operator +=(PointL & b);
            PointL & operator -=(PointL & b);
            bool operator == (PointL & b);
            bool operator != (PointL & b);
        };
        
        template <typename T>
        class PointT
        {
        private:
        public:
            T x,y;
            
            PointT<T>(T x=0, T y=0);
            void operator=(PointT & b);
            PointT operator+(PointT & b);
            PointT operator-(PointT & b);
            PointT & operator +=(PointT & b);
            PointT & operator -=(PointT & b);
            bool operator == (PointT & b);
            bool operator != (PointT & b);
        };
    }
}

#endif