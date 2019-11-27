#include "playercontrols.h"
#include "ui_playercontrols.h"


PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
    // make sure to initialize state to Stopped
    state = QMediaPlayer::State::StoppedState;
    connect(ui->playButton, &QAbstractButton::clicked, this, &PlayerControls::clickPlay);
}

PlayerControls::~PlayerControls()
{
    delete ui;
}

void PlayerControls::clickPlay()
{
    /*
    //ui->playButton->setText("you clicked play");
     check the  internal state.
     if media playing, then clicking the play button should cause the Player to pause, i.e., emit a pause signal. this should be recieved in the Player class.
     if media is paused, then clicking play should emit a play signal (to be recieved in Player)
     if media state is stopped...
    */
    /* what if user presses the buttons without the mediaplayer being loaded or right off the bat when starting the program? */
    if (state == QMediaPlayer::State::PlayingState)
    {
        emit pause();
    }
    else if (state == QMediaPlayer::State::PausedState)
    {
        emit play();
    }
    else // StoppedState
    {

    }
}


void PlayerControls::setControlsState(QMediaPlayer::State mediaState)
{
    state = mediaState;
    qDebug() << "From PlayerControls::setControlState" << state;
    if (state == QMediaPlayer::State::PlayingState)
    {
        ui->playButton->setText("||");
        // set the icon to pause symbol. look up QStyle and QIcon. for now i'm just going to setText.
    }
}
