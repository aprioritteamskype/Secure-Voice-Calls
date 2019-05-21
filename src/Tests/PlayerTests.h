#include <QTest>

class PlayerTest : public QObject {
    Q_OBJECT
private slots:
    void testReadData();
    void testBufferMaxReadingPos();
    void testBufferSize();
    void testBytesToPushOverflow();
};


