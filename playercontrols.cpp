#include "playercontrols.h"
#include "ui_playercontrols.h"


PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
    state = QMediaPlayer::State::StoppedState;
    // read from a json file to set the position of the volume slider and to set the volume itself. for now just set the slider to its maximum
    ui->volumeSlider->setSliderPosition(ui->volumeSlider->maximum());
    // look here https://doc.qt.io/qt-5/qmediaplayer.html#volume-prop and here https://doc.qt.io/qt-5/qaudio.html#convertVolume
    connect(ui->playButton, &QAbstractButton::clicked, this, &PlayerControls::clickPlay);
    connect(ui->prevButton, &QAbstractButton::clicked, this, &PlayerControls::clickPrev);
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, this, &PlayerControls::setVolume);
}

PlayerControls::~PlayerControls()
{
    // save volume state to json
    delete ui;
}


void PlayerControls::setVolume(int value)
{
    qDebug() << "Vol value: " << value;
    // take a look at the following links to set value
    // https://doc.qt.io/qt-5/qmediaplayer.html#volume-prop, https://doc.qt.io/qt-5/qaudio.html#convertVolume
    emit volumeChanged(value);

}

void PlayerControls::clickPlay()
{
    /*
     check the  internal state.
     if media playing, then clicking the play button should cause the Player to pause, i.e., emit a pause signal. this should be recieved in the Player class.
     if media is paused, then clicking play should emit a play signal (to be recieved in Player)
     if media state is stopped...
    */
    /* what if user presses the buttons without the mediaplayer being loaded or right off the bat when starting the program? */
    if (state == QMediaPlayer::State::PlayingState)
    {
        emit pauseClicked();
    }
    else if (state == QMediaPlayer::State::PausedState)
    {
        emit playClicked();
    }
    else // StoppedState
    {
        // emit a signal to be received in Player class. The Player class should just start playing whatever track is currently highlighted/selected.
        //                                               if none are selected, just start playing the first song in the playlist.
    }
}

void PlayerControls::clickPrev()
{
    // Play previous song in the playlist
}

/*
void PlayerControls::setVolumeSliderPosition()
{
    qreal volume = QAudio::convertVolume()
}
*/

void PlayerControls::setControlsState(QMediaPlayer::State mediaState)
{
    state = mediaState;
    qDebug() << "From PlayerControls::setControlState" << state;
    if (state == QMediaPlayer::State::PlayingState)
    {
        ui->playButton->setText("||");
        // set the icon to pause symbol. look up QStyle and QIcon. for now i'm just going to setText.
        // should this label change be done in here or in clickPlay()
    }
}
