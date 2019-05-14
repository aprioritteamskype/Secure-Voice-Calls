#ifndef PLAYER_H
#define PLAYER_H
#include <QByteArray>
#include <memory>
#include <QAudioOutput>
#define DEFAULT_BUFF_SIZE 0x10000

class Player : public QIODevice
{
    Q_OBJECT

public:
    Player(quint64 bufferSize = DEFAULT_BUFF_SIZE);//set default format
    Player(const QAudioFormat &format,const QAudioDeviceInfo &device,qint64 bufferSize);
    ~Player();

    quint64 get_mBuffReadyToRead()const;
    char* getBuff()const;
    quint64 getBuffSize()const;
    void   raise_mBufReadyToRead(quint64 bytesInPacket);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
public slots:
    void runPlayer();
    void stopPlayer();

private:
    void setDefaultFormat();
    void setDefaultDevice();

private:
    const quint64 mBuffSize;
    QByteArray mBuff;//buffer
    char *pbuff;//ptr to buffer
    quint64 mBufReadyToRead = 0;//position where data is ready to read
    quint64 mBufReadPos = 0;//position of reading from circular buffer
    QAudioFormat mFormat;
    QAudioDeviceInfo mDevice;
    std::unique_ptr<QAudioOutput> mAudiOutput;

signals:
    void update();
};

#endif // PLAYER_H

