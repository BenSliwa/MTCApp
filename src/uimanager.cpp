#include "uimanager.h"
#include "appcontroller.h"

UiManager::UiManager(QObject *_parent) : QObject(_parent)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onUpdateTimeout()));
}

void UiManager::addQtObject(const QString &_name, QObject *_object)
{
    m_qml.engine()->rootContext()->setContextProperty(_name, _object);
}

void UiManager::load(const QString &_file)
{
    qmlRegisterType<Plot>("cni.MTC", 1, 0, "Plot");
    qmlRegisterType<Figure>("cni.MTC", 1, 0, "Figure");

    m_qml.setResizeMode(QQuickView::SizeRootObjectToView);
    m_qml.setSource(QUrl(_file));
    m_qml.show();

    p_figure = m_qml.rootObject()->findChild<Figure*>("fig");
    p_plot = p_figure->getPlot();

    m_timer.start(1000.0/25.0);

    AppController *app = AppController::getInstance();
    connect(app, SIGNAL(appTransmitted(float,QString)), this, SLOT(onAppTransmission(float,QString)));
    connect(app, SIGNAL(updateContext(QVariantMap)), this, SLOT(onContextUpdate(QVariantMap)));

}

void UiManager::onUpdateTimeout()
{
    float time_s = AppController::getInstance()->getPassedTime() / 1000.0;
    p_plot->setTimeWindow(time_s);
}

void UiManager::onContextUpdate(const QVariantMap &_context)
{
    float time_s = AppController::getInstance()->getPassedTime() / 1000.0;
    if(_context.contains("snr"))
        p_plot->add(time_s, _context["snr"].toFloat());

    if(_context.contains("rsrp"))
        p_plot->add(time_s, _context["rsrp"].toFloat(), true);
}

void UiManager::onAppTransmission(float _time_s, const QString &_name)
{
    p_plot->addMarker(_time_s, _name);
}
