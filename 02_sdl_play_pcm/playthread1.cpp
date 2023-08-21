#include "playthread1.h"
#include<QDebug>
#include <SDL2/SDL.h>
#include<QFile>

#define FILENAME "D:/CX/44100_s16le_21.pcm"
#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 16
#define CHANNELS 2
#define SAMPLES 1024
#define BYTES_SIZE_SAMPLE  ((SAMPLE_SIZE*CHANNELS)/8)
#define BUFFER_SIZE  (SAMPLES*BYTES_SIZE_SAMPLE)
typedef struct{
    int len=0;
    int pushlen=0;
    char*data=nullptr;
}AudioBuffer;
PlayThread1::PlayThread1(QObject *parent) : QThread(parent)
{
    connect(this,&PlayThread1::finished,
            this,&PlayThread1::deleteLater);
}
PlayThread1::~PlayThread1(){
    disconnect();
    requestInterruption();
    quit();
    wait();
    qDebug()<<this<<"析构";
}
//等待音频设备回调
void pull_audio_data(void *userdata,
                     Uint8*stream,
                     int len){
    SDL_memset(stream,0,len);
    AudioBuffer*buffer=(AudioBuffer*)userdata;

    if(buffer->len<=0)return;
    buffer->pushlen=(len>buffer->len)?buffer->len:len;

    SDL_MixAudio(stream,
                 (Uint8*)buffer->data,//要填充的数据（从文件中读取的数据）
                 buffer->pushlen,
                 SDL_MIX_MAXVOLUME);  //音量，最大128
    buffer->data += buffer->pushlen;
    buffer->len -= buffer->pushlen;
}
void PlayThread1::run(){
    if(SDL_Init(SDL_INIT_AUDIO)){
        qDebug()<<"SDL_Init error"<<SDL_GetError();
        return;
    }

    SDL_AudioSpec spec;
    spec.freq=SAMPLE_RATE;
    spec.format=AUDIO_S16LSB;
    spec.channels=CHANNELS;
    spec.samples=SAMPLES;
    spec.callback=pull_audio_data;
    AudioBuffer buffer;
    spec.userdata=&buffer;

    if(SDL_OpenAudio(&spec,nullptr)){
        qDebug()<<"SDL_OpenAudio error"<<SDL_GetError();
        SDL_Quit();
        return;
    }

    QFile file(FILENAME);
    if(!file.open(QFile::ReadOnly)){
        qDebug()<<"file open error"<<FILENAME;
        SDL_CloseAudio();
        SDL_Quit();
        return;
    }
    SDL_PauseAudio(0);

Uint8 data[BUFFER_SIZE];
    while(!isInterruptionRequested()){
        if(buffer.len>0)continue;
        buffer.len=file.read((char*)data,BUFFER_SIZE);
        if(buffer.len<=0){
            int samples=buffer.pushlen/BYTES_SIZE_SAMPLE;
            int ms=samples*1000/SAMPLE_RATE;
            SDL_Delay(ms);
            qDebug()<<ms;
            break;
    }
        buffer.data=(char*)data;
  }
    file.close();
    SDL_CloseAudio();
    SDL_Quit();
}
