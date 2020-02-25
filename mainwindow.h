#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QMediaPlayer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSettings settings{"session.ini", QSettings::Format::IniFormat};

    void closeEvent(QCloseEvent *event);
    void restoreWindowState();
    void saveWindowState();

    // ========
    QMediaPlayer *mediaToBeAdded = nullptr;
    void insertToTrackTable(const QString &title,
                            int artistId,
                            const QString &album,
                            int trackNum,
                            int year,
                            const QString &genre,
                            int duration,
                            const QString &location);

    // ========

private slots:
    void onAddToLibraryActionTriggered();

    // =======
    void onAddMediaStatusChanged(QMediaPlayer::MediaStatus status);
    // =======
};

#endif // MAINWINDOW_H
