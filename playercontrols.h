#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>
#include <QMediaPlayer>
#include <QCloseEvent>
#include <QSettings>


namespace Ui {
class PlayerControls;
}

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerControls(QWidget *parent = nullptr);
    ~PlayerControls();
    int getVolume() const { return volume; }

private:
    Ui::PlayerControls *ui;
    int volume;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};

    void closeEvent(QCloseEvent *event);
    void restoreVolumeSliderState();
    void saveVolumeSliderState();
    void setConnections();

signals:
    void nextClicked();
    void pauseClicked();
    void playOrPauseClicked();
    void prevClicked();
    void progressSliderMoved(int value);
    void volumeChanged(int value);

public slots:
    void updatePlaybackState(QMediaPlayer::State mediaState);
    void setupProgressSlider(int mediaDurationInMillisec);
    void updateProgressSlider(int position);

private slots:
    void setVolume(int volumeSliderValue);
};

#endif // PLAYERCONTROLS_H
