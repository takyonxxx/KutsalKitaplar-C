#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Kutsal Kitaplar");
    currentFont = 12;

    auto dbFile = QCoreApplication::applicationDirPath() + "/Kutsal_Kitaplar.db";
    if (!QFile::exists(dbFile))
    {
        fprintf(stderr, "%s file not found, will be created.\n", dbFile.toStdString().c_str());
        QString dbName(":/Kutsal_Kitaplar.db");
        createFile(dbName);
    }

    currentType = BookTypes::Kuran;
    ui->centralwidget->setStyleSheet("font-size: 12pt; color:#ECF0F1; background-color: #AAB7B8;");
    ui->textAyetler->setStyleSheet("font-size: 12pt; color:#ECF0F1; background-color: #0E6655; padding: 6px; spacing: 6px;");
    ui->listSureler->setStyleSheet("font-size: 12pt; color:#212F3D ; background-color: #ECF0F1 ;padding: 6px; spacing: 6px;");
    ui->tableKelime->setStyleSheet("font-size: 12pt; color:#212F3D; background-color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->pushSearch->setStyleSheet("font-size: 13pt; font-weight: bold; color: white;background-color:#154360; padding: 6px; spacing: 6px;");
    ui->pushExit->setStyleSheet("font-size: 13pt; font-weight: bold; color: white;background-color:#154360; padding: 6px; spacing: 6px;");
    ui->comboKitaplar->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
    ui->comboFont->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
    ui->labelFont->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
    ui->lineEditSearch->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
    ui->comboFont->setCurrentIndex(0);

    db = new DbManager(dbFile);

    if (db->isOpen())
    {
        auto m_sureler = db->getSureler(currentType);
        ui->listSureler->setModel(m_sureler);

        ui->listSureler->setSelectionMode(QAbstractItemView::SingleSelection);

        QModelIndex index= ui->listSureler->model()->index(0,0);
        on_listSureler_clicked(index);

        /*QTableView *view = new QTableView;
        view->setModel(m_sureler);
        view->show();*/
    }
    else
    {
        qDebug() << "Database is not open!";
    }
}

MainWindow::~MainWindow()
{
    delete db;
    delete ui;
}

void MainWindow::createFile(QString &fileName)
{
    if (QFile::exists(fileName))
    {
        QFile::copy(fileName, "Kutsal_Kitaplar.db");
    }
}


void MainWindow::on_listSureler_clicked(const QModelIndex &index)
{
    auto model = ui->listSureler->model();
    auto sureno =  model->data(model->index(index.row(), 1)).toInt();

    if (currentType == BookTypes::Kuran)
    {
        auto suretam = model->data(index).toString();
        auto model_kelime = db->getAyetKelime(sureno);
        ui->tableKelime->setModel(model_kelime);
    }

    auto model_ayet = db->getAyet(currentType, sureno);

    ui->textAyetler->clear();

    for(int i = 0; i < model_ayet->rowCount(); ++i)
    {
        QString meal = model_ayet->record(i).value(1).toString() + " - " + model_ayet->record(i).value(2).toString();
        ui->textAyetler->append(meal);
    }

    ui->textAyetler->moveCursor(QTextCursor::Start);    
    ui->tableKelime->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::on_pushSearch_clicked()
{
    if(ui->lineEditSearch->text().isEmpty())
        return;

    ui->textAyetler->clear();

    auto model = db->searchAyet(currentType, ui->lineEditSearch->text());

    for(int i = 0; i < model->rowCount(); ++i)
    {
        QString meal = model->record(i).value(1).toString() + " - " + model->record(i).value(2).toString();
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

    auto m_sureler = db->getSureler(currentType);
    ui->listSureler->setModel(m_sureler);

    ui->listSureler->setSelectionMode(QAbstractItemView::SingleSelection);

    QModelIndex m_index= ui->listSureler->model()->index(0,0);
    on_listSureler_clicked(m_index);

}

void MainWindow::on_comboFont_currentIndexChanged(const QString &arg1)
{
    currentFont = arg1.toInt();
    ui->textAyetler->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; color:#ECF0F1; background-color: #0E6655; padding: 6px; spacing: 6px;");
    ui->listSureler->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; color:#212F3D ; background-color: #ECF0F1 ;padding: 6px; spacing: 6px;");
    ui->tableKelime->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; color:#212F3D; background-color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->comboKitaplar->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
    ui->comboFont->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
    ui->labelFont->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
    ui->lineEditSearch->setStyleSheet("font-size: " + QString::number(currentFont) + "pt; font-weight: bold; color: white;background-color:#148F77 ; padding: 6px; spacing: 6px;");
}
