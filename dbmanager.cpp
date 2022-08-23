#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

DbManager::DbManager(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

DbManager::~DbManager()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool DbManager::isOpen() const
{
    return m_db.isOpen();
}

void DbManager::showTables()
{
    QSqlQuery sqlite_masterQuery( m_db );
    if( !sqlite_masterQuery.exec( "SELECT * FROM sqlite_master" ) )
    {
        //Something with the SQL-Query or the DB-connection is wrong.
        QString lastError = sqlite_masterQuery.lastError().text();
        qDebug() << lastError;
    }
    else
    {
        //Here we got some valid results from the sql-query above
        do
        {
            QString tableName = sqlite_masterQuery.value("name").toString();
            if( sqlite_masterQuery.value("type").toString() == "table" && tableName != "sqlite_sequence" )
                //The "sqlite_sequence" table is an internal table used to help implement AUTOINCREMENT
            {
                qDebug() << tableName;
            }
        } while( sqlite_masterQuery.next() );
    }

}

QSqlQueryModel *DbManager::getSureler(BookTypes type)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString sureTable;

    switch(type)
    {
        case BookTypes::Kuran:
            sureTable = "tbl_kuran_sureler";
            break;
        case BookTypes::Tevrat:
            sureTable = "tbl_tevrat_sureler";
            break;
        case BookTypes::Incil:
            sureTable = "tbl_incil_sureler";
            break;
        case BookTypes::Zebur:
            sureTable = "tbl_zebur_sureler";
            break;

        default:
        break;
    }

    model->setQuery("SELECT DISTINCT sureadi, sureno FROM " + sureTable);
    model->setHeaderData(0, Qt::Horizontal, ("Sure"));
    return model;
}

QSqlQueryModel *DbManager::getAyetKelime(int sureno)
{
     QSqlQueryModel *model = new QSqlQueryModel;

     model->setQuery("SELECT sureno, ayet, latince, turkce FROM tbl_kuran_kelime WHERE sureno = " + QString::number(sureno) + " ORDER BY ayet ASC");
     model->setHeaderData(0, Qt::Horizontal, ("Sure"));
     model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
     model->setHeaderData(2, Qt::Horizontal, ("Latince"));
     model->setHeaderData(3, Qt::Horizontal, ("Turkce"));
     return model;
}


QSqlQueryModel *DbManager::getAyetKelimeByAyet(int sureno, int ayetno)
{
     QSqlQueryModel *model = new QSqlQueryModel;

     model->setQuery("SELECT sureno, ayet, latince, turkce FROM tbl_kuran_kelime WHERE sureno = " + QString::number(sureno)
                     + " And ayet = " + QString::number(ayetno) + " ORDER BY ayet ASC");
     model->setHeaderData(0, Qt::Horizontal, ("Sure"));
     model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
     model->setHeaderData(2, Qt::Horizontal, ("Latince"));
     model->setHeaderData(3, Qt::Horizontal, ("Turkce"));
     return model;
}

QSqlQueryModel *DbManager::searchAyet(BookTypes type, QString search_key)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString sureTable;

    switch(type)
    {
        case BookTypes::Kuran:
            sureTable = "tbl_kuran_meal";
            model->setQuery("SELECT sureno, ayet, meal FROM " + sureTable + " WHERE meal like '%" + search_key + "%' ORDER BY ayet ASC");
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
            model->setHeaderData(2, Qt::Horizontal, ("Meal"));
            break;
        case BookTypes::Tevrat:
            sureTable = "tbl_tevrat";
            model->setQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE ayet like '%" + search_key + "%' ORDER BY ayet ASC");
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
            break;
        case BookTypes::Incil:
            sureTable = "tbl_incil";
            model->setQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE ayet like '%" + search_key + "%' ORDER BY ayet ASC");
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
            break;
        case BookTypes::Zebur:
            sureTable = "tbl_zebur";
            model->setQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE ayet like '%" + search_key + "%' ORDER BY ayet ASC");
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
            break;

        default:
        break;
    }

    return model;
}

QSqlQueryModel *DbManager::getAyet(BookTypes type, int sureno)
{
     QSqlQueryModel *model = new QSqlQueryModel;
     QString sureTable;

     switch(type)
     {
         case BookTypes::Kuran:
             sureTable = "tbl_kuran_meal";
              model->setQuery("SELECT sureno, ayet, meal, arapca, latince, sureadi FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
             break;
         case BookTypes::Tevrat:
             sureTable = "tbl_tevrat";
             model->setQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
             break;
         case BookTypes::Incil:
             sureTable = "tbl_incil";
             model->setQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
             break;
         case BookTypes::Zebur:
             sureTable = "tbl_zebur";
             model->setQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
             break;

         default:
         break;
     }

     model->setHeaderData(0, Qt::Horizontal, ("Sure"));
     model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
     return model;
}
