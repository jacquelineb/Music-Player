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
    QMediaPlayer::State state;

signals:
    void play();
    void pause();

public slots:
    void setControlsState(QMediaPlayer::State mediaState); // maybe this can just be changed to a public function?

private slots:
    void clickPlay();
};

#endif // PLAYERCONTROLS_H
