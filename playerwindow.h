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
    ~PlayerWindow() override;

private:
    Ui::PlayerWindow *ui;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};
    QMediaPlayer *mediaPlayer = nullptr;
    QSqlTableModel *librarySourceModel = nullptr;
    LibraryModel *libraryProxyModel = nullptr;
    QModelIndex srcIndexOfCurrMedia;

    void initializeMediaPlayer();
    void initializeLibraryModels();
    void initializeLibraryTreeView();
    void setUpConnections();
    void restoreWindowState();
    void restoreMediaPlayerVolume();
    void restoreLibraryViewState();
    void closeEvent(QCloseEvent *event) override;
    void saveSessionState();
    void saveWindowState();
    void saveMediaPlayerVolume();
    void saveLibraryViewState();

    QMediaPlayer *mediaToAdd = nullptr; //
    void insertToTrackTable(const QString &title,
                            const QString &artist,
                            const QString &album,
                            int trackNum,
                            int year,
                            const QString &genre,
                            int duration,
                            const QString &location);

private slots:
    void onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlayOrPauseSignal();
    void setMediaForPlayback(const QModelIndex &index);
    void setNextMediaForPlayback();
    void setPreviousMediaForPlayback();
    void updateCurrTrackLabel();
    void updatePlaylistTreeViewSelection();

    void onAddToLibraryActionTriggered(); //
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status); //
};

#endif // PLAYERWINDOW_H
