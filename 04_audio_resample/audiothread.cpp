#include "audiothread.h"
#include"ffmpegs.h"
#include<QDebug>
#include<QFile>

extern "C"{
    #include<libswresample/swresample.h>
    #include<libavutil/avutil.h>
}
AudioThread::AudioThread(QObject *parent) : QThread(parent)
{
       connect(this,&AudioThread::finished,
               this,&AudioThread::deleteLater);
}
AudioThread::~AudioThread(){
    disconnect();
    requestInterruption();
    quit();
    wait();
    qDebug()<<this<<"析构";
}

void AudioThread::run(){
    //44100_s16le_2 -> 48000_f32le_1 -> 48000_s32le_1 -> 44100_s16le_2

    ResampleAudioSpec ras1;
    ras1.filename="D:/CX/44100_s16le_21.pcm";
    ras1.sampleFmt=AV_SAMPLE_FMT_S16;
    ras1.sampleRate=44100;
    ras1.chLayout=AV_CH_LAYOUT_STEREO;

    ResampleAudioSpec ras2;
    ras2.filename="D:/CX/48000_f32le_1.pcm";
    ras2.sampleFmt=AV_SAMPLE_FMT_FLT;
    ras2.sampleRate=48000;
    ras2.chLayout=AV_CH_LAYOUT_MONO;

    ResampleAudioSpec ras3;
    ras3.filename="D:/CX/48000_s32le_1.pcm";
    ras3.sampleFmt=AV_SAMPLE_FMT_S32;
    ras3.sampleRate=48000;
    ras3.chLayout=AV_CH_LAYOUT_MONO;

    ResampleAudioSpec ras4=ras1;
    ras4.filename="D:/CX/44100_s16le_2_new.pcm";

    FFmpegs::resampleAudio(ras1,ras2);
    FFmpegs::resampleAudio(ras2,ras3);
    FFmpegs::resampleAudio(ras3,ras4);
}

