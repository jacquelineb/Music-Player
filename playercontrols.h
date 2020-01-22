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
    void restoreVolSliderState();
    void saveVolSliderState();
    void closeEvent(QCloseEvent *event);

signals:
    void playOrPauseClicked();
    void pauseClicked();
    void volumeChanged(int value);
    void progressSliderMoved(int value);

public slots:
    void setupProgressSlider(int mediaDurationInMillisec);
    void updateProgressSlider(int position);
    void setPlayButtonLabel(QMediaPlayer::State mediaState);

private slots:
    void clickPlay();
    void clickPrev();
    void setVolume(int volSliderValue);
};

#endif // PLAYERCONTROLS_H
