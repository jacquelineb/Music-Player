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
#include <QSortFilterProxyModel> // maybe delete this later

#include "libraryplaylistmodel.h" //

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    initializeMediaPlayer();
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &Player::onStateChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setPlayButtonLabel); // maybe just rename setPlayButtonLabel to setControlState
    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);

    initializeLibraryModel();
    initializeLibraryPlaylist();
    mediaPlayer->setPlaylist(playlist);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &Player::playOrPauseMedia);
    connect(ui->controls, &PlayerControls::prevClicked, playlist, &QMediaPlaylist::previous);
    connect(ui->controls, &PlayerControls::nextClicked, playlist, &QMediaPlaylist::next);
    //initializeLibraryTableView();
    connect(ui->playlistTableView, &QTableView::doubleClicked, this, &Player::playDoubleClickedTrack);

    // Change this later
    mediaToBeAdded = new QMediaPlayer(this);
    connect(mediaToBeAdded, &QMediaPlayer::mediaStatusChanged, this, &Player::onAddMediaStatusChanged);


    /*
    QSortFilterProxyModel *proxymodel = new QSortFilterProxyModel;
    proxymodel->setSourceModel(libraryModel);
    ui->playlistTableView->setModel(proxymodel);
    */


    LibraryPlaylistModel *sortmodel = new LibraryPlaylistModel(this);
    sortmodel->setSourceModel(libraryModel);
    //sortmodel->printRows();
    //sortmodel->sort(2, Qt::SortOrder::AscendingOrder);

/*
    QSortFilterProxyModel *sortmodel = new QSortFilterProxyModel(this);
    sortmodel->setSourceModel(libraryModel);
*/
    sortmodel->setDynamicSortFilter(true);
    sortmodel->sort(2);
    ui->playlistTableView->setModel(sortmodel);
    ui->playlistTableView->setSortingEnabled(true);
    //sortmodel->sort(2, Qt::SortOrder::AscendingOrder);

}


Player::~Player()
{
    delete ui;
    delete mediaPlayer; // Not sure if I actually need this
    delete mediaToBeAdded;
    destroyPlaylist();
    destroyLibraryModel();
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


void Player::initializeLibraryModel()
{
    libraryModel = new QSqlRelationalTableModel(this);
    libraryModel->setTable("Track");
    libraryModel->setRelation(2, QSqlRelation("Artist", "id", "name"));
    libraryModel->select();

    libraryModel->setHeaderData(0, Qt::Horizontal, tr("Track ID"));
    libraryModel->setHeaderData(1, Qt::Horizontal, tr("Title"));
    libraryModel->setHeaderData(2, Qt::Horizontal, tr("Artist"));
    libraryModel->setHeaderData(3, Qt::Horizontal, tr("Album"));
    libraryModel->setHeaderData(4, Qt::Horizontal, tr("Track Number"));
    libraryModel->setHeaderData(5, Qt::Horizontal, tr("Year"));
    libraryModel->setHeaderData(6, Qt::Horizontal, tr("Genre"));
    libraryModel->setHeaderData(7, Qt::Horizontal, tr("Duration"));
    libraryModel->setHeaderData(8, Qt::Horizontal, tr("Location"));
}


void Player::initializeLibraryPlaylist()
{
    playlist = new QMediaPlaylist(this);
    for (int i = 0; i < libraryModel->rowCount(); i++)
    {
        QString trackLocation = libraryModel->data(libraryModel->index(i,8)).toString();
        qDebug() << trackLocation;
        playlist->addMedia(QUrl(trackLocation));
    }
}


void Player::initializeLibraryTableView()
{
    ui->playlistTableView->setModel(libraryModel);
    ui->playlistTableView->setColumnHidden(0, true);
    ui->playlistTableView->setColumnHidden(4, true);
    ui->playlistTableView->setColumnHidden(5, true);
    ui->playlistTableView->setColumnHidden(8, true);
}


void Player::destroyLibraryModel()
{
    delete libraryModel;
}


void Player::destroyPlaylist()
{
    delete playlist;
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
            // If there is a song(s) highlighted, start playback from the first highlighted song.
            QModelIndexList highlightedTrackRows = ui->playlistTableView->selectionModel()->selectedRows();
            if (!highlightedTrackRows.isEmpty())
            {
                playlist->setCurrentIndex(highlightedTrackRows.first().row());
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
    query.exec();
    query.first();
    return query.value("id").toInt();
}


void insertToTrackTable(QString const& title, int artistId, QString const& album,
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
        qDebug() << "Line 211: Succesful exec\n";
    }
    else
    {
        qDebug() << "Line 211: Failed to exec\n";
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
        /*  The + "" is necessary because if the metadata for the arguement passed to metaData() doesn't exist,
         *  then using .toString() on the result of metaData() will result in a NULL string. The + "" will convert
         *  this into an emptry string, which is what I need because I want an empty string to be a valid input (and
         *  for NULL to be invalid input) for the name field of the Artist table in the media database and NULL input to be invalid.
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
        insertToTrackTable(title, artistId, album, trackNum, year, genre, duration, location);
    }
}


void Player::onStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) // Once the media is loaded, i want it to start playing.
    {
        qDebug() << "Media has loaded";
        /*
        qDebug() << "status: " << mediaPlayer->mediaStatus();
        qDebug() << "audio availability: " << mediaPlayer->isAudioAvailable(); // this returns false for songs that don't play. figure out why the audio for these songs is unavailable.
        qDebug() << "duration: " << mediaPlayer->duration();
        qDebug() << "audio role: " << mediaPlayer->audioRole();
        qDebug() << "buffer status: " << mediaPlayer->bufferStatus();
        qDebug() << "error: " << mediaPlayer->error();
        qDebug() << "volume: " << mediaPlayer->volume();
        qDebug() << "playback rate: " << mediaPlayer->playbackRate();
        qDebug() << "availability: " << mediaPlayer->availability();
        */

        //mediaPlayer->play();
    }
    else if (status == QMediaPlayer::InvalidMedia || status == QMediaPlayer::UnknownMediaStatus)
    {
        // error checking?
        // send a signal to be recieved by MainWindow about any errors (use mediaPlayer->error())
    }
}


void Player::onStateChanged(QMediaPlayer::State state)
{

    qDebug() << "state: " << mediaPlayer->state();
    if (state == QMediaPlayer::State::PlayingState)
    {
        ui->currentTrackLabel->setText("Now Playing: " + mediaPlayer->metaData(QMediaMetaData::Title).toString());
        // The above might not always work because a file might not have the meta data for Title available, so an empty string might be getting printed.
        // Instead, just get the artist and title by getting the current index in the playlist and then getting the data from this index in the playlist model.
    }
    // else if it is in the paused state... do something
    // else it is in stopped state.... do something
    else if (state == QMediaPlayer::State::StoppedState)
    {
        ui->currentTrackLabel->setText("Play something?");
    }

}
