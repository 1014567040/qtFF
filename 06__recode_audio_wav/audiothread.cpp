#include "audiothread.h"
#include<QDebug>
#include<QFile>
#include<QDateTime>
#include"ffmpegs.h"
extern "C"{
    #include<libavdevice/avdevice.h>
    #include<libavformat/avformat.h>
    #include<libavutil/avutil.h>
}
AudioThread::AudioThread(QObject *parent) : QThread(parent)
{
       connect(this,&AudioThread::finished,
               this,&AudioThread::deleteLater);
}
AudioThread::~AudioThread(){
    requestInterruption();
    quit();
    wait();
    qDebug()<<this<<"析构";
}
void AudioThread::run(){
   const char *fmtname="dshow";
   const AVInputFormat *fmt =av_find_input_format(fmtname);
   if(!fmt){
        qDebug()<<"获取输入格式失败"<<fmtname;
        return;
   }
    AVFormatContext *ctx=nullptr;
    const char *deviceName="audio=麦克风 (Realtek(R) Audio)";

    int ret=avformat_open_input(&ctx,deviceName,fmt,nullptr);
    if(ret<0){
        char errbuf[1024];
        av_strerror(ret,errbuf,sizeof(errbuf));
        qDebug()<<"打开设备失败"<<errbuf;
        return;
    }

    QString filename="D:/CX/";
    filename+=QDateTime::currentDateTime().toString("MM_dd_HH_mm_ss");
    filename+=".wav";
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        qDebug()<<"打开文件失败"<<filename;
        avformat_close_input(&ctx);
        return;
    }
    AVStream *stream=ctx->streams[0];
    AVCodecParameters *params=stream->codecpar;

    WAVHeader header;
    header.sampleRate=params->sample_rate;
    header.bitsPerSample=av_get_bits_per_sample(params->codec_id);
    header.numChannels=params->channels;
    if(params->codec_id>=AV_CODEC_ID_PCM_F32BE){
        header.audioFormat=AUDIO_FORMAT_FLOAT;
    }
    header.blockAlign= header.bitsPerSample*header.numChannels>>3;
    header.byteRate = header.sampleRate*header.blockAlign;
    file.write((char*)&header,sizeof(WAVHeader));

    AVPacket pkt;
        while(!isInterruptionRequested()){
            ret=av_read_frame(ctx,&pkt);
            if(ret==0){
                file.write((const char*)pkt.data,pkt.size);

                header.dataChunkDataSize += pkt.size;
                unsigned long long ms=1000.0*header.dataChunkDataSize/header.byteRate;
                emit timeChanged(ms);

            }else if(ret==AVERROR(EAGAIN)){
                continue;
            }
            else{
                char errbuf[1024];
                av_strerror(ret,errbuf,sizeof(errbuf));
                qDebug()<<"av_read_frame error"<<errbuf<<ret;
                break;
            }
        }
        qDebug()<<file.size()<<header.dataChunkDataSize;

    file.seek(sizeof(WAVHeader)-sizeof(header.dataChunkDataSize));
    file.write((char*)&header.dataChunkDataSize,sizeof(header.dataChunkDataSize));

    header.riffChunkDataSize=file.size()
                             -sizeof(header.riffChunkId)
                             -sizeof(header.riffChunkDataSize);
    file.seek(sizeof(header.riffChunkId));
    file.write((char*)&header.riffChunkDataSize,sizeof(header.riffChunkDataSize));

    file.close();
    avformat_close_input(&ctx);

    qDebug()<<pkt.size<<"正常退出";
}
