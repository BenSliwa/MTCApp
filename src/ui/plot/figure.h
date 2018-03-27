#ifndef FIGURE_H
#define FIGURE_H

#include "uielement.h"
#include "plot.h"

struct Margin
{
    float left;
    float right;
    float top;
    float bottom;
};

class Figure : public UiElement
{
    Q_OBJECT
    Q_PROPERTY(Plot* plot READ getPlot)
public:
    Figure(QQuickItem *_parent = 0);

    void draw();

    Plot* getPlot();

private:
    Plot *m_plot;
    Margin m_margin;
};

#endif // FIGURE_H
