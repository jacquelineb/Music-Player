#include "playercontrols.h"
#include "ui_playercontrols.h"

PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
    restoreVolSliderState();
    connect(ui->playButton, &QAbstractButton::clicked, this, &PlayerControls::clickPlay);
    connect(ui->prevButton, &QAbstractButton::clicked, this, &PlayerControls::clickPrev);
    connect(ui->nextButton, &QAbstractButton::clicked, this, &PlayerControls::clickNext);
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, this, &PlayerControls::setVolume);
    connect(ui->progressSlider, &QAbstractSlider::sliderMoved, this, &PlayerControls::progressSliderMoved);
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

void PlayerControls::updateProgressSlider(int position)
{
    ui->progressSlider->setValue(position);
}


void PlayerControls::setupProgressSlider(int mediaDurationInMillisec)
{
    //qint64 mediaDurationInSec = mediaDurationInMillisec / 1000;
    //ui->progressSlider->setMaximum(mediaDurationInSec);
    ui->progressSlider->setMaximum(mediaDurationInMillisec);
}

void PlayerControls::restoreVolSliderState()
{
    // rename this to restoreControlSettings or initializeControlSettings
    // initialize PlayerControls::state in here instead of directly in constructor
    // initialize the progressSlider to be unchangeable or set the marker to be invisible. (maybe)
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

void PlayerControls::clickPlay()
{
    emit playOrPauseClicked();
}



void PlayerControls::clickPrev()
{
    emit prevClicked();
}


void PlayerControls::clickNext()
{
    emit nextClicked();
}
