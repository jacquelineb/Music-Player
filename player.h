#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
class Player;
}

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();
    void setMediaOfPlayer(QUrl filename);
    void addToLibrary(QUrl filename);

private:
    Ui::Player *ui;
    QMediaPlayer *mediaPlayer = nullptr;

private slots:
    void onStatusChanged(QMediaPlayer::MediaStatus status);
    void onStateChanged(QMediaPlayer::State state);
};

#endif // PLAYER_H
