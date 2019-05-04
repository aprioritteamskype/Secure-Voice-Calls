#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>

#include "client.h"

void runCLient();

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/Design/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    std::thread clientThread(runCLient);
    clientThread.join();
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

        std::string response = client.sendRequest("CLIENT__ nevatest");
        std::cout << "Answer received: " << response << std::endl;
}
