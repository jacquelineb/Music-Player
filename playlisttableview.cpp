#include "playlisttableview.h"
#include "ui_playlisttableview.h"

#include <QSqlRelationalTableModel>
#include <QSortFilterProxyModel>
#include <QDebug>

PlaylistTableView::PlaylistTableView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistTableView)
{
    ui->setupUi(this);

    // Not sure if i should be setting a connection to the database in here

    // set up the model and tableview
    model = new QSqlRelationalTableModel;
    model->setTable("Track");
    model->setRelation(2, QSqlRelation("Artist", "id", "name"));
    //model->select();

    model->setHeaderData(0, Qt::Horizontal, tr("Track ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Artist"));
    model->setHeaderData(3, Qt::Horizontal, tr("Album"));
    model->setHeaderData(4, Qt::Horizontal, tr("Track Number"));
    model->setHeaderData(5, Qt::Horizontal, tr("Year"));
    model->setHeaderData(6, Qt::Horizontal, tr("Genre"));
    model->setHeaderData(7, Qt::Horizontal, tr("Duration"));
    model->setHeaderData(8, Qt::Horizontal, tr("Location"));
    ui->tableView->setModel(model);

    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(4, true);
    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnHidden(8, true);

    //https://forum.qt.io/topic/77920/how-to-keep-selection-on-one-qtableview-while-operating-on-a-another/4
    /*
    QPalette *palette = new QPalette();
    palette->setColor(QPalette::Highlight, QColor("#3a7fc2"));
    palette->setColor(QPalette::HighlightedText, QColor("white"));
    ui->tableView->setPalette(*palette);
    */

    // TODO: Format the Duration column (which is the duration of the song in millisec) to (hh):mm:ss format before displaying
    // TODO: Sort the model based on multiple colums. I want to sort by Artist name, then by album name, then by track number
    // for now I will just sort by artist name
    //ui->tableView->setSortingEnabled(true); // This allows the user to sort by click on a column header.
    ui->tableView->sortByColumn(2, Qt::SortOrder::AscendingOrder);

    // TODO: Update the model when user clicks "Add file to library" from the MainWindow

    connect(ui->tableView, &QTableView::doubleClicked, this, &PlaylistTableView::onDoubleClicked);
}

void PlaylistTableView::onDoubleClicked(const QModelIndex &index)
{
    // Emit a signal to be slotted in Player. The slot should tell the QMediaPlayer/QMediaPlaylist in Player to start playing the double clicked item.
    qDebug() << "PlaylistTableView Line 50: " << index.row();
}

PlaylistTableView::~PlaylistTableView()
{
    delete model;
    delete ui;
}
