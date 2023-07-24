#include "audiothread.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>

AudioThread::AudioThread(QWidget* parent) : QThread(parent), parentWidget(parent), stopRequested(false)
{
    // 创建 QAudioRecorder 对象
    audioRecorder = new QAudioRecorder(this);
}

AudioThread::~AudioThread()
{
    stopRequested = true;
    quit();
    wait();
    qDebug() << this << "析构";
}

void AudioThread::run()
{
    // 获取输入格式对象
    const char* fmtname = "dshow";
    const AVInputFormat* fmt = av_find_input_format(fmtname);
    if (!fmt) {
        qDebug() << "获取输入格式失败" << fmtname;
        return;
    }

    // 格式上下文（将来可以用上下文操作设备）
    AVFormatContext* ctx = nullptr;

    // 设备名称
    const char* deviceName = "audio=麦克风 (Realtek(R) Audio)";

    // 打开设备
    int ret = avformat_open_input(&ctx, deviceName, fmt, nullptr);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug() << "打开设备失败" << errbuf;
        return;
    }

    // 文件名
    QString filename = "D:/CX/";
    filename += QDateTime::currentDateTime().toString("MM_dd_HH_mm_ss");
    filename += ".pcm";
    QFile file(filename);

    // 打开文件，只写模式。如果文件不存在就创建文件，存在就会清空文件内容
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "打开文件失败" << filename;

        // 关闭设备
        avformat_close_input(&ctx);
        return;
    }

    // 数据包
    AVPacket pkt;

    while (!stopRequested) {
        ret = av_read_frame(ctx, &pkt);
        if (ret == 0) {
            file.write((const char*)pkt.data, pkt.size);
        } else {
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof(errbuf));
            qDebug() << "读取设备数据失败" << errbuf;
        }
    }

    // 释放资源

    // 关闭文件
    file.close();

    // 关闭设备
    avformat_close_input(&ctx);

    qDebug() << "正常退出";
}

void AudioThread::startRecording()
{
    start();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AudioThread::updateTimeLabel);
    timer->start(1000); // 每秒更新一次录音时间

    // 创建 QLabel 对象
    timeLabel = new QLabel(parentWidget);
    timeLabel->setText("00:00");
}

void AudioThread::stopRecording()
{
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }

    stopRequested = true;
    quit();
    wait();

    // 删除并释放 QLabel 对象
    delete timeLabel;
    timeLabel = nullptr;
}

void AudioThread::updateTimeLabel()
{
    // 获取录音时间并更新 QLabel 的文本
    qint64 duration = audioRecorder->duration(); // 获取录音时间（单位：毫秒）
    QTime time(0, 0);
    time = time.addMSecs(duration);
    QString timeText = time.toString("mm:ss");
    timeLabel->setText(timeText);
}
