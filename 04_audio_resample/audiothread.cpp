#include "audiothread.h"
#include"ffmpegs.h"
#include<QDebug>
#include<QFile>

extern "C"{
    #include<libswresample/swresample.h>
    #include<libavutil/avutil.h>    //工具
}
AudioThread::AudioThread(QObject *parent) : QThread(parent)
{
    //当监听到线程结束时（finished）,就调用deleteLater回收内存
       connect(this,&AudioThread::finished,
               this,&AudioThread::deleteLater);
}
AudioThread::~AudioThread(){
    //断开所有连接
    disconnect();
    requestInterruption();//结束线程
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
    //int inChLayout=AV_CH_LAYOUT_MONO;一个声道

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


//const char *inFilename="D:/CX/44100_s16le_21.pcm";
//QFile inFile(inFilename);
//const char *outFilename="D:/CX/48000_f32le_1.pcm";
//QFile outFile(outFilename);
////输入参数
//AVSampleFormat inSamplefmt=AV_SAMPLE_FMT_S16;
//int inSampleRate=44100;
//int inChLayout=AV_CH_LAYOUT_STEREO;
////int inChLayout=AV_CH_LAYOUT_MONO;一个声道

////输入缓冲区
////指向缓冲区的指针
//uint8_t **inData=nullptr;
////缓冲区的大小
//int inLinesize=0;
////声道数
//int inChs=av_get_channel_layout_nb_channels(inChLayout);
////一个样本的大小
//int inBytesPerSample=inChs*av_get_bytes_per_sample(inSamplefmt);
////缓冲区的样本数量
//int inSamples=1024;
////读取文件数据的大小
//int len=0;

////输出参数
//AVSampleFormat outSamplefmt=AV_SAMPLE_FMT_FLT;
//int outSampleRate=48000;
//int outChLayout=AV_CH_LAYOUT_MONO;
////输出缓冲区
////指向缓冲区的指针
//uint8_t **outData=nullptr;
////缓冲区的大小
//int outLinesize=0;
////声道数
//int outChs=av_get_channel_layout_nb_channels(inChLayout);
////一个样本的大小
//int outBytesPerSample=outChs*av_get_bytes_per_sample(outSamplefmt);
////缓冲区的样本数量
//int outSamples=av_rescale_rnd(outSampleRate,inSamples,inSampleRate,AV_ROUND_UP);

////inSampeRate     inSamples
////-----------  =  -------------
////outSampleRate=  outSamples

//int ret=0;
////创建重采样上下文
//SwrContext*ctx=swr_alloc_set_opts(nullptr,
//                                  outChLayout,outSamplefmt,outSampleRate,
//                                  inChLayout,inSamplefmt,inSampleRate,
//                                  0,nullptr);//一般看见alloc一般是指在队空间分配内存
//if(!ctx){
//    qDebug()<<"swr_alloc_set_opts error";
//    goto end;
//}
////初始化重采样上下文
// ret=swr_init(ctx);
//if(ret<0){
//    ERROR_BUF(ret);
//    qDebug()<<"swr_init_error:"<<errbuf;
//    goto end;
//}

////indata=av_calloc(1,sizeof(uint_8_t)*)

////创建输入缓冲区
//ret=av_samples_alloc_array_and_samples(&inData,&inLinesize,inChs,inSamples,inSamplefmt,1);
//if(ret<0){
//    ERROR_BUF(ret);
//    qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
//    goto end;
//}
////创建输出缓冲区
//ret=av_samples_alloc_array_and_samples(&outData,&outLinesize,outChs,outSamples,outSamplefmt,1);
//if(ret<0){
//    ERROR_BUF(ret);
//    qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
//    goto end;
//}
////打开文件
//if(!inFile.open(QFile::ReadOnly)){
//    qDebug()<<"file open error:"<<inFilename;
//    goto end;
//}
//if(!outFile.open(QFile::WriteOnly)){
//    qDebug()<<"file open error:"<<outFilename;
//    goto end;
//}
//while((len=inFile.read((char*)inData[0],inLinesize))){
//    //读取的样本数量
//    inSamples=len/inBytesPerSample;
//    //重采样(返回值是转换后的样本数量)
//    ret=swr_convert(ctx,
//                outData,outSamples,
//                (const uint8_t **)inData,inSamples);
//    if(ret<0){
//        ERROR_BUF(ret);
//        qDebug()<<"swr_conver error:"<<errbuf;
//        goto end;
//    }

//    //int size=av_samples_get_buffer_size(nullptr,outChs,ret,outSampleFmt,1)==ret*outBytesperSampke   兼容，对齐。

//    //将转换后的数据写入到输出文件中
//    outFile.write((char*)outData[0],ret*outBytesPerSample);
//}

////检查一下输入缓冲区是否还有残留的的样本（已经重采样过的，转换过的）
//while((ret=swr_convert(ctx,
//                      outData,outSamples,
//                      nullptr,0))>0){
//    qDebug()<<ret;
//    outFile.write((char*)outData[0],ret*outBytesPerSample);
//}
//end:
////释放资源
////关闭文件
//inFile.close();
//outFile.close();
////释放输入缓冲区
//av_freep(&inData);
////释放输出缓冲区
//av_freep(&outData);
////释放重采样上下文
//swr_free(&ctx);

////    void *ptr=malloc(100);
////    freep(&ptr);
