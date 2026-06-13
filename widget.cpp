#include "widget.h"
#include "ui_widget.h"
#include "fileloader.h"
#include <QDebug>
#include <QThread>
#include <QFile>

int curLessonInt = 0;
int orientation [21] = {1,2,3,4,5,6,7,8,1,8,1,8,1,8,7,6,5,4,3,2,1};
QMap<QString, int> tonic_map = {
    {"G3", 43}, {"A3", 45}, {"B3", 47}, {"C4", 48}, {"D4", 50}, {"A4", 52}, {"B4", 54}, {"C5", 55}
};
Speaker speaker;
QThread speakerThread;
extern QList<QByteArray> rawRecArrays;

Speaker::Speaker()
{

}

void Speaker::start()
{
    open(QIODevice::ReadOnly);
}

void Speaker::stop()
{
    m_pos = 0;
    close();
}

void Speaker::newTest(QByteArray bufferOut)
{
    qDebug()<<"  %%%%  NEW TEST ";
    m_buffer.assign(bufferOut);
    m_buffer.clear();
    qDebug()<<&m_buffer<<" m_buffer_.size() " << m_buffer.size();
    m_buffer = bufferOut;
    qDebug()<<&m_buffer<<" m_buffer_.size() " << m_buffer.size();
}

qint64 Speaker::readData(char *data, qint64 len)
{
    qDebug() << "enter speaker readdata...is on main? " << QThread::isMainThread();
    qint64 total = 0;
    if (!m_buffer.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
            m_pos = (m_pos + chunk) % m_buffer.size();
            total += chunk;
            qDebug() << "chunk..." << chunk << "pos> = " << m_pos ;
        }
    }    
    return total;
}

qint64 Speaker::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Speaker::bytesAvailable() const
{
    return QIODevice::bytesAvailable();
}

void Speaker::startSound()
{
    qDebug() << "is Main Thread in startSound: " << QThread::isMainThread();

}

void Speaker::stopSound()
{
    stop();
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initializeAudioOutput(m_devicesOut->defaultAudioOutput());
    FileLoader::ReadConfig();
    FileLoader::ReadLesson();
    FileLoader::GetRandomTestSet(gTestGroup[curLessonInt]);
    connect(this,&Widget::halt,this,&Widget::testSound, Qt::DirectConnection);
}

Widget::~Widget()
{
    SpeakerThread.terminate();
    delete ui;
}

void Widget::do_Orientation(int)
{
    m_Speaker->newTest( rawRecArrays[orientation[nPos] - 1]);
    qDebug() << "orientation value: " << orientation[nPos] - 1;
    m_Speaker->start();
    m_audioOutput->stop();
    m_audioOutput->start(m_Speaker.data());
    SpeakerThread.start();
}

void Widget::testSound()
{
    qDebug() << "test Sound...";
    SpeakerThread.exit();
    SpeakerThread.start();
}


void Widget::initializeAudioOutput(const QAudioDevice &deviceInfo)
{
    qDebug() << "initializeAudioOutput...";
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);
    qDebug() << "     !!!   from  INIT format: " << format.sampleRate();
    m_Speaker.reset(new Speaker());
    m_audioOutput.reset(new QAudioSink(deviceInfo, format));
}

void Widget::on_btnStart_clicked()
{
    qDebug() << "starting...";
    ui->lbStatus->setText("starting...");
    curLessonInt = currentlesson.toInt();
    // get sound array set
    tonicNote = tonic_map[gNote[curLessonInt-1]];
    qDebug() << tonicNote;
    qDebug() << gNote[curLessonInt-1];
    FileLoader files;
    files.GetFileList(tonicNote);
    nPos = 0;
    m_Speaker->moveToThread(&SpeakerThread);
    do_Orientation(nPos);
    nPos++;
}

void Widget::on_btnStop_clicked()
{
    emit halt();
    qDebug() << "stopped...";
    ui->lbStatus->setText("halt called...");
}

void Widget::on_btnNext_clicked()
{
    ui->lbStatus->setText("Next...");
    do_Orientation(nPos);
    nPos++;
}
