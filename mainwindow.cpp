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
}

MainWindow::~MainWindow()
{
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
    exit(0);
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
