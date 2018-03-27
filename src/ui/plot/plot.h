#ifndef PLOT_H
#define PLOT_H

#include "uielement.h"
#include "marker.h"
#include "axis.h"


class Figure;

class Plot : public UiElement
{
    Q_OBJECT
public:
    Plot(QQuickItem *_parent = 0);

    Q_INVOKABLE void add(float _x, float _y, bool _right=false);
    Q_INVOKABLE void addMarker(float _x, const QString &_type);
    void setTimeWindow(float _time_s);


    QPointF getPlotPoint(const QPointF &_point);
    void drawPlotLine(const QPointF &_from, const QPointF &_to, const QVariantMap &_style);

    //

    QString getTitle();


    Axis* getLeftAxis();
    Axis* getRightAxis();
    Axis* getBottomAxis();

private:
    void draw();

private slots:
    void onWindowChanged();

private:
    Figure *p_figure;
    QList<QPointF> m_data;
    QList<QPointF> m_rightData;
    QVector<Marker> m_marker;

    QString m_title;

    float m_time_s;
    Axis m_leftAxis;
    Axis m_rightAxis;
    Axis m_bottomAxis;
};

#endif // PLOT_H
