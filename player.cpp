#include "player.h"
#include "ui_player.h"

#include <QDebug>
#include <QAbstractButton>
#include <QMediaMetaData>

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    mediaPlayer = new QMediaPlayer(this);
    //mediaPlayer->setAudioRole(QAudio::MusicRole);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &Player::onStateChanged);


    //connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setControlsState); //do this from the controls class

    connect(ui->controls, &PlayerControls::play, mediaPlayer, &QMediaPlayer::play);
    connect(ui->controls, &PlayerControls::pause, mediaPlayer, &QMediaPlayer::pause);
}

Player::~Player()
{
    delete ui;
}


void Player::setMediaFile(QUrl filename)
{
    mediaPlayer->setMedia(filename); // make sure to set up a connection (see Qt's doc for QMediaPlayer::setMedia)
    qDebug() << mediaPlayer->mediaStatus();
}

void Player::addToLibrary(QUrl filename)
{

}

void Player::onStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) // Once the media is loaded, i want it to start playing.
    {
        qDebug() << "status: " << mediaPlayer->mediaStatus();
        qDebug() << "audio availability: " << mediaPlayer->isAudioAvailable(); // this returns false for songs that don't play. figure out why the audio for these songs is unavailable.
        qDebug() << "duration: " << mediaPlayer->duration();
        qDebug() << "audio role: " << mediaPlayer->audioRole();
        qDebug() << "buffer status: " << mediaPlayer->bufferStatus();
        qDebug() << "error: " << mediaPlayer->error();
        qDebug() << "volume: " << mediaPlayer->volume();
        qDebug() << "playback rate: " << mediaPlayer->playbackRate();
        qDebug() << "availability: " << mediaPlayer->availability();
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
