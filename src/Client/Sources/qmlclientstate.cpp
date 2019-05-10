#include "qmlclientstate.h"
#include <iostream>

using namespace secure_voice_call;
QMLClientState::QMLClientState(QObject *parent) : QObject(parent)
{
//    connect(this, &QMLClientState::tryAuthorizate, this, [](QString str){
//        std::cout << "str: " + str.toStdString() << std::endl;
//    });
}

void QMLClientState::setState(QMLClientState::ClientStates clientState)
{
    if (m_clientState == clientState)
        return;

    m_clientState = clientState;
    emit stateChanged(m_clientState);
}
