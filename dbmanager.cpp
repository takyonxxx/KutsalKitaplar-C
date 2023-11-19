#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

DbManager::DbManager(const QString &path)
{
    // Open the copied database file
    m_db = QSqlDatabase::addDatabase("QSQLITE", "connection_kutsal_kitaplar");
    m_db.setDatabaseName(path);

    // Open the database and check the connection state
    if (openDatabase()) {
        // Check if tables exist after opening the database
        QStringList tables = m_db.tables();
        qDebug() << "Table count:" << tables.count();
    }
}

DbManager::~DbManager()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool DbManager::openDatabase()
{
    if (!m_db.isOpen()) {
        qDebug() << "Database not open. Attempting to open...";

        if (!m_db.open()) {
            qDebug() << "Error opening database:" << m_db.lastError().text();
            return false;
        }
    }
    qDebug() << "Database connection state:" << m_db.isOpen();
    return true;
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
QSqlQueryModel* DbManager::getSureler(BookTypes type)
{
    QSqlQueryModel* model = nullptr;
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

    model=executeQuery("SELECT DISTINCT sureadi, sureno FROM " + sureTable);
    if(model)
        model->setHeaderData(0, Qt::Horizontal, "Sure");

    if (model->lastError().isValid()) {
        qDebug() << "Error during query execution:" << model->lastError().text();
        // Release memory and return nullptr
        delete model;
        return nullptr;
    }

    return model;
}


QSqlQueryModel *DbManager::getAyetKelime(int sureno)
{
    QSqlQueryModel *model = executeQuery("SELECT sureno, ayet, latince, turkce FROM tbl_kuran_kelime WHERE sureno = "
                                         + QString::number(sureno) + " ORDER BY ayet ASC");
    model->setHeaderData(0, Qt::Horizontal, ("Sure"));
    model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
    model->setHeaderData(2, Qt::Horizontal, ("Latince"));
    model->setHeaderData(3, Qt::Horizontal, ("Turkce"));
    return model;
}


QSqlQueryModel *DbManager::getAyetKelimeByAyet(int sureno, int ayetno)
{
    QSqlQueryModel *model = executeQuery("SELECT sureno, ayet, latince, turkce FROM tbl_kuran_kelime WHERE sureno = "
                                         + QString::number(sureno)
                                         + " And ayet = " + QString::number(ayetno) + " ORDER BY ayet ASC");
    model->setHeaderData(0, Qt::Horizontal, ("Sure"));
    model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
    model->setHeaderData(2, Qt::Horizontal, ("Latince"));
    model->setHeaderData(3, Qt::Horizontal, ("Turkce"));
    return model;
}

QSqlQueryModel *DbManager::searchAyet(BookTypes type, QString search_key)
{
    QSqlQueryModel *model = nullptr;
    QString sureTable;

    switch(type)
    {
    case BookTypes::Kuran:
        sureTable = "tbl_kuran_meal";
        model= executeQuery("SELECT sureno, ayet, meal FROM " + sureTable + " WHERE meal like '%" + search_key + "%' ORDER BY ayet ASC");
        if(model)
        {
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
            model->setHeaderData(2, Qt::Horizontal, ("Meal"));
        }
        break;
    case BookTypes::Tevrat:
        sureTable = "tbl_tevrat";
        model= executeQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE ayet like '%" + search_key + "%' ORDER BY ayet ASC");
        if(model)
        {
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
        }
        break;
    case BookTypes::Incil:
        sureTable = "tbl_incil";
        model= executeQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE ayet like '%" + search_key + "%' ORDER BY ayet ASC");
        if(model)
        {
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
        }
        break;
    case BookTypes::Zebur:
        sureTable = "tbl_zebur";
        model= executeQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE ayet like '%" + search_key + "%' ORDER BY ayet ASC");
        if(model)
        {
            model->setHeaderData(0, Qt::Horizontal, ("Sure"));
            model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
        }
        break;

    default:
        break;
    }

    return model;
}

QSqlQueryModel *DbManager::getAyet(BookTypes type, int sureno)
{
    QSqlQueryModel *model = nullptr;
    QString sureTable;

    switch(type)
    {
    case BookTypes::Kuran:
        sureTable = "tbl_kuran_meal";
        model= executeQuery("SELECT sureno, ayet, meal, arapca, latince, sureadi FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
        break;
    case BookTypes::Tevrat:
        sureTable = "tbl_tevrat";
        model= executeQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
        break;
    case BookTypes::Incil:
        sureTable = "tbl_incil";
        model= executeQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
        break;
    case BookTypes::Zebur:
        sureTable = "tbl_zebur";
        model= executeQuery("SELECT sureno, ayet FROM " + sureTable + " WHERE sureno = " + QString::number(sureno));
        break;

    default:
        break;
    }

    if(model)
    {
        model->setHeaderData(0, Qt::Horizontal, ("Sure"));
        model->setHeaderData(1, Qt::Horizontal, ("Ayet"));
    }
    return model;
}

QSqlQueryModel* DbManager::executeQuery(const QString &queryString)
{
    QSqlQuery query(m_db);

    if (query.exec(queryString)) {
        QSqlQueryModel *model = new QSqlQueryModel;
        model->setQuery(query);
        return model;
    } else {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    // Return null if there's an error
    return nullptr;
}
