#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    ui->player->setMediaFile(filename);
}

void MainWindow::onAddToLibraryActionTriggered()
{
    QUrl filename = QFileDialog::getOpenFileUrl(this, "Add file to library");
    ui->player->addToLibrary(filename);
}
