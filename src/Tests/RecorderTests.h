#include <QTest>

class RecorderTest : public QObject {
    Q_OBJECT
private slots:
    void testWriteData();
    void testBufferMaxWritingPos();
    void testBufferSize();
    void testBytesToGetOverflow();
};


