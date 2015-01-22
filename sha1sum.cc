#include <QByteArray>
#include <QtCrypto>

QByteArray sha1sum(QByteArray data)
{
    QCA::Hash hashObj("sha1");
    hashObj.update(data);
    return hashObj.final().toByteArray();
}

QByteArray sha1sum(char *data, int size)
{
    QCA::Hash hashObj("sha1");
    hashObj.update(data, size);
    return hashObj.final().toByteArray();
}
