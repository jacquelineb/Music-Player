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
    // initialize the volume of mediaPlayer to whatever PlayerControls has the volumeSlider set to. mediaPlayer->setVolume(ui->controls->getVolume())
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Player::onStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &Player::onStateChanged);


    //connect(mediaPlayer, &QMediaPlayer::stateChanged, ui->controls, &PlayerControls::setControlsState); //do this from the controls class

    connect(ui->controls, &PlayerControls::playClicked, mediaPlayer, &QMediaPlayer::play);  // might have to edit this connection later.
                                                                                     // if user presses play when no media is set,
                                                                                     // then i'd want playback to just start from first song in library.

    connect(ui->controls, &PlayerControls::pauseClicked, mediaPlayer, &QMediaPlayer::pause);
    // connection for when control sends signal from pressing prev button
    // connection for when control send signal from pressing next button
    // connection for when control's volume slider changes.
    connect(ui->controls, &PlayerControls::volumeChanged, mediaPlayer, &QMediaPlayer::setVolume);

}

Player::~Player()
{
    delete ui;
    delete mediaPlayer; // Not sure if I actually need this
}


void Player::setMediaOfPlayer(QUrl filename)
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
