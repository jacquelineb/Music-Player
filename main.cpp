#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

bool connectToMediaDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("Q:\\Documents\\testdb\\db.sqlite"); // just testing for now.
    //qDebug() << QCoreApplication::applicationDirPath();
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/mediadb.sqlite");
    if (!db.open())
    {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open media database"),
            QObject::tr("Unable to establish a connection to the media database.\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    // Create tables. Make sure to check if the tables already exists.A
    // MOVE SOME STUFF INTO FUNCTIONS
    // Add delete on cascade?
    QString createArtistTable =
            "CREATE TABLE IF NOT EXISTS Artist ("
                "id     INTEGER PRIMARY KEY,"
                "name   VARCHAR(255) UNIQUE NOT NULL)";

    QString createTrackTable =
            "CREATE TABLE IF NOT EXISTS Track ("
                   "id          INTEGER PRIMARY KEY,"
                   "title       VARCHAR(255),"
                   "artist_id   INTEGER,"
                   "album       VARCHAR(255),"
                   "track_num   INTEGER,"
                   "year        INTEGER,"
                   "genre       VARCHAR(255),"
                   "duration    INTEGER,"
                   "location    TEXT UNIQUE NOT NULL,"
                   "FOREIGN KEY(artist_id) REFERENCES Artist(id))";


    QSqlQuery query;
    query.exec(createTrackTable);
    query.exec(createArtistTable);

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
