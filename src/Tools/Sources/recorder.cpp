#include <QAudioInput>
#include <QByteArray>
#include <QDebug>
#include "recorder.h"

#define SAMPLE_RATE 22050
#define CHANNELS 1
#define SAMPLE_SIZE 16
#define SAMPLE_TYPE SignedInt
#define RECORDING_BYTES 2

Recorder::Recorder(const QAudioFormat &format,const QAudioDeviceInfo &device,quint64 bufferSize):mFormat(format),mDevice(device),mBuffSize(bufferSize){
    mBuff.resize(mBuffSize);
    pBuff=mBuff.data();//get buffer adress
    open(QIODevice::WriteOnly);
}

Recorder::Recorder(quint64 buffSize):mBuffSize(buffSize){

    setDefaultFormat();  //set default mFormat
    setDefaultDevice();  //set default device
    mBuff.resize(mBuffSize);
    pBuff=mBuff.data();  //get buffer adress
    open(QIODevice::WriteOnly);
}

Recorder::~Recorder(){
    close();
}

quint64 Recorder::getBuffSize() const
{
    return mBuffSize;
}

bool Recorder::getDataFromBuff(char *data, quint64 bytes)
{
    if(mBufWritePos - mBufSendPos >= bytes){//FIXME
        memcpy(data,&pBuff[mBufSendPos%mBuffSize],bytes);
        mBufSendPos += bytes;
        return true;
    }
    return false;
}


qint64 Recorder::writeData(const char *data, qint64 len)
{
    qint64 total=0;
    while (len > total)
    {
        memcpy(&pBuff[mBufWritePos%mBuffSize],&data[total], RECORDING_BYTES); //write 2Bytes into circular buffer
        mBufWritePos+=RECORDING_BYTES; //next 16bit buffer location
        total+=RECORDING_BYTES;  //next data location
    }
    return total; //return total number of bytes received
}

void Recorder::runRecorder()
{
    mAudioInput.reset(new QAudioInput (mDevice, mFormat));
    mAudioInput.get()->start(this);
}

void Recorder::stopRecorder()
{
    mAudioInput.get()->stop();
}

qint64 Recorder::readData(char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

void Recorder::setDefaultFormat(){
    mFormat.setSampleRate(SAMPLE_RATE);
    mFormat.setChannelCount(CHANNELS);
    mFormat.setSampleSize(SAMPLE_SIZE);
    mFormat.setCodec("audio/pcm");
    mFormat.setByteOrder(QAudioFormat::LittleEndian);
    mFormat.setSampleType(QAudioFormat::SAMPLE_TYPE);
}

void Recorder::setDefaultDevice(){
    mDevice = QAudioDeviceInfo::defaultInputDevice();
    QAudioDeviceInfo info(mDevice);
    if (!info.isFormatSupported(mFormat)) {
        qDebug() << "Raw audio format not supported by backend, cannot write audio.";
        return;//check other device here
    }
}
