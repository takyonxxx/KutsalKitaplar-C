#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QHeaderView>
#include <QFile>
#include <QFileInfo>
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
    void on_listSureler_clicked(const QModelIndex &);
    void on_pushSearch_clicked();
    void on_pushExit_clicked();
    void on_comboKitaplar_currentIndexChanged(int);
    void on_textAyetler_cursorPositionChanged();    
    void on_comboFont_currentIndexChanged(int);

private:
    void createFile(const QString&, const QString&);

    BookTypes currentType{};
    int currentSure{};
    int currentFont{12};
    DbManager *db{};
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
