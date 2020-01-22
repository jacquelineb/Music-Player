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

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    mediaPlayer = new QMediaPlayer(this);
    restorePlayerSettings();
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &Player::onStateChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setPlayButtonLabel);

    //connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setControlsState); //do this from the controls class

    //connect(ui->controls, &PlayerControls::playClicked, mediaPlayer, &QMediaPlayer::play);  // might have to edit this connection later.
                                                                                     // if user presses play when no media is set,
                                                                                     // then i'd want playback to just start from first song in library.

    //connect(ui->controls, &PlayerControls::pauseClicked, mediaPlayer, &QMediaPlayer::pause);
    connect(ui->controls, &PlayerControls::playOrPauseClicked, this, &Player::playOrPauseMedia);

    // connection for when control sends signal from pressing prev button
    // connection for when control send signal from pressing next button
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);

    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);

    mediaToBeAdded = new QMediaPlayer(this);
    connect(mediaToBeAdded, &QMediaPlayer::mediaStatusChanged, this, &Player::onAddMediaStatusChanged);

    // Set up the playlistModel
    playlistModel = new QSqlRelationalTableModel(this);
    playlistModel->setTable("Track");
    playlistModel->setRelation(2, QSqlRelation("Artist", "id", "name"));
    playlistModel->select();

    playlistModel->setHeaderData(0, Qt::Horizontal, tr("Track ID"));
    playlistModel->setHeaderData(1, Qt::Horizontal, tr("Title"));
    playlistModel->setHeaderData(2, Qt::Horizontal, tr("Artist"));
    playlistModel->setHeaderData(3, Qt::Horizontal, tr("Album"));
    playlistModel->setHeaderData(4, Qt::Horizontal, tr("Track Number"));
    playlistModel->setHeaderData(5, Qt::Horizontal, tr("Year"));
    playlistModel->setHeaderData(6, Qt::Horizontal, tr("Genre"));
    playlistModel->setHeaderData(7, Qt::Horizontal, tr("Duration"));
    playlistModel->setHeaderData(8, Qt::Horizontal, tr("Location"));

    playlist = new QMediaPlaylist(this);
    for (int i = 0; i < playlistModel->rowCount(); i++)
    {
        QString trackLocation = playlistModel->data(playlistModel->index(i,8)).toString();
        qDebug() << trackLocation;
        playlist->addMedia(QUrl(trackLocation));
    }
    mediaPlayer->setPlaylist(playlist);

    ui->playlistTableView->setModel(playlistModel);
    ui->playlistTableView->setColumnHidden(0, true);
    ui->playlistTableView->setColumnHidden(4, true);
    ui->playlistTableView->setColumnHidden(5, true);
    ui->playlistTableView->setColumnHidden(8, true);

    connect(ui->playlistTableView, &QTableView::doubleClicked, this, &Player::playSelected);
}


void Player::playOrPauseMedia()
{
    if (mediaPlayer->state() == QMediaPlayer::State::PausedState)
    {
        mediaPlayer->play();
    }
    else if (mediaPlayer->state() == QMediaPlayer::State::PlayingState)
    {
        mediaPlayer->pause();
    }
    else // StoppedState
    {
        qDebug() << "Line 92. Pressing play from Stopped State. See comments";
        // If there is a song(s) highlighted, start playback from the first highlighted song.
        // If not, then playback will just start from first song in playlist.
        QModelIndexList highlightedTrackRows = ui->playlistTableView->selectionModel()->selectedRows();
        if (!highlightedTrackRows.isEmpty())
        {
            playlist->setCurrentIndex(highlightedTrackRows.first().row());
        }
        mediaPlayer->play();
    }
}


void Player::playSelected(const QModelIndex &index)
{
    qDebug() << index.row();
    // Start playing the song at this index
    playlist->setCurrentIndex(index.row());
    mediaPlayer->play();
}


Player::~Player()
{
    delete ui;
    delete mediaPlayer; // Not sure if I actually need this
    delete mediaToBeAdded;
}


void Player::restorePlayerSettings()
{
    /* Restore volume, last playlist, etc. */
    mediaPlayer->setVolume(ui->controls->getVolume());
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


void Player::setMediaOfPlayer(QUrl filename)
{
    mediaPlayer->setMedia(filename); // make sure to set up a connection (see Qt's doc for QMediaPlayer::setMedia)
    qDebug() << mediaPlayer->mediaStatus();
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
    query.exec();

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
