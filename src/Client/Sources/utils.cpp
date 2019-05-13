#include "utils.h"
#include <QString>
#include <QRegExp>

//cap(1) - ipv4: or ipv6:
//cap(2) - host
//cap(3) - port

bool secure_voice_call::isValidIp(const QString &ip)
{
    QRegExp ipRegExp(QString("^(ipv[46]:)?(%1\\.%1\\.%1\\.%1):(%2)$")
            .arg("\\d{1,3}")
            .arg("\\d{1,5}"));
    return ipRegExp.exactMatch(ip);
}

void secure_voice_call::changeHost(QString &ip, const QString &host)
{
    QRegExp ipRegExp(QString("^(ipv[46]:)?(%1\\.%1\\.%1\\.%1):(%2)$")
            .arg("\\d{1,3}")
            .arg("\\d{1,5}"));
    ipRegExp.indexIn(ip);
    QString tmp = QString("%1:%2")
            .arg(host)
            .arg(ipRegExp.cap(3));
    ip = std::move(tmp);
}

void secure_voice_call::changePort(QString &ip, const unsigned int port)
{
    QRegExp ipRegExp(QString("^(ipv[46]:)?(%1\\.%1\\.%1\\.%1):(%2)$")
            .arg("\\d{1,3}")
            .arg("\\d{1,5}"));
    ipRegExp.indexIn(ip);
    QString tmp = QString("%1:%2")
            .arg(ipRegExp.cap(2))
            .arg(port);
    ip = std::move(tmp);
}
