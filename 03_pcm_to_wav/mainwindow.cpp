#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(!_audioThread){
        _audioThread=new AudioThread(this);
              _audioThread->start();
              connect(_audioThread,&AudioThread::finished,
              [this](){
                  _audioThread=nullptr;
                  ui->pushButton->setText("开始录音");
              });
        ui->pushButton->setText("结束录音");
    }else{
         //_audioThread->setStop(true);
         _audioThread->requestInterruption();
         _audioThread=nullptr;
         ui->pushButton->setText("开始录音");
    }

}
