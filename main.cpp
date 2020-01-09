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
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to establish a connection to the media database.\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    // Create tables. Make sure to check if the tables already exists.A
    // MOVE SOME STUFF INTO FUNCTIONS
    // Add delete on cascade?
    QString createSongTable =
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
                   "FOREIGN KEY(artist_id) REFERENCES Artist(id)"
            ");";

    QString createArtistTable =
            "CREATE TABLE IF NOT EXISTS Artist ("
                "id     INTEGER PRIMARY KEY,"
                "name   VARCHAR(255) UNIQUE NOT NULL"
            ");";

    QString createPlaylistTable =
            "CREATE TABLE IF NOT EXISTS Playlist ("
                "id     INTEGER PRIMARY KEY,"
                "name   VARCHAR(255) UNIQUE NOT NULL"
            ");";

    QString createPlaylistSongsTable =
            "CREATE TABLE IF NOT EXISTS PlaylistSongs ("
                "playlist_id    INTEGER,"
                "track_id       INTEGER,"
                "FOREIGN KEY(playlist_id) REFERENCES Playlist(id),"
                "FOREIGN KEY(track_id) REFERENCES Track(id)"
            ");";

    QSqlQuery query;
    query.exec(createSongTable);
    qDebug() << query.lastError();
    query.exec(createArtistTable);
    query.exec(createPlaylistTable);
    query.exec(createPlaylistSongsTable);
    query.exec("INSERT INTO Playlist (name)"
               "VALUES ('All Tracks');");

    query.exec("INSERT INTO Artist (name)"
               "VALUES ('BANKS');");

    query.exec("INSERT INTO Artist (name)"
               "VALUES ('ME');");

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
