#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QtSql>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "MainWindow 15";
    restoreWindowState();
    qDebug() << "MainWindow 17";
    connect(ui->actionAddToLibrary, &QAction::triggered, this, &MainWindow::onAddToLibraryActionTriggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onAddToLibraryActionTriggered()
{
    QUrl filename = QFileDialog::getOpenFileUrl(this, "Add file to library");
    ui->player->addToLibrary(filename);
}

void MainWindow::restoreWindowState()
{
    qDebug() << "RESTORING WINDOW STATE";
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
    qDebug() << "Saving Window State";
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
