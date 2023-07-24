#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>
#include <QAudioRecorder>
#include <QLabel>
#include <QTimer>

extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

class AudioThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(QWidget* parent = nullptr);
    ~AudioThread();

    void run() override;

public slots:
    void startRecording();
    void stopRecording();
    void updateTimeLabel();

private:
    QAudioRecorder* audioRecorder;
    QLabel* timeLabel;
    QTimer* timer;
    QWidget* parentWidget;
    volatile bool stopRequested;
};

#endif // AUDIOTHREAD_H
