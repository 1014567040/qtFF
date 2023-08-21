#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H
#include<QLabel>
#include <QThread>
#include <QAudioRecorder>
#include<QTimer>
class AudioThread : public QThread
{
    Q_OBJECT
private:
    void run()override;
    bool _stop=false;
public:
    explicit AudioThread(QObject *parent = nullptr);
    ~AudioThread();
    void setStop(bool stop);
signals:

};

#endif // AUDIOTHREAD_H
