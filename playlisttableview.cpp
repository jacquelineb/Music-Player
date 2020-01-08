#include "playlisttableview.h"
#include "ui_playlisttableview.h"

PlaylistTableView::PlaylistTableView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistTableView)
{
    ui->setupUi(this);
    // connect to database
    // get the All Songs playlist
    // show the songs from All Songs playlist
}

PlaylistTableView::~PlaylistTableView()
{
    delete ui;
}
