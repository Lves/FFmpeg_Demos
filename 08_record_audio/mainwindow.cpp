#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <qfile.h>

extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
}

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
#ifdef Q_OS_WIN
    const char *fmtName = "dshow";
#else
    const char *fmtName = "avfoundation";
#endif
    const AVInputFormat *fmt = av_find_input_format(fmtName);

    if (!fmt) {
        qDebug() << "获取输入格式对象失败" << fmtName;
        return;
    }

    // 打开设备
    // 格式上下文（将来可以利用上下文操作设备）
    AVFormatContext *ctx = nullptr;
    // 设备名称
    const char *deviceName = ":2";

    int ret = avformat_open_input(&ctx, deviceName, fmt, nullptr);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug() << "打开设备失败" << errbuf;
        return;
    }

    // 文件名
    const char *fileName = "out.pcm";
    QFile file(fileName);
    // 打开文件
    // WriteOnly: 只写模式。如果文件不存在，创建文件；如果文件存在，清空文件
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "文件打开失败"<< fileName;
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

