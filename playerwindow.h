#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include "libraryproxymodel.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QSettings>
#include <QSqlTableModel>


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
    QSettings session{"session.ini", QSettings::Format::IniFormat};
    QMediaPlayer *mediaPlayer = nullptr;
    QSqlTableModel *librarySourceModel = nullptr;
    LibraryProxyModel *libraryProxyModel = nullptr;
    QModelIndex srcIndexOfCurrMedia;

    void initializeMediaPlayer();
    void initializeLibraryModels();
    void initializeLibraryView();
    void setUpConnections();
    void restoreWindowState();
    void restoreMediaPlayerVolume();
    void restoreLibraryViewState();
    void closeEvent(QCloseEvent *event) override;
    void saveSessionState();
    void saveWindowState();
    void saveMediaPlayerVolume();
    void saveLibraryViewState();
    void insertToTrackTable(const QString &title,
                            const QString &artist,
                            const QString &album,
                            unsigned int trackNum,
                            unsigned int year,
                            const QString &genre,
                            int duration,
                            const QString &location);

private slots:
    void onAddToLibraryActionTriggered();
    void onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlayOrPauseSignal();
    void setMediaForPlayback(const QModelIndex &index);
    void setNextMediaForPlayback();
    void setPreviousMediaForPlayback();
    void updateCurrTrackLabel();
    void updateLibraryViewSelection();
};

#endif // PLAYERWINDOW_H
