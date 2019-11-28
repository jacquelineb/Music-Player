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
    int getVolume() { return volume; }

private:
    Ui::PlayerControls *ui;
    QMediaPlayer::State state; //  this is needed in order to change the functionality of the play button accordingly (i.e. should it play or pause)
    int volume;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};
    void restoreVolSliderState();
    void saveVolSliderState();
    void closeEvent(QCloseEvent *event);

signals:
    void playClicked();
    void pauseClicked();
    void volumeChanged(int value);

public slots:
    void setControlsState(QMediaPlayer::State mediaState); // maybe this can just be changed to a public function?

private slots:
    void clickPlay();
    void clickPrev();
    void setVolume(int volSliderValue);
};

#endif // PLAYERCONTROLS_H
