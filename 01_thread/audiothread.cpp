#include "audiothread.h"
#include<QDebug>
#include<QFile>
#include<QDateTime>
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

    const char *filename="D:/CX/out2.pcm";

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        qDebug()<<"打开文件失败"<<filename;
        avformat_close_input(&ctx);
        return;
    }

    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        ret = av_read_frame(ctx, pkt);
        if (ret == 0) {
            file.write((const char *) pkt->data, pkt->size);
        } else if (ret == AVERROR(EAGAIN)) {
            continue;
        } else {
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof (errbuf));
            qDebug() << "av_read_frame error" << errbuf << ret;
            break;
        }
        av_packet_unref(pkt);
    }
    av_packet_free(&pkt);

    file.close();
    avformat_close_input(&ctx);
    qDebug()<<pkt->size<<"正常退出";
}
void AudioThread::setStop(bool stop){
    _stop=stop;
}

