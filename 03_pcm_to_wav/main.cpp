#include "mainwindow.h"

#include <QApplication>
#include "ffmpegs.h"
extern "C"{
    #include<libavdevice/avdevice.h>//设备
}
int main(int argc, char *argv[])
{
    WAVHeader header;
    header.riffChunkDataSize=1767996;
    header.sampleRate=44100;
    header.bitsPerSample=16;
    header.numChannels=2;
    header.blockAlign= header.bitsPerSample*header.numChannels>>3;
    header.byteRate = header.sampleRate*header.blockAlign;
    header.dataChunkDataSize=1767960;

    FFmpegs::pcm2wav(header,"D:/CX/05_05_17-56_13.pcm","D:/CX/out.wav");

    avdevice_register_all();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
