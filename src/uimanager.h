#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QtGui>
#include <QtQuick>

#include "ui/plot/plot.h"
#include "ui/plot/figure.h"

class UiManager : public QObject
{
    Q_OBJECT
public:
    UiManager(QObject *_parent = 0);

    void addQtObject(const QString &_name, QObject *_object);
    void load(const QString &_file);

private slots:
    void onUpdateTimeout();
    void onContextUpdate(const QVariantMap &_context);
    void onAppTransmission(float _time_s, const QString &_name);

private:
    QQuickView m_qml;

    Figure *p_figure;
    Plot *p_plot;
    QTimer m_timer;
};

#endif // UIMANAGER_H
