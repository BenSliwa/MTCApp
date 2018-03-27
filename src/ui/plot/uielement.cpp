#include "uielement.h"

UiElement::UiElement(QQuickItem *_parent) :
    QQuickPaintedItem(_parent),
    p_painter(0)
{

}

float UiElement::map(float _value, float _inMin, float _inMax, float _outMin, float _outMax)
{
    return (_value-_inMin) * (_outMax-_outMin) / (_inMax-_inMin) + _outMin;
}

void UiElement::paint(QPainter *_painter)
{
    p_painter = _painter;
    draw();
}

void UiElement::drawBox()
{
    p_painter->setPen("black");
    p_painter->drawRect(this->boundingRect());
}

QPen UiElement::createPen(const QVariantMap &_style)
{
    QPen pen;
    pen.setColor(_style["color"].toString());
    pen.setWidth(_style["width"].toFloat());

    return pen;
}


void drawTextF(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect = 0)
{
   const qreal size = 32767.0;
   QPointF corner(x, y - size);
   if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
   else if (flags & Qt::AlignRight) corner.rx() -= size;
   if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
   else if (flags & Qt::AlignTop) corner.ry() += size;
   else flags |= Qt::AlignBottom;
   QRectF rect(corner, QSizeF(size, size));
   painter.drawText(rect, flags, text, boundingRect);
}


void UiElement::drawText(const QString &_text, const QPointF &_position, const TextStyle &_style)
{
    p_painter->save();
    p_painter->translate(_position);
    p_painter->rotate(_style.rotation);



    QFont font;
    font.setFamily(_style.font);
    font.setPixelSize(_style.size);
    p_painter->setFont(font);
    p_painter->setPen(_style.color);


   // drawTextF(*p_painter, 0, 0, (Qt::AlignmentFlag)_style.verticalAlignment | (Qt::AlignmentFlag)_style.horizontalAlignment, _text);
    drawTextF(*p_painter, 0, 0, (Qt::AlignmentFlag)Qt::AlignCenter, _text);

    p_painter->restore();
}

QPointF UiElement::getPoint(const QPointF &_point)
{
    return QPointF(_point.x(), height() - _point.y());
}
