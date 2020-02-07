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
    initializeLibraryPlaylist();
    initializeMediaPlayer();
    setUpConnections();

    // Change this later
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

    libraryViewModel = new LibraryPlaylistModel(this);
    libraryViewModel->setSourceModel(librarySourceModel);
    libraryViewModel->setHeaderData(0, Qt::Horizontal, tr("Track ID"));
    libraryViewModel->setHeaderData(1, Qt::Horizontal, tr("Title"));
    libraryViewModel->setHeaderData(2, Qt::Horizontal, tr("Artist"));
    libraryViewModel->setHeaderData(3, Qt::Horizontal, tr("Album"));
    libraryViewModel->setHeaderData(4, Qt::Horizontal, tr("Track Number"));
    libraryViewModel->setHeaderData(5, Qt::Horizontal, tr("Year"));
    libraryViewModel->setHeaderData(6, Qt::Horizontal, tr("Genre"));
    libraryViewModel->setHeaderData(7, Qt::Horizontal, tr("Duration"));
    libraryViewModel->setHeaderData(8, Qt::Horizontal, tr("Location"));
    libraryViewModel->setDynamicSortFilter(true);
}

void Player::initializeLibraryTreeView()
{
    ui->playlistTreeView->setModel(libraryViewModel);
    ui->playlistTreeView->sortByColumn(2, Qt::SortOrder::AscendingOrder);
    /*
    ui->playlistTreeView->setColumnHidden(0, true);
    ui->playlistTreeView->setColumnHidden(4, true);
    ui->playlistTreeView->setColumnHidden(5, true);
    ui->playlistTreeView->setColumnHidden(8, true);
    */
}

void Player::initializeLibraryPlaylist()
{
    playlist = new QMediaPlaylist(this);
    for (int i = 0; i < libraryViewModel->rowCount(); i++)
    {
        QModelIndex index = libraryViewModel->index(i, 8, QModelIndex());
        QString trackLocation = libraryViewModel->data(index).toString();
        playlist->addMedia(QUrl::fromLocalFile(trackLocation));
    }
}

void Player::initializeMediaPlayer()
{
    mediaPlayer = new QMediaPlayer(this);
    restorePlayerSettings();
    mediaPlayer->setPlaylist(playlist);
}

void Player::restorePlayerSettings()
{
    /* Restore volume, last playlist, etc. */
    mediaPlayer->setVolume(ui->controls->getVolume());
}

void Player::setUpConnections()
{
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setPlayButtonLabel); // maybe just rename setPlayButtonLabel to setControlState
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);
    connect(playlist, &QMediaPlaylist::currentMediaChanged, this, &Player::updateCurrTrackLabel);
    connect(playlist, &QMediaPlaylist::currentMediaChanged, this, &Player::updatePlaylistTreeViewSelection);
    connect(ui->playlistTreeView, &QTreeView::doubleClicked, this, &Player::playDoubleClickedTrack);
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &Player::playOrPauseMedia);
    connect(ui->controls, &PlayerControls::prevClicked, playlist, &QMediaPlaylist::previous);
    connect(ui->controls, &PlayerControls::nextClicked, playlist, &QMediaPlaylist::next);
}

void Player::playOrPauseMedia()
{
    qDebug() << "Playorpause";
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
            int highlightedRow = ui->playlistTreeView->currentIndex().row();
            if (highlightedRow != -1)
            {
                playlist->setCurrentIndex(highlightedRow);
            }
            else
            {
                playlist->setCurrentIndex(0);
            }
        }
        mediaPlayer->play();
    }
}

void Player::playDoubleClickedTrack(const QModelIndex &index)
{
    playlist->setCurrentIndex(index.row());
    mediaPlayer->play();
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

        // Update the playlist with the new track.
        QModelIndex srcIndexOfNewTrack = librarySourceModel->index(librarySourceModel->rowCount()-1, 0, QModelIndex());
        QModelIndex viewIndexOfNewTrack = libraryViewModel->mapFromSource(srcIndexOfNewTrack);
        playlist->insertMedia(viewIndexOfNewTrack.row(), QUrl::fromLocalFile(location));

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

void Player::onStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) // Once the media is loaded, i want it to start playing.
    {
        qDebug() << "Media has loaded";
        //mediaPlayer->play();
    }
    else if (status == QMediaPlayer::InvalidMedia)
    {
        qDebug() << "Line 282: " << status;
        qDebug() << "Error loading file";
        // error checking?
        // send a signal to be recieved by MainWindow about any errors (use mediaPlayer->error())
        // As example of when this status might occur is if a media file couldn't be located, eg., if I deleted an added song file from disk.
    }
    else if (status == QMediaPlayer::LoadingMedia)
    {
        qDebug() << "LOading media..";
    }
    else if (status == QMediaPlayer::BufferedMedia)
    {
        qDebug() << "Buffered Media...";
    }
    else if (status == QMediaPlayer::NoMedia)
    {
        //ui->currentTrackLabel->clear();
    }
    else if (status == QMediaPlayer::EndOfMedia)
    {
        qDebug() << status;
        qDebug() << "Maybe I should insert the songs into the playlist here";
        ui->currentTrackLabel->clear();
    }
    else
    {
        qDebug() << "Line 294: " << status;
    }
}

void Player::updateCurrTrackLabel()
{
    int currPlaylistIndex = playlist->currentIndex();
    if (currPlaylistIndex != -1)
    {
        QString currTrack = libraryViewModel->data(libraryViewModel->index(currPlaylistIndex, 1)).toString();
        QString currArtist = libraryViewModel->data(libraryViewModel->index(currPlaylistIndex, 2)).toString();
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
    int currPlaylistIndex = playlist->currentIndex();
    QModelIndex index = libraryViewModel->index(currPlaylistIndex, 0);
    ui->playlistTreeView->setCurrentIndex(index);
}
