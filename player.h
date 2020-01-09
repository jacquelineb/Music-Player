#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QSettings>

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
    QMediaPlayer *mediaToBeAdded = nullptr;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};
    qint64 position_ = 0;
    void closeEvent(QCloseEvent *event);
    void restorePlayerSettings();
    void savePlayerSettings();

private slots:
    void onStatusChanged(QMediaPlayer::MediaStatus status);
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onStateChanged(QMediaPlayer::State state);
};

#endif // PLAYER_H
