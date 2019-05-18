#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>
#include <QQmlContext>

#include "client.h"
#include "qmlclientstate.h"
#include "qmlclientsonlinemodel.h"
#include "peertopeer.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    secure_voice_call::PeerToPeer *peerToPeer = new secure_voice_call::PeerToPeer;
    secure_voice_call::PeerToPeer *peerToPeer2 = new secure_voice_call::PeerToPeer;

//    std::thread t([peerToPeer](){
//        peerToPeer->runServer();
//    });


    std::thread t2([peerToPeer2](){
        peerToPeer2->sendCallRequest("0.0.0.0:5001", "ivan");
    });

    secure_voice_call::QMLClientsOnlineModel *model = new secure_voice_call::QMLClientsOnlineModel();
    secure_voice_call::Client *client = new secure_voice_call::Client(*model);

    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<secure_voice_call::QMLClientState>("com.securevoivecaller", 1, 0, "QMLClientState",
                                               "Uncreatable type QMLClientState");
    engine.rootContext()->setContextProperty("globClientState", &secure_voice_call::QMLClientState::getInstance());
    engine.rootContext()->setContextProperty("onlineClientsModel", model);

    engine.load(QUrl(QStringLiteral("qrc:/Design/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
