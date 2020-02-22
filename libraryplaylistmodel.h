#ifndef LIBRARYPLAYLISTMODEL_H
#define LIBRARYPLAYLISTMODEL_H

#include <QSortFilterProxyModel>
//#include <QSqlRelationalTableModel>

class LibraryPlaylistModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    LibraryPlaylistModel(QObject *parent = nullptr);
protected:
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    enum class Column : int
    {
        trackId = 0,
        title = 1,
        artist = 2,
        album = 3,
        trackNum = 4,
        year = 5,
        genre = 6,
        duration = 7,
        location = 8
    };
    bool multiColumnLessThan(const QModelIndex &left, const QModelIndex &right, QList<Column> columns) const;
};

#endif // LIBRARYPLAYLISTMODEL_H
