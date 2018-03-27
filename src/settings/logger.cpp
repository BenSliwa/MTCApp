#include "logger.h"
#include "settings/filehandler.h"
#include "appcontroller.h"

Logger::Logger()
{

}

Logger* Logger::getInstance()
{
    static Logger instance;
    return &instance;
}

void Logger::init(const QString &_path)
{
    m_path = _path;
}

void Logger::log(const QString &_line)
{
    float time_s = AppController::getInstance()->getMeasurementTime() / 1000.0;
    FileHandler::append(QString::number(time_s) + ": " + _line, m_path);
}
