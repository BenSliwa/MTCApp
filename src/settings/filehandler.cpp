#include "filehandler.h"
#include <QFile>
#include <QTextStream>


FileHandler::FileHandler(QObject *_parent) : QObject(_parent)
{

}

QStringList FileHandler::read(const QString &_path)
{
    QStringList data;
    QFile file(_path);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        while(!in.atEnd())
            data << in.readLine();
        file.close();
    }

    return data;
}

void FileHandler::write(const QString &_data, const QString &_path)
{
    QFile file(_path);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << _data << "\n";

        file.close();
    }
    //else
    //    qDebug() << "failed to write" << _path;
}

void FileHandler::append(const QString &_data, const QString &_path)
{
    QFile file(_path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << _data << "\n";

        file.close();
    }
    //else
    //    qDebug() << "failed to append" << _path;
}
