#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    onTimeChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onTimeChanged(unsigned long long ms){
    QTime time(0,0,0,0);
    QString text=time.addMSecs(ms).toString("mm:ss.z");
    ui->time->setText(text.left(7));
}
void MainWindow::on_pushButton_clicked()
{
    if(!_audioThread){
        _audioThread=new AudioThread(this);
              _audioThread->start();

              connect(_audioThread,&AudioThread::timeChanged,
                      this,&MainWindow::onTimeChanged);

              connect(_audioThread,&AudioThread::finished,
              [this](){
                  _audioThread=nullptr;
                  ui->pushButton->setText("开始录音");
              });
        ui->pushButton->setText("结束录音");
    }else{
         _audioThread->requestInterruption();
         _audioThread=nullptr;
         ui->pushButton->setText("开始录音");
    }

}
