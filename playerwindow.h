#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QMediaPlayer>
#include <QSqlTableModel>

#include "librarymodel.h"

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

private:
    Ui::PlayerWindow *ui;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};

    void closeEvent(QCloseEvent *event);
    void restoreWindowState();
    void saveWindowState();

    // ========
    QMediaPlayer *mediaToBeAdded = nullptr;
    void insertToTrackTable(const QString &title,
                            const QString &artist,
                            const QString &album,
                            int trackNum,
                            int year,
                            const QString &genre,
                            int duration,
                            const QString &location);


    QSqlTableModel *librarySourceModel = nullptr;
    LibraryModel *libraryProxyModel = nullptr;
    void initializeLibraryModels();
    void initializeLibraryTreeView();
    void initializeMediaPlayer();
    QMediaPlayer *mediaPlayer = nullptr;
    void restorePlayerSettings();
    void setUpConnections();
    QModelIndex srcIndexOfCurrMedia;
    void onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status);
    void playLoadedMedia();
    void setNextMediaForPlayback();
    void updateCurrTrackLabel();
    void updatePlaylistTreeViewSelection();
    void playOrPauseMedia();
    void setPreviousMediaForPlayback();

    // ========

private slots:
    void onAddToLibraryActionTriggered();

    // =======
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void setMediaForPlayback(const QModelIndex &index);
    // =======
};

#endif // PLAYERWINDOW_H
