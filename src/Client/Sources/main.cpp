#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>
#include <QQmlContext>
#include <QDir>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "client.h"
#include "qmlclientstate.h"
#include "qmlclientsonlinemodel.h"
#include "peertopeer.h"
#include "commandArgsParser.h"
#define LOG_DIR_PASS  "Logs"
#define LOG_PASS  "Logs/ClientLog2.txt"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qRegisterMetaType<secure_voice_call::QMLClientState::ClientStates>("ClientStates");
    qRegisterMetaType<secure_voice_call::PeerToPeer::OutgoingCallStates>("OutgoingCallStates");

    if(!QDir().exists(LOG_DIR_PASS)){
        QDir().mkdir(LOG_DIR_PASS);
    }
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::info,LOG_PASS).addAppender(&consoleAppender);

    secure_voice_call::CommandArgsParser parser(argc, argv);
    LOG(plog::info) << "[CommandArgsParser]parser.serverIp() " << parser.serverIp();
    LOG(plog::info) << "[CommandArgsParser]parser.serverPort() " << parser.serverPort();
    LOG(plog::info) << "[CommandArgsParser]parser.peerToPeerClientSidePort() " << parser.peerToPeerClientSidePort();
    LOG(plog::info) << "[CommandArgsParser]parser.peerToPeerServerSidePort() " << parser.peerToPeerServerSidePort();
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
