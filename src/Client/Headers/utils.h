#ifndef UTILS_H
#define UTILS_H

class QString;
class QRegExp;

namespace secure_voice_call {

    bool isValidIp(const QString &ip);
    void changeHost(QString &ip, const QString &host);
    void changePort(QString &ip, const unsigned int port);
}

#endif // UTILS_H
