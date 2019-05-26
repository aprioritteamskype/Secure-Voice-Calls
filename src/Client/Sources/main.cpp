#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>
#include <QQmlContext>

#include "client.h"
#include "qmlclientstate.h"
#include "qmlclientsonlinemodel.h"
#include "peertopeer.h"
#include "commandArgsParser.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qRegisterMetaType<secure_voice_call::QMLClientState::ClientStates>("ClientStates");
    qRegisterMetaType<secure_voice_call::PeerToPeer::OutgoingCallStates>("OutgoingCallStates");

    secure_voice_call::CommandArgsParser parser(argc, argv);
    std::cout << "parser.serverIp() " << parser.serverIp() << std::endl;
    std::cout << "parser.serverPort() " << parser.serverPort() << std::endl;
    std::cout << "parser.peerToPeerClientSidePort() " << parser.peerToPeerClientSidePort() << std::endl;
    std::cout << "parser.peerToPeerServerSidePort() " << parser.peerToPeerServerSidePort() << std::endl;
    secure_voice_call::QMLClientsOnlineModel *model = new secure_voice_call::QMLClientsOnlineModel();
    secure_voice_call::Client *client = new secure_voice_call::Client(*model,
                                                                      parser.peerToPeerClientSidePort(),
                                                                      parser.peerToPeerServerSidePort(),
                                                                      parser.serverIp());

    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<secure_voice_call::QMLClientState>("com.securevoicecaller", 1, 0, "QMLClientState",
                                               "Uncreatable type QMLClientState");
    engine.rootContext()->setContextProperty("globClientState", &secure_voice_call::QMLClientState::getInstance());
    engine.rootContext()->setContextProperty("onlineClientsModel", model);
    engine.rootContext()->setContextProperty("client", client);

    engine.load(QUrl(QStringLiteral("qrc:/Design/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
