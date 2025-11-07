#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>
#include <QUrl>

class FileIO : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    explicit FileIO(QObject *parent = nullptr);

    Q_INVOKABLE QByteArray read();
    Q_INVOKABLE bool write(const QByteArray &data);

    QUrl source();
    void setSource(const QUrl &source);;

Q_SIGNALS:
    void sourceChanged();
    void error(const QString& msg);

private:
    QUrl mSource;
};

#endif // FILEIO_H
