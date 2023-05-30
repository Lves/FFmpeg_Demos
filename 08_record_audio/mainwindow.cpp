#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <qfile.h>

extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
}


#ifdef Q_OS_WIN
    #define FMT_NAME "dshow"
    #define DEVICE_NAME "audio="
    #define FILE_NAME "F:/out.pcm"
#else
    // 格式名称
    #define FMT_NAME "avfoundation"
    //设备名称
    #define DEVICE_NAME ":0"
    // PCM  文件名
    #define FILE_NAME "/Users/lixingle/Desktop/out.pcm"
#endif


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


void MainWindow::on_audioButton_clicked()
{
    //获取输入格式对象
    const AVInputFormat *fmt = av_find_input_format(FMT_NAME);

    if (!fmt) {
        qDebug() << "获取输入格式对象失败" << FMT_NAME;
        return;
    }

    // 打开设备
    // 格式上下文（将来可以利用上下文操作设备）
    AVFormatContext *ctx = nullptr;

    int ret = avformat_open_input(&ctx, DEVICE_NAME, fmt, nullptr);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug() << "打开设备失败" << errbuf;
        return;
    }

    // 文件名
    QFile file(FILE_NAME);
    // 打开文件
    // WriteOnly: 只写模式。如果文件不存在，创建文件；如果文件存在，清空文件
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "文件打开失败"<< FILE_NAME;
        avformat_close_input(&ctx);
        return;
    }

    //采集次数
    int count = 10;

    //数据包
    AVPacket pkt;

    //采集数据, 采集一次
    while (count-- > 0 && av_read_frame(ctx, &pkt) == 0) {
        //将数据写入文件
        file.write((const char *)pkt.data, pkt.size);
    }

    //释放自选
    //关闭文件
    file.close();
    // 关闭资源
    avformat_close_input(&ctx);
}

