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
QMap<int, QString> tilesGoodBadMap = {
    {0, "note0"}, {1, "note2"}, {2, "note4"}, {3, "note5"}, {4, "note7"}, {5, "note9"},
    {6, "note11"}, {7, "note12"}
};
QMap<int, int> tileKbShift = {
    {0, 0}, {1, 2}, {2, 4}, {3, 5}, {4, 7}, {5, 9}, {6, 11}, {7, 12}
};
Speaker speaker;
QThread speakerThread;
QThread microphoneThread;
extern bool collectMicData;
extern double rec_arr[];    // DEFINED AS DOUBLE FOR FFTW
extern int rec_arr_cnt;
extern int frame_start;
extern int frame_size;
extern int frame_end;
extern int rec_arr_end;
QByteArray bufferReadTo;
QByteArray *currentAudioOut;
QList<QByteArray> note_tone;
int major_number_list[] = {0,2,4,5,7,9,11,12};
int tonic_nunber;
int audio_number_list;
const QList <QString> note_letters = {"C", "C#", "D", "D#", "E",
                                     "F", "F#", "G", "G#", "A", "A#", "B" };
extern float note_acc;
extern int noteC_no;
extern int noteC_oct;
extern float nn_l[12];
extern QList<QByteArray> rawRecArrays;
int accValue = 0;
int displayDuration = 3000;


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
    qDebug() << "enter readdata..." << QThread::isMainThread();
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
    qDebug() << "is Main Thread in readData: " << QThread::isMainThread();
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
    //connect(m_Speaker,&Speaker::halt,this,&Widget::closingSound);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::do_Orientation(int)
{
    m_Speaker->newTest( rawRecArrays[orientation[nPos] - 1]);
    qDebug() << "orientation value: " << orientation[nPos] - 1;
    playedNote = orientation[nPos] - 1;
    kbPlayedNote = playedNote + tonicNote;
    m_Speaker->start();
    m_audioOutput->stop();
    m_audioOutput->start(m_Speaker.data());
}

void Widget::closingSound()
{
    ui->lbStatus->setText("closing...");
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
    if (currentlesson != "1")

    qDebug() << "starting to build array set";

    // get sound array set
    tonicNote = tonic_map[gNote[curLessonInt-1]];
    qDebug() << tonicNote;
    qDebug() << gNote[curLessonInt-1];
    FileLoader files;
    files.GetFileList(tonicNote);
    nPos = 0;
    m_Speaker->moveToThread(&SpeakerThread);
    SpeakerThread.start();
    do_Orientation(nPos);
    nPos++;
}


void Widget::on_btnStop_clicked()
{
    qDebug() << "stoped...";
    ui->lbStatus->setText("stoped...");
}


void Widget::on_btnNext_clicked()
{
    do_Orientation(nPos);
    nPos++;
}

