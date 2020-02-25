#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    restoreWindowState();
    connect(ui->actionAddToLibrary, &QAction::triggered, this, &MainWindow::onAddToLibraryActionTriggered);

    // =======
    // Change this later. Use taglib
    mediaToBeAdded = new QMediaPlayer(this);
    connect(mediaToBeAdded, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onAddMediaStatusChanged);
    // =======
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddToLibraryActionTriggered()
{
    QUrl filename = QFileDialog::getOpenFileUrl(this, "Add file to library");
    mediaToBeAdded->setMedia(filename);
}

// ===========================================================
void insertToArtistTable(const QString &artistName)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Artist (name) "
                  "VALUES (:artist)");
    query.bindValue(":artist", artistName);
    query.exec();
}

int getIdFromArtistTable(const QString &artistName)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM Artist "
                  "WHERE name = :artist");
    query.bindValue(":artist", artistName);
    if (query.exec() && query.first())
    {
        return query.value("id").toInt();
    }
    return -1;
}

void MainWindow::insertToTrackTable(const QString &title,
                                int artistId,
                                const QString &album,
                                int trackNum,
                                int year,
                                const QString &genre,
                                int duration,
                                const QString &location)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Track (title, artist_id, album, track_num, year, genre, duration, location) "
                  "VALUES (:title, :artist_id, :album, :track_num, :year, :genre, :duration, :location)");
    query.bindValue(":title", title);
    query.bindValue(":artist_id", artistId);
    query.bindValue(":album", album);
    query.bindValue(":track_num", trackNum);
    query.bindValue(":year", year);
    query.bindValue(":genre", genre);
    query.bindValue(":duration", duration);
    query.bindValue(":location", location);

    if (query.exec())
    {
        // Move this code out of here. Make insertToTrackTable a bool function, and then call the below from onAddMediaStatusChanged
        // if this function returns true.
        qDebug() << "Line 211: Succesful exec\n";
        //librarySourceModel->select();
        ui->player->updatePlaylistView();
    }
    else
    {
        qDebug() << "Line 211: Failed to exec\n";
        qDebug() << query.lastError();
    }
}

void MainWindow::onAddMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia)
    {
        // Can only get the metadata once the media has loaded
        QString location = mediaToBeAdded->currentMedia().canonicalUrl().toLocalFile();
        QString title = mediaToBeAdded->metaData(QMediaMetaData::Title).toString();
        if (title.isEmpty())
        {
            title = QFileInfo(location).completeBaseName();
        }
        QString artist = mediaToBeAdded->metaData(QMediaMetaData::Author).toString() + "";
        /* The + "" is necessary because if the metadata for the arguement passed to metaData() doesn't exist,
         * then using .toString() on the result of metaData() will result in a NULL string. The + "" will convert
         * this into an emptry string, which is what I need because I want an empty string to be a valid input (and
         * for NULL to be invalid input) for the name field of the Artist table in the media database and NULL input to be invalid.
        */

        QString album = mediaToBeAdded->metaData(QMediaMetaData::AlbumTitle).toString();
        int trackNum = mediaToBeAdded->metaData(QMediaMetaData::TrackNumber).toInt();
        int year = mediaToBeAdded->metaData(QMediaMetaData::Year).toInt();
        QString genre = mediaToBeAdded->metaData(QMediaMetaData::Genre).toString();
        int duration = mediaToBeAdded->metaData(QMediaMetaData::Duration).toInt();

        qDebug() << "Location: " << location;
        qDebug() << "Title: " << title;
        qDebug() << "Artist: " << artist;
        qDebug() << "Album: " << album;
        qDebug() << "Track Number: " << trackNum;
        qDebug() << "Year: " << year;
        qDebug() << "Genre: " << genre;
        qDebug() << "Duration: " << duration;

        insertToArtistTable(artist);
        int artistId = getIdFromArtistTable(artist);
        if (artistId != -1)
        {
            insertToTrackTable(title, artistId, album, trackNum, year, genre, duration, location);
        }
    }
}
// ===========================================================

void MainWindow::restoreWindowState()
{
    move(settings.value("MainWindow/position", pos()).toPoint());
    if (settings.value("MainWindow/isMaximized", true).toBool())
    {
        setWindowState(Qt::WindowState::WindowMaximized);
    }
    else
    {
        resize(settings.value("MainWindow/size", size()).toSize());
    }
}

void MainWindow::saveWindowState()
{
    settings.setValue("MainWindow/position", pos());
    settings.setValue("MainWindow/isMaximized", isMaximized());
    settings.setValue("MainWindow/size", size());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveWindowState();
    ui->player->close();
    event->accept();
}
