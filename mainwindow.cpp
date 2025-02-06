#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGuiApplication>
#include <QScreen>
#include<QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Kutsal Kitaplar");

    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QRect screenGeometry = primaryScreen->availableGeometry();
    setGeometry(screenGeometry);

    currentFont = 12;
    QString dbFile;

    qDebug() << "Available database drivers:" << QSqlDatabase::drivers();
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qDebug() << "SQLite driver not available";
    }

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    QString iosWritablePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    dbFile = iosWritablePath + "/Kutsal_Kitaplar.db";
#else
    dbFile = QCoreApplication::applicationDirPath() + "/Kutsal_Kitaplar.db";
#endif

    QString dbName(":/Kutsal_Kitaplar.db");
    createFile(dbName, dbFile);

    currentType = BookTypes::Kuran;
    ui->comboFont->setCurrentIndex(1);
    ui->tableKelime->verticalHeader()->hide();
    ui->tableKelime->resizeColumnsToContents();
    ui->tableKelime->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableKelime->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableKelime->verticalHeader()->setMinimumSectionSize(100);


    db = new DbManager(dbFile);
    if (db->isOpen())
    {
        auto m_sureler = db->getSureler(currentType);
        ui->comboSureler->setModel(m_sureler);

        int columnIndex = m_sureler->record().indexOf("sureadi");
        ui->comboSureler->setModelColumn(columnIndex);

        QModelIndex index= ui->comboSureler->model()->index(0,0);
        int row = index.row();
        ui->comboSureler->setCurrentIndex(row);
    }
    else
    {
        qDebug() << "Database is not open!";
    }

    QAudioDevice outputDevice;

    for (auto &device: QMediaDevices::audioOutputs()) {

        if(device.description().contains("JBL"))
        {
            outputDevice = device;
            m_format.setSampleFormat(QAudioFormat::Int16);
            m_format.setSampleRate(sampleRate);
            m_format.setChannelCount(channelCount);

            if (outputDevice.isFormatSupported(m_format)) {
                m_audioOutput = new QAudioSink(outputDevice, m_format, this);
                connect(m_audioOutput,&QAudioSink::stateChanged, this, &MainWindow::handleAudioStateChanged);
                setSpeechEngine();
                qDebug() << device.description();
            }
            break;
        }
    }
}

MainWindow::~MainWindow()
{
    if(m_speech)
        delete m_speech;
    if(m_audioOutput)
        delete m_audioOutput;
    delete ui;
}

void MainWindow::createFile(const QString& resourcePath, const QString& destinationPath)
{
    QFile resourceFile(resourcePath);
    QFile destinationFile(destinationPath);

    if (destinationFile.exists()) {
        qDebug() << "Database file already exists at the destination:" << destinationPath;
    } else {
        if (resourceFile.open(QIODevice::ReadOnly) && destinationFile.open(QIODevice::WriteOnly)) {
            QByteArray data = resourceFile.readAll();
            destinationFile.write(data);
            destinationFile.close();
            resourceFile.close();
            qDebug() << "Database file copied to writable location:" << destinationPath;
        } else {
            qDebug() << "Error copying database file:" << resourceFile.errorString();
        }
    }
}

void MainWindow::setSpeechEngine()
{
    m_speech = new QTextToSpeech(this);
    m_speech->setPitch(0);
    connect(m_speech, &QTextToSpeech::stateChanged, this, &MainWindow::onSpeechStateChanged);

    // Populate the languages combobox before connecting its signal.
    const QVector<QLocale> locales = m_speech->availableLocales();
    for (const QLocale &locale : locales) {
        QString name(QString("%1 (%2)")
                         .arg(QLocale::languageToString(locale.language()))
                         .arg(QLocale::territoryToString(locale.territory())));
        QVariant localeVariant(locale);
        if (name.contains("Turkish"))
        {
            qDebug() << "Setting locale:" << name;
            m_speech->setLocale(locale);

            // Try to set a female voice
            QVector<QVoice> voices = m_speech->availableVoices();
            for (const QVoice &voice : voices)
            {
                if (voice.gender() == QVoice::Female)
                {
                    qDebug() << "Selected female voice:" << voice.name();
                    m_speech->setVoice(voice);
                    return; // Stop after setting the first female voice
                }
            }

            qDebug() << "No female voice found, using default voice.";
            return;
        }
    }
}

void MainWindow::onSpeechStateChanged(QTextToSpeech::State state)
{
    QString statusMessage;

    if (state == QTextToSpeech::Speaking) {
        statusMessage = tr("Speech started...");
        m_speaking = true;
    } else if (state == QTextToSpeech::Ready)
    {
        m_speaking = false;
        speakNextLine();
        statusMessage = tr("Speech stopped...playing next line.");
    }
    else if (state == QTextToSpeech::Paused)
        statusMessage = tr("Speech paused...");
    else
        statusMessage = tr("Speech error!");

    if (ui->statusBar) {
        ui->statusBar->showMessage(statusMessage);
    }
}

void MainWindow::on_pushSearch_clicked()
{
    if(ui->lineEditSearch->text().isEmpty())
        return;

    currentSure = 0;
    ui->textAyetler->clear();

    auto model = db->searchAyet(currentType, ui->lineEditSearch->text());

    for(int i = 0; i < model->rowCount(); ++i)
    {
        QString meal = model->record(i).value(0).toString() + " - " + model->record(i).value(1).toString() + " - " + model->record(i).value(2).toString();
        ui->textAyetler->append(meal);
    }

    ui->textAyetler->moveCursor(QTextCursor::Start);

    QString text = ui->textAyetler->toHtml();
    auto skey = ui->lineEditSearch->text();
    text.replace( skey, QString("<span style='color:yellow;'>%1</span>").arg(skey));
    ui->textAyetler->setHtml(text);
}

void MainWindow::on_pushExit_clicked()
{
    QApplication::quit();
}

void MainWindow::on_comboKitaplar_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        currentType = BookTypes::Kuran;
        break;
    case 1:
        currentType = BookTypes::Tevrat;
        break;
    case 2:
        currentType = BookTypes::Incil;
        break;
    case 3:
        currentType = BookTypes::Zebur;
        break;

    default:
        break;
    }

    if (currentType != BookTypes::Kuran)
        ui->tableKelime->setHidden(true);
    else
        ui->tableKelime->setHidden(false);

    auto m_sureler = db->getSureler(currentType);
    ui->comboSureler->setModel(m_sureler);
//    QModelIndex m_index= ui->comboSureler->model()->index(0,0);
}

void MainWindow::on_textAyetler_cursorPositionChanged()
{
    QTextCursor cursor;
    QString text;
    int ayet_number{};
    int sure_number{};

    cursor = ui->textAyetler->textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    text = cursor.selectedText();
    auto splitted = text.left(10).trimmed().split('-');


    if(currentSure == 0 && splitted.size() > 1)
    {
        sure_number = splitted[0].toInt();
        ayet_number = splitted[1].toInt();
    }
    else
    {
        sure_number = currentSure;
        ayet_number = splitted[0].toInt();
    }

    auto model_kelime = db->getAyetKelimeByAyet(sure_number, ayet_number);
    ui->tableKelime->setModel(model_kelime);

}


void MainWindow::on_comboFont_currentIndexChanged(int index)
{
    QString selectedFont = ui->comboFont->itemText(index);

    currentFont = selectedFont.toInt();
    ui->centralwidget->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; color:#ECF0F1; background-color: #AAB7B8;");
    ui->textAyetler->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; color:#ECF0F1; background-color: #003333; padding: 6px; spacing: 6px;");
    ui->tableKelime->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; color:white; background-color: #003333; padding: 6px; spacing: 6px;");
    ui->comboKitaplar->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#0C5F92 ; padding: 6px; spacing: 6px;");
    ui->comboSureler->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#0C5F92 ; padding: 6px; spacing: 6px;");
    ui->comboFont->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#0C5F92 ; padding: 6px; spacing: 6px;");
    ui->labelFont->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#248AC8 ; padding: 6px; spacing: 6px;");
    ui->lineEditSearch->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#248AC8 ; padding: 6px; spacing: 6px;");
    ui->pushSearch->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#154360; padding: 6px; spacing: 6px;");
    ui->pushPlay->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#154360; padding: 6px; spacing: 6px;");
    ui->pushExit->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#154360; padding: 6px; spacing: 6px;");

    // Set word wrap delegate for the DisplayRole
    WordWrapDelegate *wordWrapDelegate = new WordWrapDelegate;
    ui->tableKelime->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableKelime->setItemDelegateForColumn(0, wordWrapDelegate);
    ui->tableKelime->setItemDelegateForColumn(1, wordWrapDelegate);
    ui->tableKelime->horizontalHeader()->hide();
    ui->tableKelime->verticalHeader()->hide();
}


void MainWindow::on_comboSureler_currentIndexChanged(int index)
{
    currentSure = index + 1;

    if (currentType == BookTypes::Kuran)
    {
        auto model_kelime = db->getAyetKelime(currentSure);
        ui->tableKelime->setModel(model_kelime);
    }

    auto model_ayet = db->getAyet(currentType, currentSure);

    ui->textAyetler->clear();

    for (int i = 0; i < model_ayet->rowCount(); ++i)
    {
        QString meal = model_ayet->record(i).value(1).toString() + " - " + model_ayet->record(i).value(2).toString();
        ui->textAyetler->append(meal);
    }

    ui->textAyetler->moveCursor(QTextCursor::Start);
    ui->tableKelime->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::handleAudioStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::IdleState:
        m_audioOutput->stop();
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        if (m_audioOutput->error() != QAudio::NoError) {
            // Error handling
        }
        break;

    default:
        break;
    }
}

void MainWindow::speakNextLine()
{
    if (!m_textQueue.isEmpty())
    {
        QString nextLine = m_textQueue.dequeue();
        m_speech->say(nextLine);
    }
    else
    {
        ui->pushPlay->setText("Play"); // No more lines to speak
    }
}

void MainWindow::on_pushPlay_clicked()
{
    if (ui->pushPlay->text() == "Play")
    {
        if (m_audioOutput && m_speech)
        {
            ui->pushPlay->setText("Stop");

            // Split text into lines and enqueue them
            m_textQueue.clear();
            QStringList lines = ui->textAyetler->toPlainText().split("\n", Qt::SkipEmptyParts);
            for (const QString &line : lines)
            {
                m_textQueue.enqueue(line);
            }

            // Start speaking the first line
            speakNextLine();
        }
    }
    else
    {
        if (m_audioOutput && m_speech)
        {
            ui->pushPlay->setText("Play");
            m_textQueue.clear();  // Clear queue
            m_speech->stop();     // Stop speech
        }
    }
}

