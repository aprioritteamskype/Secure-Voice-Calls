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
        Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
        Q_PROPERTY(ClientStates clientState READ getState WRITE setState NOTIFY stateChanged)

        ClientStates getState() const;
        static QMLClientState& getInstance();
        QString callerName() const;
        QString authorizatedAs() const;
        QString status() const;

    public slots:
        void setState(ClientStates clientState);
        void setCallerName(QString callerName);
        void setAuthorizatedAs(QString authorizatedAs);
        void setStatus(QString status);

    signals:
        void stateChanged(ClientStates clientState);
        void callerNameChanged(QString callerName);
        void authorizatedAsChanged(QString authorizatedAs);
        void statusChanged(QString status);

    private:
        ClientStates mClientState = ClientStates::Authorization;
        QString mCallerName;
        QString mAuthorizatedAs;
        QString mStatus;
    };
}
