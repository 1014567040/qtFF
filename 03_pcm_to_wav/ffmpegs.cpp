#include "ffmpegs.h"
#include<QFile>
#include<QDebug>
FFmpegs::FFmpegs()
{
}
void FFmpegs::pcm2wav(WAVHeader &header,
                      const char *pcmFilename,
                      const char *wavFilename){
        QFile pcmfile(pcmFilename);
        if(!pcmfile.open(QFile::ReadOnly)){
            qDebug()<<"打开文件失败"<<pcmFilename;
            return;
    }

            QFile wavfile(wavFilename);
            if(!wavfile.open(QFile::WriteOnly)){
                qDebug()<<"打开文件失败"<<wavFilename;
                return;
           }

        wavfile.write((const char*)&header,sizeof(WAVHeader));
        char buf[1024];
        int size;
        while((size=pcmfile.read(buf,sizeof(buf)))>0){
            wavfile.write(buf,size);
        }
        pcmfile.close();
        wavfile.close();
}
