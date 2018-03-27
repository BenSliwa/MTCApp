#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QDebug>
#include <QStringList>

class FileHandler : public QObject
{
    Q_OBJECT
public:
    FileHandler(QObject *_parent = 0);

    static QStringList read(const QString &_path);
    static void write(const QString &_data, const QString &_path);
    static void append(const QString &_data, const QString &_path);
};

#endif // FILEHANDLER_H
