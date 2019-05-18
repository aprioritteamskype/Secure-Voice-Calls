#pragma once

class QString;
class QRegExp;

namespace secure_voice_call {

    bool isValidIp(const QString &ip);
    void changeHost(QString &ip, const QString &host);
    void changePort(QString &ip, unsigned int port);
}
