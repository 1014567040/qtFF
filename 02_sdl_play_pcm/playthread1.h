#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>

class PlayThread1 : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread1(QObject *parent = nullptr);
    ~PlayThread1();
private:
    void run();
signals:

};

#endif // PLAYTHREAD_H
