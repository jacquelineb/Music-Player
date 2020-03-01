#include "playercontrols.h"
#include "ui_playercontrols.h"

PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
    restoreVolumeSliderState();
    setConnections();
}

PlayerControls::~PlayerControls()
{
    delete ui;
}

void PlayerControls::restoreVolumeSliderState()
{
    // rename this to restoreControlSettings or initializeControlSettings
    // initialize PlayerControls::state in here instead of directly in constructor
    //ui->volumeSlider->setValue(settings.value("PlayerControls/volumeSlider", ui->volumeSlider->maximum()).toInt());

    const int DEFAULT_VOLUME = 100;
    int volume = settings.value("MediaPlayer/volume", DEFAULT_VOLUME).toInt();
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
        ui->playOrPauseButton->setText("Pause");
    }
    else
    {
        ui->playOrPauseButton->setText("Play");
    }
}
