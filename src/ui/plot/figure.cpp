#include "figure.h"

Figure::Figure(QQuickItem *_parent) :
    UiElement(_parent),
    m_plot(0)
{
    m_margin = {80, 80, 20, 40};
    m_plot = new Plot(this);
}



void Figure::draw()
{
    p_painter->fillRect(this->boundingRect(), "darkgray");


    if(m_plot)
    {
        m_plot->setWidth(width()-m_margin.left-m_margin.right);
        m_plot->setHeight(height()-m_margin.top-m_margin.bottom);
        m_plot->setPosition(QPointF(m_margin.left, m_margin.top));

        TextStyle style = {"Arial", 20, "black", 0};
        //drawText("40", QPointF(20, 20), style);

        //drawText("-10", QPointF(20, height() - 20), style);



        Axis *bottomAxis = m_plot->getBottomAxis();
        QVector<Tick> xTicks = bottomAxis->ticks;
        for(int x=0; x<xTicks.size(); x++)
        {
            Tick tick = xTicks.at(x);
            drawText(tick.label, QPointF(tick.value + m_margin.left, height() - m_margin.bottom/4*3), style);
        }
        drawText(bottomAxis->label, QPointF(m_margin.left + m_plot->width()/2, height() - m_margin.bottom/4), style);


        Axis *leftAxis = m_plot->getLeftAxis();
        QVector<Tick> yTicks = m_plot->getLeftAxis()->ticks;
        for(int y=0; y<yTicks.size(); y++)
        {
            Tick tick = yTicks.at(y);
            drawText(tick.label, QPointF(m_margin.left/4*3, height() - (tick.value + m_margin.bottom)), style);
        }
        style = {"Arial", 20, "black", -90};
        drawText(leftAxis->label, QPointF(m_margin.left/4, m_plot->height()/2), style);


        Axis *rightAxis = m_plot->getRightAxis();
        QVector<Tick> yTicks2 = rightAxis->ticks;
        for(int y=0; y<yTicks2.size(); y++)
        {
            style = {"Arial", 20, "black", 0};
            Tick tick = yTicks2.at(y);
            drawText(tick.label, QPointF(width() - m_margin.left/4*3, height() - (tick.value + m_margin.bottom)), style);
        }
        style = {"Arial", 20, "black", 90};
        drawText(rightAxis->label, QPointF(width() - m_margin.left/4, m_plot->height()/2), style);

    }


}

Plot* Figure::getPlot()
{
    return m_plot;
}
