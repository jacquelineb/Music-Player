#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include <QWidget>
#include <QSqlRelationalTableModel>

namespace Ui {
class PlaylistTableView;
}

class PlaylistTableView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistTableView(QWidget *parent = nullptr);
    ~PlaylistTableView();

private:
    Ui::PlaylistTableView *ui;
    QSqlRelationalTableModel *model;

private slots:
    void onDoubleClicked(const QModelIndex &index);
};

#endif // PLAYLISTTABLEVIEW_H
