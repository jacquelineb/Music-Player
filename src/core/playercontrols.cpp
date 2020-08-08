#include "playercontrols.h"
#include "ui_playercontrols.h"

#include <QStyle>

PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
    setButtonsIcons();
    restoreVolumeSliderState();
    setConnections();
}


PlayerControls::~PlayerControls()
{
    delete ui;
}


void PlayerControls::setButtonsIcons()
{
    ui->playOrPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->prevButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
}


void PlayerControls::restoreVolumeSliderState()
{
    const int DEFAULT_VOLUME = 100;
    int volume = session.value("MediaPlayer/volume", DEFAULT_VOLUME).toInt();
    ui->volumeSlider->setValue(volume);
}


void PlayerControls::setConnections()
{
    connect(ui->playOrPauseButton, &QAbstractButton::clicked, this, &PlayerControls::playOrPauseClicked);
    connect(ui->prevButton, &QAbstractButton::clicked, this, &PlayerControls::prevClicked);
    connect(ui->nextButton, &QAbstractButton::clicked, this, &PlayerControls::nextClicked);
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, this, &PlayerControls::volumeChanged);
    connect(ui->progressSlider, &QAbstractSlider::sliderMoved, this, &PlayerControls::progressSliderMoved);
}


void PlayerControls::updateProgressSlider(int position)
{
    if (!ui->progressSlider->isSliderDown())
    {
        ui->progressSlider->setValue(position);
    }
}


void PlayerControls::setupProgressSlider(int mediaDurationInMillisec)
{
    ui->progressSlider->setMaximum(mediaDurationInMillisec);
}


void PlayerControls::updatePlaybackState(QMediaPlayer::State mediaState)
{
    if (mediaState == QMediaPlayer::State::PlayingState)
    {
        //ui->playOrPauseButton->setText("Pause");
        ui->playOrPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));

    }
    else
    {
        //ui->playOrPauseButton->setText("Play");
        ui->playOrPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}
