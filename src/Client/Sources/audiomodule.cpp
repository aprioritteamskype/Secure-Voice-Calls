#include "audiomodule.h"
#include <QThread>
secure_voice_call::AudioModule::AudioModule()
    : QObject (nullptr)
{
    mReader.reset(new AudioReader());
    mWriter.reset(new AudioWriter());

    mRecorder.reset(new Recorder());
    connect(this,SIGNAL(startRecordingAudio()),mRecorder.get(),SLOT(runRecorder()));
    connect(this,SIGNAL(stopRecordingAudio()),mRecorder.get(),SLOT(stopRecorder()));
    mPlayer.reset(new Player());
    connect(this,SIGNAL(startPlayingAudio()),mPlayer.get(),SLOT(runPlayer()));
    connect(this,SIGNAL(stopPlayingAudio()),mPlayer.get(),SLOT(stopPlayer()));

    recThread = new QThread(this);
    playThread = new QThread(this);
    connect(this,SIGNAL(destroyed()),recThread,SLOT(quit()));
    connect(this,SIGNAL(destroyed()),playThread,SLOT(quit()));
    mPlayer->moveToThread(playThread);
    mRecorder->moveToThread(recThread);
    playThread->start();
    recThread->start();
}

void secure_voice_call::AudioModule::readVoice(secure_voice_call::CallRequest &request) const
{
    mReader->readAudio(request,mPlayer);
}

void secure_voice_call::AudioModule::readVoice(secure_voice_call::CallResponse &response) const
{
    mReader->readAudio(response,mPlayer);
}

bool secure_voice_call::AudioModule::writeVoice(secure_voice_call::CallRequest &request) const
{
    return mWriter->writeAudio(request,mRecorder);
}

bool secure_voice_call::AudioModule::writeVoice(secure_voice_call::CallResponse &response) const
{
    return mWriter->writeAudio(response,mRecorder);
}


