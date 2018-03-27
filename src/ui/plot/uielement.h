#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <QQuickPaintedItem>
#include <QPainter>
#include <QDebug>

struct TextStyle
{
    QString font;
    double size;
    QString color;
    double rotation;
};

class UiElement : public QQuickPaintedItem
{
    Q_OBJECT
public:
    UiElement(QQuickItem *_parent = 0);

    float map(float _value, float _inMin, float _inMax, float _outMin, float _outMax);

    void paint(QPainter *_painter);
    void drawBox();

    QPen createPen(const QVariantMap &_style);

    void drawText(const QString &_text, const QPointF &_position, const TextStyle &_style);

    virtual void draw() = 0;

    QPointF getPoint(const QPointF &_point);



protected:
    QPainter *p_painter;
};

#endif // UIELEMENT_H
