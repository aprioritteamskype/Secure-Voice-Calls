#include "qmlclientstate.h"

QMLClientState::QMLClientState(QObject *parent) : QObject(parent)
{

}

void QMLClientState::setState(QMLClientState::ClientStates clientState)
{
    if (m_clientState == clientState)
        return;

    m_clientState = clientState;
    emit stateChanged(m_clientState);
}
