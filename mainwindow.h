#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QHeaderView>
#include <QFile>
#include <QFileInfo>
#include <QAudioSink>
#include <QMediaDevices>
#include <QTextToSpeech>
#include <QQueue>
#include <QDebug>
#include <dbmanager.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushSearch_clicked();
    void on_pushExit_clicked();
    void on_comboKitaplar_currentIndexChanged(int);
    void on_textAyetler_cursorPositionChanged();    
    void on_comboFont_currentIndexChanged(int);
    void on_comboSureler_currentIndexChanged(int index);
    void handleAudioStateChanged(QAudio::State);
    void onSpeechStateChanged(QTextToSpeech::State state);

    void on_pushPlay_clicked();

private:
    void createFile(const QString&, const QString&);
    void setSpeechEngine();

    QQueue<QString> m_textQueue;
    void speakNextLine();

    const unsigned sampleRate = 48000;
    const unsigned channelCount = 2;

    QAudioSink *m_audioOutput = nullptr;
    QTextToSpeech *m_speech = nullptr;
    QAudioFormat m_format;
    bool m_speaking = false;

    BookTypes currentType{};
    int currentSure{};
    int currentFont{12};
    DbManager *db{};
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
