#include <QByteArray>
#include <QDebug>
#include <QAudioOutput>
#include "player.h"

#define SAMPLE_RATE 22050
#define CHANNELS 1
#define SAMPLE_SIZE 16
#define SAMPLE_TYPE SignedInt
#define PLAYING_BYTES 2

Player::Player(const QAudioFormat &format,const QAudioDeviceInfo &device,qint64 bufferSize):mFormat(format),
    mDevice(device),mBuffSize(bufferSize>PLAYING_BYTES?bufferSize:PLAYING_BYTES){
    mBuff.resize(mBuffSize);
    pBuff=mBuff.data();//get buffer adress
    open(QIODevice::ReadOnly);
}

Player::Player(quint64 bufferSize):mBuffSize(bufferSize>PLAYING_BYTES?bufferSize:PLAYING_BYTES){
    //set default mFormat
    setDefaultFormat();

    //set default device
    setDefaultDevice();

    //mAudiOutput.reset(new QAudioOutput (mDevice, mFormat));

    mBuff.resize(mBuffSize);//set 64 kb Buffer
    pBuff=mBuff.data();//get buffer adress
    open(QIODevice::ReadOnly);
}

Player::~Player(){
    close();
}

void Player::pushDataToBuff(const char *data, quint64 bytes)
{
    quint64 ramainingSize = mBuffSize - mBufReadyToRead%mBuffSize;
    if(ramainingSize < bytes){//FIXME
        pushDataToBuff(data,ramainingSize);
        pushDataToBuff(data+ramainingSize,bytes-ramainingSize);
    }
    else
    {
        memcpy(&pBuff[mBufReadyToRead%mBuffSize],data, bytes); //write data to buffer
        mBufReadyToRead+=bytes; //next buffer location
    }
}

quint64 Player::getBuffSize() const
{
    return mBuffSize;
}

qint64 Player::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

void Player::runPlayer()
{
    mAudiOutput.reset(new QAudioOutput (mDevice, mFormat));
    mAudiOutput.get()->start(this);
}

void Player::stopPlayer()
{
    mAudiOutput.get()->stop();
}

qint64 Player::readData(char *data, qint64 len)
{
    qint64 total = 0;
    while (len > total  && mBufReadyToRead > mBufReadPos)//write and synchronise buffers
    {
        memcpy(&data[total],&pBuff[mBufReadPos%mBuffSize],PLAYING_BYTES);    //copy 2 Bytes
        mBufReadPos+=PLAYING_BYTES; //point to next buffer 16 bit location
        total+=PLAYING_BYTES;
    }
    return total;  //the reset interval
}

void Player::setDefaultFormat(){
    mFormat.setSampleRate(SAMPLE_RATE);
    mFormat.setChannelCount(CHANNELS);
    mFormat.setSampleSize(SAMPLE_SIZE);
    mFormat.setCodec("audio/pcm");
    mFormat.setByteOrder(QAudioFormat::LittleEndian);
    mFormat.setSampleType(QAudioFormat::SAMPLE_TYPE);
}

void Player::setDefaultDevice(){
    mDevice = QAudioDeviceInfo::defaultOutputDevice();
    QAudioDeviceInfo info(mDevice);
    if (!info.isFormatSupported(mFormat)) {
        qDebug() << "Raw audio format not supported by backend, cannot write audio.";
        return;//check other devices here //FIXME
    }
}

