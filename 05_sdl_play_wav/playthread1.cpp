#include "playthread1.h"
#include<QDebug>
#include <SDL2/SDL.h>
#include<QFile>
#define FILENAME "D:/CX/out.wav"
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

void pull_audio_data(void *userdata,
                     Uint8*stream,
                     int len){
    SDL_memset(stream,0,len);
    AudioBuffer*buffer=(AudioBuffer*)userdata;

    if(buffer->len<=0)return;
    buffer->pushlen=(len>buffer->len)?buffer->len:len;
    SDL_MixAudio(stream,
                 (Uint8*)buffer->data,
                 buffer->pushlen,
                 SDL_MIX_MAXVOLUME);
    buffer->data += buffer->pushlen;
    buffer->len -= buffer->pushlen;
}
void PlayThread1::run(){
    if(SDL_Init(SDL_INIT_AUDIO)){
        qDebug()<<"SDL_Init error"<<SDL_GetError();
        return;
    }
    SDL_AudioSpec spec;
    Uint8 *data=nullptr;
    Uint32 len;
    if(!SDL_LoadWAV(FILENAME,&spec,&data,&len)){
        qDebug()<<"SDL_LoadWAV error"<<SDL_GetError();
        SDL_Quit();
        return;
    }
    spec.samples=1024;
    spec.callback=pull_audio_data;
    AudioBuffer buffer;
    buffer.data=(char*)data;
    buffer.len=len;
    spec.userdata=&buffer;

    if(SDL_OpenAudio(&spec,nullptr)){
        qDebug()<<"SDL_OpenAudio error"<<SDL_GetError();
        SDL_Quit();
        return;
    }
    SDL_PauseAudio(0);
    int sampleSize=SDL_AUDIO_BITSIZE(spec.format);
    int bytesPerSample=(sampleSize*spec.channels)/8;

    while(!isInterruptionRequested()){
        if(buffer.len>0)continue;

        if(buffer.len<=0){
            int samples=buffer.pushlen/bytesPerSample;
            int ms=samples*1000/spec.freq;
            SDL_Delay(ms);
            qDebug()<<ms;
            break;
    }
}
    SDL_FreeWAV(data);
    SDL_CloseAudio();
    SDL_Quit();
}
