#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QHeaderView>
#include <QFile>
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
    void on_listSureler_clicked(const QModelIndex &index);
    void on_pushSearch_clicked();

    void on_pushExit_clicked();

    void on_comboKitaplar_currentIndexChanged(int index);

    void on_comboFont_currentIndexChanged(const QString &arg1);

    void on_textAyetler_cursorPositionChanged();

private:
    void createFile(QString &fileName);

    BookTypes currentType{};
    int currentSure{};
    int currentFont{12};
    DbManager *db{};
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
