#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QSettings>
#include <QSqlRelationalTableModel>

#include "libraryplaylistmodel.h"

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
    QSqlRelationalTableModel *librarySourceModel = nullptr;
    LibraryPlaylistModel *libraryViewModel = nullptr;
    QMediaPlaylist *playlist = nullptr;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};
    qint64 position_ = 0;
    void closeEvent(QCloseEvent *event);
    void initializeMediaPlayer();
    void restorePlayerSettings();
    void savePlayerSettings();
    void initializeLibraryModels();
    void destroyLibraryModels();
    void initializeLibraryPlaylist();
    void destroyPlaylist();
    void initializeLibraryTableView();

private slots:
    void onStatusChanged(QMediaPlayer::MediaStatus status);
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onStateChanged(QMediaPlayer::State state);
    void playDoubleClickedTrack(const QModelIndex &index);
    void playOrPauseMedia();

    void headerClicked(int index);
};

#endif // PLAYER_H
