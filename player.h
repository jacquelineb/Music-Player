#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QSettings>
#include <QSqlRelationalTableModel>

namespace Ui {
class Player;
}

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();
    void addToLibrary(QUrl filename);

private:
    Ui::Player *ui;
    QMediaPlayer *mediaPlayer = nullptr;
    QMediaPlayer *mediaToBeAdded = nullptr;
    QSqlRelationalTableModel *libraryModel = nullptr;
    QMediaPlaylist *playlist = nullptr;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};
    qint64 position_ = 0;
    void closeEvent(QCloseEvent *event);
    void initializeMediaPlayer();
    void restorePlayerSettings();
    void savePlayerSettings();
    void initializeLibraryModel();
    void destroyLibraryModel();
    void initializeLibraryPlaylist();
    void destroyPlaylist();
    void initializeLibraryTableView();

private slots:
    void onStatusChanged(QMediaPlayer::MediaStatus status);
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onStateChanged(QMediaPlayer::State state);
    void playDoubleClickedTrack(const QModelIndex &index);
    void playOrPauseMedia();
};

#endif // PLAYER_H
