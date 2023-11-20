#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QPainter>

enum class BookTypes
{
    Kuran,
    Tevrat,
    Incil,
    Zebur
};


class WordWrapDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        QTextDocument doc;
        QTextCursor cursor(&doc);
        cursor.insertHtml(opt.text);

        opt.text = "";
        doc.setTextWidth(opt.rect.width());

        QTextCharFormat charFormat;
        charFormat.setForeground(QBrush(Qt::white));
        charFormat.setFontPointSize(12);

        cursor.setPosition(0);
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        cursor.setCharFormat(charFormat);

        painter->save();

        painter->translate(opt.rect.x(), opt.rect.y());
        doc.drawContents(painter);

        painter->restore();
    }

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
