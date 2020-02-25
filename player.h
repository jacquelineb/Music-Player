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
    LibraryPlaylistModel *libraryProxyModel = nullptr;
    QMediaPlayer *mediaPlayer = nullptr;
    QMediaPlayer *mediaToBeAdded = nullptr;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};
    QModelIndex srcIndexOfCurrMedia;

    void closeEvent(QCloseEvent *event);
    void initializeLibraryModels();
    void initializeLibraryTreeView();
    void initializeMediaPlayer();
    void insertToTrackTable(const QString &title,
                            int artistId,
                            const QString &album,
                            int trackNum,
                            int year,
                            const QString &genre,
                            int duration,
                            const QString &location);

    void savePlayerSettings();
    void setUpConnections();
    void restorePlayerSettings();

private slots:
    void onStatusChanged(QMediaPlayer::MediaStatus status);
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void setMediaForPlayback(const QModelIndex &index);
    void playOrPauseMedia();
    void updateCurrTrackLabel();
    void updatePlaylistTreeViewSelection();
    void setNextMediaForPlayback();
    void setPreviousMediaForPlayback();
    void playLoadedMedia();
};

#endif // PLAYER_H
