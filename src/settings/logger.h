#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:
    Logger();

    static Logger* getInstance();

    void init(const QString &_path);
    void log(const QString &_line);

private:
    QString m_path;
};

#endif // LOGGER_H
