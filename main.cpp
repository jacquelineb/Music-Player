#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QtSql>

bool connectToMediaDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("Q:\\Documents\\testdb\\db.sqlite"); // just testing for now.
    //qDebug() << QCoreApplication::applicationDirPath();
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/mediadb.sqlite");
    if (!db.open())
    {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to establish a connection to the media database.\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    // Create tables
    QString createSongTable =
            "CREATE TABLE Song ("
                   "id          INTEGER PRIMARY KEY,"
                   "title       VARCHAR(255),"
                   "artist_id   INTEGER,"
                   "album       VARCHAR(255),"
                   "track_num   INTEGER,"
                   "year        INTEGER,"
                   "genre       VARCHAR(255),"
                   "duration    INTEGER,"
                   "location    TEXT UNIQUE NOT NULL"
            ");";

    QString createArtistTable =
            "CREATE TABLE Artist ("
                "id     INTEGER PRIMARY KEY,"
                "name   VARCHAR(255) UNIQUE NOT NULL"
            ");";

    QSqlQuery query;
    query.exec(createSongTable);
    query.exec(createArtistTable);

    query.exec("INSERT INTO Artist (name)"
               "VALUES ('BANKS');");

    query.exec("INSERT INTO Artist (name)"
               "VALUES ('ME');");
    // Set table relations
    //QSqlRelationalTableModel *relationalTableModel = new QSqlRelationalTableModel;
    //rel

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!connectToMediaDb())
    {
        return EXIT_FAILURE;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
