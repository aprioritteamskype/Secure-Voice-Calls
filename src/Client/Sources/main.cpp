#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>
#include <QQmlContext>

#include "client.h"
#include "qmlclientstate.h"

void runCLient();

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<QMLClientState>("com.securevoivecaller", 1, 0, "QMLClientState",
                                               "Uncreatable type QMLClientState");
    engine.rootContext()->setContextProperty("globClientState", &QMLClientState::getInstance());


    engine.load(QUrl(QStringLiteral("qrc:/Design/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    runCLient();
//    std::thread clientThread(runCLient);
//    clientThread.join();
    return app.exec();
}

void runCLient(){
    std::string address("0.0.0.0:5000");
    secure_voice_call::Client client(
                grpc::CreateChannel(
                    address,
                    grpc::InsecureChannelCredentials()
                    )
                );

    bool isAuthorizationSuccess = false;
    client.sendAuthorizationRequest("some name", isAuthorizationSuccess);
    if (isAuthorizationSuccess){
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
    }

    std::cout << "isAuthorizationSuccess: " << isAuthorizationSuccess << std::endl;
}
