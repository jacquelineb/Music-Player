#include "player.h"
#include "ui_player.h"

#include <QDebug>
#include <QAbstractButton>
#include <QCloseEvent>
#include <QFileInfo>
#include <QMediaMetaData>
#include <QTime>
#include <QSqlQuery>
#include <QSqlError> // maybe delete this later
#include <QMediaPlaylist>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlIndex> //

#include "libraryplaylistmodel.h" //

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    initializeLibraryModels();
    initializeLibraryTreeView();
    initializeMediaPlayer();
    setUpConnections();

    // Change this later. try taglib
    mediaToBeAdded = new QMediaPlayer(this);
    connect(mediaToBeAdded, &QMediaPlayer::mediaStatusChanged, this, &Player::onAddMediaStatusChanged);
}

Player::~Player()
{
    delete ui;
}

void Player::initializeLibraryModels()
{
    librarySourceModel = new QSqlRelationalTableModel(this);
    librarySourceModel->setTable("Track");
    librarySourceModel->setRelation(2, QSqlRelation("Artist", "id", "name"));
    librarySourceModel->select();

    libraryProxyModel = new LibraryPlaylistModel(this);
    libraryProxyModel->setSourceModel(librarySourceModel);
    libraryProxyModel->setHeaderData(0, Qt::Horizontal, tr("Track ID"));
    libraryProxyModel->setHeaderData(1, Qt::Horizontal, tr("Title"));
    libraryProxyModel->setHeaderData(2, Qt::Horizontal, tr("Artist"));
    libraryProxyModel->setHeaderData(3, Qt::Horizontal, tr("Album"));
    libraryProxyModel->setHeaderData(4, Qt::Horizontal, tr("Track Number"));
    libraryProxyModel->setHeaderData(5, Qt::Horizontal, tr("Year"));
    libraryProxyModel->setHeaderData(6, Qt::Horizontal, tr("Genre"));
    libraryProxyModel->setHeaderData(7, Qt::Horizontal, tr("Duration"));
    libraryProxyModel->setHeaderData(8, Qt::Horizontal, tr("Location"));
    libraryProxyModel->setDynamicSortFilter(true);
}

void Player::initializeLibraryTreeView()
{
    ui->playlistView->setModel(libraryProxyModel);
    ui->playlistView->sortByColumn(2, Qt::SortOrder::AscendingOrder);

    //ui->playlistView->setColumnHidden(0, true);
    //ui->playlistView->setColumnHidden(4, true);
    //ui->playlistView->setColumnHidden(5, true);
    //ui->playlistView->setColumnHidden(8, true);
}

void Player::initializeMediaPlayer()
{
    mediaPlayer = new QMediaPlayer(this);
    restorePlayerSettings();
}

void Player::restorePlayerSettings()
{
    /* Restore volume, last playlist, etc. */
    mediaPlayer->setVolume(ui->controls->getVolume());
}

void Player::setUpConnections()
{
    connect(ui->playlistView, &QTreeView::doubleClicked, this, &Player::setMediaForPlayback);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &Player::updateCurrTrackLabel);
    connect(mediaPlayer, &QMediaPlayer::currentMediaChanged, this, &Player::updatePlaylistTreeViewSelection); // Check difference between QMediaPlayer's mediChanged and currentMediaChanged signals
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setPlayButtonLabel); // maybe just rename setPlayButtonLabel to setControlState
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);

    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &Player::playOrPauseMedia);
    connect(ui->controls, &PlayerControls::nextClicked, this, &Player::setNextMediaForPlayback);
    connect(ui->controls, &PlayerControls::prevClicked, this, &Player::setPreviousMediaForPlayback);
}

void Player::setMediaForPlayback(const QModelIndex &selectedIndex)
{
    if (selectedIndex.isValid())
    {
        srcIndexOfCurrMedia = libraryProxyModel->mapToSource(selectedIndex);
        QUrl trackLocation = libraryProxyModel->data(libraryProxyModel->index(selectedIndex.row(), 8, QModelIndex())).toUrl();
        mediaPlayer->setMedia(trackLocation);
    }
    else
    {
        srcIndexOfCurrMedia = QModelIndex();
        mediaPlayer->setMedia(QMediaContent());
    }
}

void Player::onStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia)
    {
        playLoadedMedia();
    }
    else if (status == QMediaPlayer::EndOfMedia)
    {
        setNextMediaForPlayback();
    }
    else if (status == QMediaPlayer::InvalidMedia)
    {
        qDebug() << "Line 282: " << status;
        qDebug() << "Error loading file";
        // error checking?
        // send a signal to be recieved by MainWindow about any errors (use mediaPlayer->error())
        // As example of when this status might occur is if a media file couldn't be located, eg., if I deleted an added song file from disk.
    }
    else
    {
        qDebug() << "Line 294: " << status;
    }
}

void Player::playLoadedMedia()
{
    mediaPlayer->play();
}

void Player::setNextMediaForPlayback()
{
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    QModelIndex next = proxyIndexOfCurrMedia.siblingAtRow(proxyIndexOfCurrMedia.row()+1);
    setMediaForPlayback(next);
}

void Player::setPreviousMediaForPlayback()
{
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    QModelIndex previous = proxyIndexOfCurrMedia.siblingAtRow(proxyIndexOfCurrMedia.row()-1);
    setMediaForPlayback(previous);
}

void Player::updateCurrTrackLabel()
{
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    if (proxyIndexOfCurrMedia.isValid())
    {
        int currRow = proxyIndexOfCurrMedia.row();
        QString currTrack = libraryProxyModel->data(libraryProxyModel->index(currRow, 1)).toString();
        QString currArtist = libraryProxyModel->data(libraryProxyModel->index(currRow, 2)).toString();
        qDebug() << currTrack << " " << currArtist;
        QString currentlyPlayingText = "Now Playing: ";
        if (!currArtist.isEmpty())
        {
            currentlyPlayingText += currArtist + " - ";
        }
        currentlyPlayingText += currTrack;
        ui->currentTrackLabel->setText(currentlyPlayingText);
    }
    else
    {
        ui->currentTrackLabel->clear();
    }
}

void Player::updatePlaylistTreeViewSelection()
{
    // Automatically highlight the currently playing track in the playlist view.
    QModelIndex proxyIndexOfCurrMedia = libraryProxyModel->mapFromSource(srcIndexOfCurrMedia);
    ui->playlistView->setCurrentIndex(proxyIndexOfCurrMedia);
}

void Player::playOrPauseMedia()
{
    if (mediaPlayer->state() == QMediaPlayer::State::PlayingState)
    {
        mediaPlayer->pause();
    }
    else // Pause or Stopped State
    {
        if (mediaPlayer->state() == QMediaPlayer::State::StoppedState)
        {
            /* If pressing play button from a stopped state,
             * start playback from the first highlighted song if there is one.
             * Otherwise just start playback from the beginning of playlist.
            */
            QModelIndex selectedIndex = ui->playlistView->currentIndex();
            if (selectedIndex.isValid())
            {
                setMediaForPlayback(selectedIndex);
            }
            else
            {
                setMediaForPlayback(libraryProxyModel->index(0, 0, QModelIndex()));
            }
        }
        mediaPlayer->play();
    }
}

void Player::savePlayerSettings()
{
    /* Save which playlist was last opened */
}

void Player::closeEvent(QCloseEvent *event)
{
    savePlayerSettings();
    ui->controls->close();
    event->accept();
}

void Player::addToLibrary(QUrl filename)
{
    //https://stackoverflow.com/questions/23678748/qtcreator-qmediaplayer-meta-data-returns-blank-qstring
    // above explains why I chose to use a separate QMediaPlayer (addMedia) for adding media to the library.
    mediaToBeAdded->setMedia(filename);
}

void insertToArtistTable(QString const& artistName)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Artist (name) "
                  "VALUES (:artist)");
    query.bindValue(":artist", artistName);
    query.exec();
}

int getIdFromArtistTable(QString const& artistName)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM Artist "
                  "WHERE name = :artist");
    query.bindValue(":artist", artistName); // exec() overwrites the placeholder with data, so rebinding :artist to artist is necessary.
    if (query.exec() && query.first())
    {
        return query.value("id").toInt();
    }
    return -1;
}

void Player::insertToTrackTable(QString const& title, int artistId, QString const& album,
                                int trackNum, int year, QString const& genre, int duration,
                                QString const& location)
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
        librarySourceModel->select();

        /* There is currently a bug where if a song in the playlist is already playing and the user decides to add
         * a new song to the library, if the new song was inserted into the playlist at some index before the current song,
         * playback will start at the beginning of the playlist. If the new song was inserted into the playlist somewhere
         * after the currently playing song, playback will continue as normal.
        */
    }
    else
    {
        qDebug() << "Line 211: Failed to exec\n";
        qDebug() << query.lastError();
    }
}

void Player::onAddMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
   if (status == QMediaPlayer::LoadedMedia)
   {
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

