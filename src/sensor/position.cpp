#include "position.h"
#include <math.h>

Position::Position(double _x, double _y, double _z) :
    x(_x),
    y(_y),
    z(_z)
{

}

double Position::norm()
{
    return sqrt(x*x+y*y+z*z);
}

Position Position::normed()
{
    return *this/norm();
}

QString Position::toString() const
{
    QString data = QString::number(x) + "," + QString::number(y) + "," + QString::number(z);
    return data;
}

bool Position::isValid()
{
    return (x!=0 && y!=0);
}

Position operator+(Position _lhs, const Position &_rhs)
{
    return Position(_lhs.x+_rhs.x, _lhs.y+_rhs.y, _lhs.z+_rhs.z);
}

Position operator-(Position _lhs, const Position &_rhs)
{
    return Position(_lhs.x-_rhs.x, _lhs.y-_rhs.y, _lhs.z-_rhs.z);
}

Position operator*(Position _lhs, double _rhs)
{
    return Position(_lhs.x*_rhs, _lhs.y*_rhs, _lhs.z*_rhs);
}

Position operator/(Position _lhs, double _rhs)
{
    return Position(_lhs.x/_rhs, _lhs.y/_rhs, _lhs.z/_rhs);
}
