#include "qmlclientstate.h"

using namespace secure_voice_call;
QMLClientState::QMLClientState(QObject *parent) : QObject(parent)
{
}

QMLClientState::ClientStates QMLClientState::getState() const { return mClientState; }

QMLClientState &QMLClientState::getInstance() {
    static QMLClientState  instance;
    return instance;
}

QString QMLClientState::callerName() const
{
    return mCallerName;
}

QString QMLClientState::authorizatedAs() const
{
    return mAuthorizatedAs;
}

QString QMLClientState::status() const
{
    return mStatus;
}

void QMLClientState::setState(QMLClientState::ClientStates clientState)
{
    if (mClientState == clientState)
        return;

    mClientState = clientState;
    emit stateChanged(mClientState);
}

void QMLClientState::setCallerName(QString callerName)
{
    if (mCallerName == callerName)
        return;

    mCallerName = callerName;
    emit callerNameChanged(mCallerName);
}

void QMLClientState::setAuthorizatedAs(QString authorizatedAs)
{
    if (mAuthorizatedAs == authorizatedAs)
        return;

    mAuthorizatedAs = authorizatedAs;
    emit authorizatedAsChanged(mAuthorizatedAs);
}

void QMLClientState::setStatus(QString status)
{
    mStatus = status;
    emit statusChanged(mStatus);
}
