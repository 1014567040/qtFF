#include "ffmpegs.h"
#include<QFile>
#include<QDebug>
FFmpegs::FFmpegs()
{
}
void FFmpegs::pcm2wav(WAVHeader &header,
                      const char *pcmFilename,
                      const char *wavFilename){
    //打开pcm文件
        QFile pcmfile(pcmFilename);
        if(!pcmfile.open(QFile::ReadOnly)){
            qDebug()<<"打开文件失败"<<pcmFilename;
            return;
    }

        //打开wav文件
            QFile wavfile(wavFilename);
        //打开文件，只写模式。如果文件不存在就创建文件，存在就会清空文件内容
            if(!wavfile.open(QFile::WriteOnly)){
                qDebug()<<"打开文件失败"<<wavFilename;
                return;
           }
        //写入头部
        wavfile.write((const char*)&header,sizeof(WAVHeader));
        //写入pcm数据
        char buf[1024];
        int size;
        while((size=pcmfile.read(buf,sizeof(buf)))>0){
            wavfile.write(buf,size);
        }
        //关闭文件
        pcmfile.close();
        wavfile.close();
}
