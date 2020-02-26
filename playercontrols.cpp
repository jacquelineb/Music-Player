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
    ui->volumeSlider->setValue(settings.value("PlayerControls/volumeSlider", ui->volumeSlider->maximum()).toInt());
    setVolume(ui->volumeSlider->value());
}

void PlayerControls::setConnections()
{
    connect(ui->playButton, &QAbstractButton::clicked, this, &PlayerControls::playOrPauseClicked);
    connect(ui->prevButton, &QAbstractButton::clicked, this, &PlayerControls::prevClicked);
    connect(ui->nextButton, &QAbstractButton::clicked, this, &PlayerControls::nextClicked);
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, this, &PlayerControls::setVolume);
    connect(ui->progressSlider, &QAbstractSlider::sliderMoved, this, &PlayerControls::progressSliderMoved);
}

void PlayerControls::setVolume(int volumeSliderValue)
{
    //qreal linearVolume = QAudio::convertVolume(volumeSliderValue / qreal(100.0),
    //                                           QAudio::LogarithmicVolumeScale,
    //                                           QAudio::LinearVolumeScale);
    //volume = qRound(linearVolume * 100);
    //emit volumeChanged(volume);

    volume = volumeSliderValue;
    emit volumeChanged(volume);
}

void PlayerControls::closeEvent(QCloseEvent *event)
{
    saveVolumeSliderState();
    event->accept();
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

void PlayerControls::saveVolumeSliderState()
{
    settings.setValue("PlayerControls/volumeSlider", ui->volumeSlider->value());
}

void PlayerControls::setPlayButtonLabel(QMediaPlayer::State mediaState)
{
    if (mediaState == QMediaPlayer::State::PlayingState)
    {
        ui->playButton->setText("Pause");
    }
    else
    {
        ui->playButton->setText("Play");
    }
}
