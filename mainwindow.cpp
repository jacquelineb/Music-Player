#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "right after setupui : " << pos();
    restoreWindowSettings();
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenActionTriggered);
    connect(ui->actionAddToLibrary, &QAction::triggered, this, &MainWindow::onAddToLibraryActionTriggered);
    qDebug() << "after connections: " << pos();
}

MainWindow::~MainWindow()
{
    saveWindowSettings();
    delete ui;
}

void MainWindow::onOpenActionTriggered()
{
    QUrl filename = QFileDialog::getOpenFileUrl(this, "Open file");
    if (!filename.isEmpty())
    {
        ui->player->setMediaOfPlayer(filename);
    }
}

void MainWindow::onAddToLibraryActionTriggered()
{
    QUrl filename = QFileDialog::getOpenFileUrl(this, "Add file to library");
    ui->player->addToLibrary(filename);
}

void MainWindow::restoreWindowSettings()
{
    QSettings settings("session.ini", QSettings::Format::IniFormat);
    //qDebug() << "before resize: " << size();
    move(settings.value("mainwindow/pos", pos()).toPoint());
    resize(settings.value("mainwindow/size", size()).toSize());
    //qDebug() << "after resize: " << size();
    //qDebug() << pos();
    restoreState(settings.value("mainwindow/state").toByteArray());
    //MainWindow::setWindowState(Qt::WindowState::WindowMaximized);
    qDebug() << "From restore: " << pos();
}

void MainWindow::saveWindowSettings()
{
    qDebug() << "from save: " << pos();
    QSettings settings("session.ini", QSettings::Format::IniFormat);
    settings.setValue("mainwindow/pos", pos());
    settings.setValue("mainwindow/size", size());
    //qDebug() << "saving size: " << size();
}
