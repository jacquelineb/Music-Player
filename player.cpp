#include "player.h"
#include "ui_player.h"

#include <QDebug>
#include <QAbstractButton>
#include <QCloseEvent>
#include <QFileInfo>
#include <QMediaMetaData>
#include <QTime>

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioRole(QAudio::Role::MusicRole); // this should actually change based on media type
    restorePlayerSettings();
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &Player::onStateChanged);


    //connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setControlsState); //do this from the controls class

    connect(ui->controls, &PlayerControls::playClicked, mediaPlayer, &QMediaPlayer::play);  // might have to edit this connection later.
                                                                                     // if user presses play when no media is set,
                                                                                     // then i'd want playback to just start from first song in library.

    connect(ui->controls, &PlayerControls::pauseClicked, mediaPlayer, &QMediaPlayer::pause);
    // connection for when control sends signal from pressing prev button
    // connection for when control send signal from pressing next button
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);
    connect(ui->controls, &PlayerControls::progressSliderMoved, mediaPlayer, &QMediaPlayer::setPosition);

    connect(mediaPlayer, &QMediaPlayer::durationChanged, ui->controls, &PlayerControls::setupProgressSlider);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, ui->controls, &PlayerControls::updateProgressSlider);

    addMedia = new QMediaPlayer(this);
    connect(addMedia, &QMediaPlayer::mediaStatusChanged, this, &Player::onAddMediaStatusChanged);


}


Player::~Player()
{
    delete ui;
    delete mediaPlayer; // Not sure if I actually need this
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
    addMedia->setMedia(filename);
}

void Player::onAddMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
   if (status == QMediaPlayer::LoadedMedia)
   {
        QString location = addMedia->currentMedia().canonicalUrl().toLocalFile();
        QString title = addMedia->metaData(QMediaMetaData::Title).toString();
        if (title == "")
        {
            title = QFileInfo::QFileInfo(location).completeBaseName();
        }
        QString artist = addMedia->metaData(QMediaMetaData::Author).toString();
        QString album = addMedia->metaData(QMediaMetaData::AlbumTitle).toString();
        quint64 trackNum = addMedia->metaData(QMediaMetaData::TrackNumber).toUInt();
        quint64 year = addMedia->metaData(QMediaMetaData::Year).toUInt();
        QString genre = addMedia->metaData(QMediaMetaData::Genre).toString();
        quint64 duration = addMedia->metaData(QMediaMetaData::Duration).toUInt();

        qDebug() << "Location: " << location;
        qDebug() << "Title: " << title;
        qDebug() << "Artist: " << artist;
        qDebug() << "Album: " << album;
        qDebug() << "Track Number: " << trackNum;
        qDebug() << "Year: " << year;
        qDebug() << "Genre: " << genre;
        qDebug() << "Duration: " << duration;

        // TODO: check the media database's Artist table for the artist. If found, get the artist id, else add this new artist to the Artist table,
        // and get its id.
        // Then insert the song into the Song table with the title, artist id, album, trackNum, year, genre, length, and location data.
   }
}


void Player::onStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) // Once the media is loaded, i want it to start playing.
    {
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
        mediaPlayer->play();
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
    // set state of PlayerControls to be the same state.
    ui->controls->setControlsState(state);
    if (state == QMediaPlayer::State::PlayingState)
    {
        ui->currentTrackLabel->setText("Now Playing: " + mediaPlayer->metaData(QMediaMetaData::Title).toString()); // read the data from a json, instead of directly using the metadata?
        // other stuff
    }
    // else if it is in the paused state... do something
    // else it is in stopped state.... do something
    else if (state == QMediaPlayer::State::StoppedState)
    {
        ui->currentTrackLabel->setText("Play something");
    }

}
