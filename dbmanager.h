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
    QSqlQueryModel *getSureler(BookTypes type);
    QSqlQueryModel *getAyet(BookTypes type, int sureno);
    QSqlQueryModel *getAyetKelime(int sureno);
    QSqlQueryModel *searchAyet(BookTypes type, QString search_key);

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
