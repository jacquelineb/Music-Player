#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

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

private slots:
    void onOpenActionTriggered();
    void onAddToLibraryActionTriggered();
};

#endif // MAINWINDOW_H
