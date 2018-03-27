#ifndef POSITION_H
#define POSITION_H

#include <QObject>

class Position
{
public:
    Position(double _x = 0, double _y = 0, double _z = 0);

    double norm();
    Position normed();

    QString toString() const;
    bool isValid();

public:
    double x;
    double y;
    double z;
};

Position operator+(Position _lhs, const Position &_rhs);
Position operator-(Position _lhs, const Position &_rhs);
Position operator*(Position _lhs, double _rhs);
Position operator/(Position _lhs, double _rhs);

#endif // POSITION_H
