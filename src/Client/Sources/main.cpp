#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>
#include <QQmlContext>

#include "client.h"
#include "qmlclientstate.h"
#include "qmlclientsonlinemodel.h"

void runCLient(secure_voice_call::QMLClientsOnlineModel &model);

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    secure_voice_call::QMLClientsOnlineModel model;
    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<secure_voice_call::QMLClientState>("com.securevoivecaller", 1, 0, "QMLClientState",
                                               "Uncreatable type QMLClientState");
    engine.rootContext()->setContextProperty("globClientState", &secure_voice_call::QMLClientState::getInstance());
    engine.rootContext()->setContextProperty("onlineClientsModel", &model);

    engine.load(QUrl(QStringLiteral("qrc:/Design/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    runCLient(model);
//    std::thread clientThread(runCLient);
//    clientThread.join();
    return app.exec();
}

void runCLient(secure_voice_call::QMLClientsOnlineModel &model){
    std::string address("0.0.0.0:5000");
    secure_voice_call::Client client(
                grpc::CreateChannel(
                    address,
                    grpc::InsecureChannelCredentials()
                    )
                );

    bool isAuthorizationSuccess = false;
    std::vector<std::string> clients;
    client.sendAuthorizationRequest("some name", isAuthorizationSuccess, clients);

    if (isAuthorizationSuccess){
        model.setCLients(clients);
        secure_voice_call::QMLClientState::getInstance().setState(secure_voice_call::QMLClientState::ClientStates::Online);
    }

    std::cout << "isAuthorizationSuccess: " << isAuthorizationSuccess << std::endl;
}
