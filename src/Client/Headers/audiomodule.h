#pragma once
#include "player.h"
#include "recorder.h"
#include "client-server.grpc.pb.h"
#include "audiowriter.h"
#include "audioreader.h"

namespace secure_voice_call{
    class AudioModule:public QObject{
        Q_OBJECT
    public:
        AudioModule();
        void readVoice(CallRequest &request) const;
        void readVoice(CallResponse &response) const;
        bool writeVoice(CallRequest &request) const;
        bool writeVoice(CallResponse &response) const;
    signals:
        void startRecordingAudio() const;
        void stopRecordingAudio() const;
        void startPlayingAudio() const;
        void stopPlayingAudio() const;
    private:

        std::unique_ptr<AudioWriter> mWriter;
        std::unique_ptr<AudioReader> mReader;
        std::shared_ptr<Recorder> mRecorder;
        QThread *recThread;
        std::shared_ptr<Player> mPlayer;
        QThread *playThread;
    };
}
