#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQueryModel>

enum class BookTypes
{
    Kuran,
    Tevrat,
    Incil,
    Zebur
};

class DbManager
{
public:

    DbManager(const QString& path);
    ~DbManager();



    bool isOpen() const;
    void showTables();
    bool openDatabase();
    QSqlQueryModel* executeQuery(const QString &);
    QSqlQueryModel *getSureler(BookTypes);
    QSqlQueryModel *getAyet(BookTypes , int );
    QSqlQueryModel *getAyetKelime(int );
    QSqlQueryModel *getAyetKelimeByAyet(int , int );
    QSqlQueryModel *searchAyet(BookTypes , QString );

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
