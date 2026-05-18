#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QByteArray>
#include <QAudioSource>
#include <QAudioSink>
#include <QIODevice>
#include <QMediaDevices>
#include <QScopedPointer>
#include <QThread>

class Speaker : public QIODevice
{
    Q_OBJECT

public:
    Speaker();
    void start();
    void stop();
    void getWaveFile(QString noteFile);
    void newTest(QByteArray bufferOut);
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    qint64 size() const override { return m_buffer.size(); }
    qint64 m_pos = 0;
    QByteArray m_buffer;

signals:
    void halt();

public slots:
    void startSound();
    void stopSound();
};

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

    QThread SpeakerThread;
    QMediaDevices *m_devicesOut = nullptr;
    QScopedPointer<Speaker> m_Speaker;
    QScopedPointer<QAudioSink> m_audioOutput;
    void do_Orientation(int);
    int playedNote;
    int kbPlayedNote;
    int tonicNote;
    int nPos;

public slots:
    void testSound();

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();

    void on_btnNext_clicked();

private:
    void initializeAudioOutput(const QAudioDevice &deviceInfo);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
