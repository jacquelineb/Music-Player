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
    QSqlRelationalTableModel *librarySourceModel = nullptr;
    LibraryPlaylistModel *libraryViewModel = nullptr;
    QMediaPlayer *mediaPlayer = nullptr;
    QMediaPlayer *mediaToBeAdded = nullptr;
    QMediaPlaylist *playlist = nullptr;
    qint64 position_ = 0;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};

    void closeEvent(QCloseEvent *event);
    void initializeLibraryModels();
    void initializeLibraryPlaylist();
    void initializeLibraryTreeView();
    void initializeMediaPlayer();
    void savePlayerSettings();
    void setUpConnections();
    void restorePlayerSettings();

    void insertToTrackTable(QString const& title, int artistId, QString const& album,
                        int trackNum, int year, QString const& genre, int duration,
                        QString const& location);

private slots:
    void onStatusChanged(QMediaPlayer::MediaStatus status);
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onStateChanged(QMediaPlayer::State state);
    void playDoubleClickedTrack(const QModelIndex &index);
    void playOrPauseMedia();


    void playlistIndexChanged(int position);
};

#endif // PLAYER_H
