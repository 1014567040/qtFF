#include "playthread1.h"
#include<QDebug>
#include <SDL2/SDL.h>
#include<QFile>

#define FILENAME "D:/CX/44100_s16le_21.pcm"
#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 16
#define CHANNELS 2
#define SAMPLES 1024    //音频缓冲区的样本数量
#define BYTES_SIZE_SAMPLE  ((SAMPLE_SIZE*CHANNELS)/8)   //每个样本占多少个字节
#define BUFFER_SIZE  (SAMPLES*BYTES_SIZE_SAMPLE)  //文件缓冲区的大小
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
    disconnect();//断开所有的连接
    requestInterruption();
    quit();
    wait();
    qDebug()<<this<<"析构";
}
//等待音频设备回调
void pull_audio_data(void *userdata,
                     //需要往stream中填充pcm数据,stream指向音频缓冲区
                     Uint8*stream,
                     //希望填充的大小(samples*format*channels/8)
                     int len){
    //清空stream
    SDL_memset(stream,0,len);
    //取出AudioBuffer
    AudioBuffer*buffer=(AudioBuffer*)userdata;

    //文件数据还没准备好
    if(buffer->len<=0)return;
    //取len和buffLen的最小值
    buffer->pushlen=(len>buffer->len)?buffer->len:len;
    //填充数据
    SDL_MixAudio(stream,
                 (Uint8*)buffer->data,//要填充的数据（从文件中读取的数据）
                 buffer->pushlen,
                 SDL_MIX_MAXVOLUME);  //音量，最大128
    buffer->data += buffer->pushlen;
    buffer->len -= buffer->pushlen;
}
void PlayThread1::run(){
    //初始化子系统,0为成功，负数失败
    if(SDL_Init(SDL_INIT_AUDIO)){
        qDebug()<<"SDL_Init error"<<SDL_GetError();
        return;
    }

    //音频参数
    SDL_AudioSpec spec;
    //采用率
    spec.freq=SAMPLE_RATE;
    //采用格式
    spec.format=AUDIO_S16LSB;
    //声道数
    spec.channels=CHANNELS;
    //音频缓冲区样本数量(这个值必须是2的幂)
    spec.samples=SAMPLES;
    //回调
    spec.callback=pull_audio_data;
    //传递给回调函数的参数
    AudioBuffer buffer;
    spec.userdata=&buffer;
    //打开设备
    if(SDL_OpenAudio(&spec,nullptr)){
        qDebug()<<"SDL_OpenAudio error"<<SDL_GetError();
        //清除子系统
        SDL_Quit();
        return;
    }

    //打开文件
    QFile file(FILENAME);
    if(!file.open(QFile::ReadOnly)){
        qDebug()<<"file open error"<<FILENAME;
        //关闭设备
        SDL_CloseAudio();
        //关闭子系统
        SDL_Quit();
        return;
    }

    //开始播放
    SDL_PauseAudio(0);

//存放文件中读取数据
Uint8 data[BUFFER_SIZE];
    while(!isInterruptionRequested()){   //线程不退出
        //只要从文件中读取数据还没有填充完毕，就跳过
        if(buffer.len>0)continue;
        buffer.len=file.read((char*)data,BUFFER_SIZE);
        //文件数据已经读取完毕
        if(buffer.len<=0){
            // 剩余的样本数量
            int samples=buffer.pushlen/BYTES_SIZE_SAMPLE;
            int ms=samples*1000/SAMPLE_RATE;
            SDL_Delay(ms);
            qDebug()<<ms;
            break;
    }
        // 读取到了文件数据
        buffer.data=(char*)data;
  }
    //关闭文件
    file.close();
    //关闭设备
    SDL_CloseAudio();
    //清除所有的子系统
    SDL_Quit();
}
