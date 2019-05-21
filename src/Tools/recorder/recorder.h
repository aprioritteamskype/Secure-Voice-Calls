#ifndef RECORDER_H
#define RECORDER_H

#include <QAudioInput>
#include <QByteArray>
#include <memory>
#define DEFAULT_BUFF_SIZE 0x10000


class Recorder : public QIODevice
{
    Q_OBJECT

public:
    Recorder(quint64 buffSize = DEFAULT_BUFF_SIZE);//setting default format and device
    Recorder(const QAudioFormat &format,const QAudioDeviceInfo &device,quint64 bufferSize);
    ~Recorder();

    quint64 getBuffSize()const;
    bool   getDataFromBuff( char* data,quint64 bytes);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

public slots:
    void runRecorder();
    void stopRecorder();

private:
    void setDefaultFormat();
    void setDefaultDevice();

private:
    const quint64 mBuffSize;
    QByteArray mBuff;//buffer
    char *pBuff;//ptr to buffer
    quint64 mBufWritePos = 0;//position of writting to circular buffer
    quint64 mBufSendPos = 0;
    QAudioFormat mFormat;
    QAudioDeviceInfo mDevice;
    std::unique_ptr<QAudioInput> mAudioInput;

signals:
    void update();
};

#endif // AUDIOINPUT_H
