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
    restoreWindowState();
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

void MainWindow::restoreWindowState()
{
    move(settings.value("MainWindow/position", pos()).toPoint());
    if (settings.value("MainWindow/isMaximized", true).toBool())
    {
        setWindowState(Qt::WindowState::WindowMaximized);
    }
    else
    {
        resize(settings.value("MainWindow/size", size()).toSize());
    }
}

void MainWindow::saveWindowState()
{
    settings.setValue("MainWindow/position", pos());
    settings.setValue("MainWindow/isMaximized", isMaximized());
    settings.setValue("MainWindow/size", size());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    saveWindowState();
    ui->player->close();
    event->accept();
}
