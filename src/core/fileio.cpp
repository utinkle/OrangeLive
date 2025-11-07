#include "fileio.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QQmlFile>

FileIO::FileIO(QObject *parent)
    : QObject(parent)
{
}

QByteArray FileIO::read()
{
    if (mSource.isEmpty()){
        Q_EMIT error("[FileIO] source is empty");
        return QByteArray();
    }

    QFile file(QQmlFile::urlToLocalFileOrQrc(mSource));
    if (!file.open(QIODevice::ReadOnly)) {
        Q_EMIT error("[FileIO] Unable to open the file: " + file.errorString());
        return QByteArray();
    }

    QByteArray fileContent = file.readAll();
    file.close();

    return fileContent;
}

bool FileIO::write(const QByteArray& data)
{
    if (mSource.isEmpty())
        return false;

    QFile file(QQmlFile::urlToLocalFileOrQrc(mSource));
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
        return false;

    QDataStream out(&file);
    out << data;

    file.close();
    return true;
}

QUrl FileIO::source()
{
    return mSource;
}

void FileIO::setSource(const QUrl &source)
{
    if (mSource == source)
        return;

    mSource = source;
    Q_EMIT sourceChanged();
}
