#pragma once
#include <QObject>

//Singleton
namespace secure_voice_call{
    class QMLClientState : public QObject
    {
        Q_OBJECT

        QMLClientState(QObject *parent = nullptr);
        QMLClientState( const QMLClientState&) = default;
        QMLClientState& operator=( QMLClientState& ) = default;
    public:
        enum class ClientStates{
            Authorization,
            Online,
            IncomingCall,
            OutgoingCall,
            InConversation
        };
        Q_ENUM(ClientStates)
        Q_PROPERTY(QString callerName READ callerName WRITE setCallerName NOTIFY callerNameChanged)
        Q_PROPERTY(QString authorizatedAs READ authorizatedAs WRITE setAuthorizatedAs NOTIFY authorizatedAsChanged)
        Q_PROPERTY(ClientStates clientState READ getState WRITE setState NOTIFY stateChanged)

        ClientStates getState() const;
        static QMLClientState& getInstance();
        QString callerName() const;
        QString authorizatedAs() const;

    public slots:
        void setState(ClientStates clientState);
        void setCallerName(QString callerName);
        void setAuthorizatedAs(QString authorizatedAs);

    signals:
        void stateChanged(ClientStates clientState);
        void callerNameChanged(QString callerName);
        void authorizatedAsChanged(QString authorizatedAs);

    private:
        ClientStates mClientState = ClientStates::Authorization;
        QString mCallerName;
        QString mAuthorizatedAs;
    };
}
