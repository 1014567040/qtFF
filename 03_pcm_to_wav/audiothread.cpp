#include "audiothread.h"
#include<QDebug>
#include<QFile>
#include<QDateTime>
#include"ffmpegs.h"
extern "C"{
    #include<libavdevice/avdevice.h>//设备
    #include<libavformat/avformat.h>//格式
    #include<libavutil/avutil.h>    //工具
}
AudioThread::AudioThread(QObject *parent) : QThread(parent)
{       //当监听到线程结束时（finished）,就调用deleteLater回收内存
       connect(this,&AudioThread::finished,
               this,&AudioThread::deleteLater);
}
AudioThread::~AudioThread(){
    requestInterruption();//结束线程
    quit();
    wait();
    qDebug()<<this<<"析构";
}
void AudioThread::run(){
    //获取输入格式对象
   const char *fmtname="dshow";
   const AVInputFormat *fmt =av_find_input_format(fmtname);
   if(!fmt){
        qDebug()<<"获取输入格式失败"<<fmtname;
        return;
   }
    //格式上下文（将来可以用上下文操作设备）
    AVFormatContext *ctx=nullptr;
    //设备名称
    const char *deviceName="audio=麦克风 (Realtek(R) Audio)";
    //打开设备
    int ret=avformat_open_input(&ctx,deviceName,fmt,nullptr);
    if(ret<0){
        char errbuf[1024];
        av_strerror(ret,errbuf,sizeof(errbuf));
        qDebug()<<"打开设备失败"<<errbuf;
        return;
    }
    //文件名
    //const char *filename="D:/CX/out.pcm";
    QString filename="D:/CX/";
    filename+=QDateTime::currentDateTime().toString("MM_dd_HH_mm_ss");
    filename+=".pcm";
    QFile file(filename);
    //打开文件，只写模式。如果文件不存在就创建文件，存在就会清空文件内容
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        qDebug()<<"打开文件失败"<<filename;
        //关闭设备
        avformat_close_input(&ctx);
        return;
    }


    //数据包
    AVPacket pkt;

    //采集次数
//    int count=50;
    //采集数据
//    while(!_stop&&av_read_frame(ctx,&pkt)==0){
//          //将数据写入文件
//          file.write((const char*)pkt.data,pkt.size);
//}

//        while(!isInterruptionRequested()&&av_read_frame(ctx,&pkt)==0){
//              //将数据写入文件
//              file.write((const char*)pkt.data,pkt.size);
//    }          有带点小问题

        while(!isInterruptionRequested()){
            ret=av_read_frame(ctx,&pkt);
            if(ret==0){
                file.write((const char*)pkt.data,pkt.size);
            }
            else{
                char errbuf[1024];
                av_strerror(ret,errbuf,sizeof(errbuf));
                qDebug()<<"打开设备失败"<<errbuf;
            }
        }

    //释放资源
    //关闭文件
    file.close();
    //关闭设备
    avformat_close_input(&ctx);

    qDebug()<<pkt.size<<"正常退出";

//    //pcm转wav文件
//    FFmpegs::pcm2wav("D:/CX/out.pcm","D:/CX/out.wav");
}
void AudioThread::setStop(bool stop){
    _stop=stop;
}
