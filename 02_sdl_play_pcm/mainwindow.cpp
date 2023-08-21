#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <SDL2/SDL.h>
#include <QDebug>
#include<QFile>

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

void showVersion(){
    SDL_version version;
    SDL_VERSION(&version);
    qDebug()<<version.major<<version.minor<<version.patch;
}
void MainWindow::on_pushButton_clicked()
{
    showVersion();
    if(_playThread){
        _playThread->requestInterruption();
        _playThread=nullptr;
        ui->pushButton->setText("开始播放");
    }else{
        _playThread=new PlayThread1(this);
        _playThread->start();
        connect(_playThread,&PlayThread1::finished,
                [this](){
            _playThread=nullptr;
            ui->pushButton->setText("开始播放");
        });
        ui->pushButton->setText("停止播放");
    }
}
