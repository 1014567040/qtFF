#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>

class PlayThread : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    ~PlayThread();
private:
    void run();
signals:

};

#endif // PLAYTHREAD_H
