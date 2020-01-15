#include "playlisttableview.h"
#include "ui_playlisttableview.h"

#include <QSqlRelationalTableModel>
PlaylistTableView::PlaylistTableView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistTableView)
{
    ui->setupUi(this);
    // connect to database
    // get the All Songs playlist
    // show the songs from All Songs playlist

    // Not sure if i should be setting a connection to the database in here
    model = new QSqlRelationalTableModel;
    model->setTable("Track");

    model->setRelation(2, QSqlRelation("Artist", "id", "name"));
    model->select();
    ui->tableView->setModel(model);
}

PlaylistTableView::~PlaylistTableView()
{
    delete model;
    delete ui;
}
