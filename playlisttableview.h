#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include <QWidget>

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
};

#endif // PLAYLISTTABLEVIEW_H
