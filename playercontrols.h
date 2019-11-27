#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
class PlayerControls;
}

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerControls(QWidget *parent = nullptr);
    ~PlayerControls();

private:
    Ui::PlayerControls *ui;
    QMediaPlayer::State state; //  this is needed in order to change the functionality of the play button accordingly (i.e. should it play or pause)

signals:
    void playClicked();
    void pauseClicked();
    void volumeChanged(int value);

public slots:
    void setControlsState(QMediaPlayer::State mediaState); // maybe this can just be changed to a public function?

private slots:
    void clickPlay();
    void clickPrev();
    void setVolume(int value);
};

#endif // PLAYERCONTROLS_H
