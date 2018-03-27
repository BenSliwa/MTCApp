#include "plot.h"
#include "figure.h"

Plot::Plot(QQuickItem *_parent) :
    UiElement(_parent)
{
    m_bottomAxis.min = 0;
    m_bottomAxis.max = 100;
    m_bottomAxis.label = "Time [ms]";

    m_leftAxis.min = -10;
    m_leftAxis.max = 40;
    m_leftAxis.label = "SNR [dB]";

    m_rightAxis.min = -140;
    m_rightAxis.max = -40;
    m_rightAxis.label = "RSRP [dB]";

    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(onWindowChanged()));

    p_figure = qobject_cast<Figure*>(_parent);

}

void Plot::add(float _x, float _y, bool _right)
{
    if(!_right)
        m_data << QPointF(_x, _y);
    else
        m_rightData << QPointF(_x, _y);

    update();
}

void Plot::addMarker(float _x, const QString &_type)
{
    Marker marker;
    marker.x = _x;
    marker.type = _type;

    m_marker << marker;

    update();
}

QPointF Plot::getPlotPoint(const QPointF &_point)
{
    float x = map(_point.x(), m_bottomAxis.min, m_bottomAxis.max, 0, width());
    float y = map(_point.y(), m_leftAxis.min, m_leftAxis.max, height(), 0);
    return QPointF(x, y);
}

void Plot::drawPlotLine(const QPointF &_from, const QPointF &_to, const QVariantMap &_style)
{
    p_painter->setPen(createPen(_style));
    p_painter->drawLine(getPlotPoint(_from), getPlotPoint(_to));
}

QString Plot::getTitle()
{
    return m_title;
}

Axis* Plot::getLeftAxis()
{
    return &m_leftAxis;
}

Axis* Plot::getRightAxis()
{
    return &m_rightAxis;
}

Axis* Plot::getBottomAxis()
{
    return &m_bottomAxis;
}

void Plot::draw()
{
    float xRange = m_bottomAxis.max - m_bottomAxis.min;
    float xMin = m_time_s - xRange;

    p_painter->fillRect(this->boundingRect(), "white");
    UiElement::drawBox();

    //
    QVariantMap lineStyle;
    lineStyle["color"] = "darkgray";
    lineStyle["width"] = 2;
    drawPlotLine(QPointF(m_bottomAxis.min, 0), QPointF(QPointF(m_bottomAxis.max, 0)), lineStyle);
    drawPlotLine(QPointF(m_bottomAxis.min, 10), QPointF(QPointF(m_bottomAxis.max, 10)), lineStyle);

    //

    QVariantMap plotStyle;
    plotStyle["color"] = "blue";
    plotStyle["width"] = 2;

    QVector<QPointF> polyline;
    for(int i=0; i<m_data.size(); i++)
    {
        QPointF value = m_data.at(i);
        float x = map(value.x() - xMin, m_bottomAxis.min, m_bottomAxis.max, 0, width());
        float y = map(value.y(), m_leftAxis.min, m_leftAxis.max, height(), 0);

        polyline << QPointF(x, y);
    }
    p_painter->setPen(createPen(plotStyle));
    p_painter->drawPolyline(polyline);


    plotStyle["color"] = "red";
    polyline.clear();
    for(int i=0; i<m_rightData.size(); i++)
    {
        QPointF value = m_rightData.at(i);
        float x = map(value.x() - xMin, m_bottomAxis.min, m_bottomAxis.max, 0, width());
        float y = map(value.y(), m_rightAxis.min, m_rightAxis.max, height(), 0);

        polyline << QPointF(x, y);
    }
    p_painter->setPen(createPen(plotStyle));
    p_painter->drawPolyline(polyline);



    //


    for(int i=0; i<m_marker.size(); i++)
    {
        Marker marker = m_marker.at(i);
        QVariantMap markerStyle;
        markerStyle["color"] = "red";
        markerStyle["width"] = 2;

        if(marker.type=="periodic")
            markerStyle["color"] = "black";


        float x = marker.x - xMin;
        drawPlotLine(QPointF(x, m_leftAxis.min), QPointF(QPointF(x, m_leftAxis.max)), markerStyle);
    }

    onWindowChanged();
}

void Plot::setTimeWindow(float _time_s)
{
    m_time_s = _time_s;
    float xRange = m_bottomAxis.max - m_bottomAxis.min;
    for(int i=0; i<m_data.size(); )
    {
        float x = m_data.at(i).x();
        if(m_time_s-x>xRange)
            m_data.removeAt(i);
        else
            i++;
    }

    for(int i=0; i<m_rightData.size(); )
    {
        float x = m_rightData.at(i).x();
        if(m_time_s-x>xRange)
            m_rightData.removeAt(i);
        else
            i++;
    }

    //
    for(int i=0; i<m_marker.size(); )
    {
        float x = m_marker.at(i).x;
        if(m_time_s-x>xRange)
            m_marker.removeAt(i);
        else
            i++;
    }
}


void Plot::onWindowChanged()
{
    m_leftAxis.ticks.clear();
    int ticks = 6;
    for(int y=0; y<ticks; y++)
    {
        double step = 10;
        double value = m_leftAxis.min + y * step;
        Tick tick;
        tick.label = QString::number(value);
        tick.value = height()/(ticks-1) * y;

        m_leftAxis.ticks << tick;
    }

    m_rightAxis.ticks.clear();
    ticks = 6;
    for(int y=0; y<ticks; y++)
    {
        double step = 20;
        double value = m_rightAxis.min + y * step;
        Tick tick;
        tick.label = QString::number(value);
        tick.value = height()/(ticks-1) * y;

        m_rightAxis.ticks << tick;
    }

    ticks = 11;
    m_bottomAxis.ticks.clear();
    for(int x=0; x<ticks; x++)
    {
        double step = 10;
        double value = m_bottomAxis.min + x * step;

        Tick tick;
        tick.label = QString::number(value);
        tick.value = width()/(ticks-1) * x;

        m_bottomAxis.ticks << tick;
    }



    p_figure->update();
}
