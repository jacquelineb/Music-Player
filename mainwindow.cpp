#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    restoreWindowSettings();
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenActionTriggered);
    connect(ui->actionAddToLibrary, &QAction::triggered, this, &MainWindow::onAddToLibraryActionTriggered);
}

MainWindow::~MainWindow()
{
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
    move(settings.value("mainwindow/pos", pos()).toPoint());
    if (settings.value("mainwindow/isMaximized", true).toBool())
    {
        setWindowState(Qt::WindowState::WindowMaximized);
    }
    else
    {
        resize(settings.value("mainwindow/size", size()).toSize());
    }
}



void MainWindow::saveWindowSettings()
{
    qDebug() << "from save: " << pos();
    QSettings settings("session.ini", QSettings::Format::IniFormat);
    settings.setValue("mainwindow/pos", pos());
    settings.setValue("mainwindow/isMaximized", isMaximized());
    settings.setValue("mainwindow/size", size());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    saveWindowSettings();
}
