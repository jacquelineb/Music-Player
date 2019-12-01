#include "playercontrols.h"
#include "ui_playercontrols.h"

PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
    state = QMediaPlayer::State::StoppedState;
    restoreVolSliderState();
    connect(ui->playButton, &QAbstractButton::clicked, this, &PlayerControls::clickPlay);
    connect(ui->prevButton, &QAbstractButton::clicked, this, &PlayerControls::clickPrev);
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, this, &PlayerControls::setVolume);
}

PlayerControls::~PlayerControls()
{
    delete ui;
}


void PlayerControls::closeEvent(QCloseEvent *event)
{
    saveVolSliderState();
    event->accept();
}


void PlayerControls::restoreVolSliderState()
{
    ui->volumeSlider->setValue(settings.value("PlayerControls/volumeSlider", ui->volumeSlider->maximum()).toInt());
    setVolume(ui->volumeSlider->value());
}


void PlayerControls::setVolume(int volSliderValue)
{
    qDebug() << "Vol value: " << volSliderValue;
    qreal linearVolume = QAudio::convertVolume(volSliderValue / qreal(100.0),
                                               QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);
    volume = qRound(linearVolume * 100);
    emit volumeChanged(volume);
}


void PlayerControls::saveVolSliderState()
{
    settings.setValue("PlayerControls/volumeSlider", ui->volumeSlider->value());
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
