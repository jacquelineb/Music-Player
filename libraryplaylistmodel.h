#ifndef LIBRARYPLAYLISTMODEL_H
#define LIBRARYPLAYLISTMODEL_H

#include <QSortFilterProxyModel>
//#include <QSqlRelationalTableModel>

class LibraryPlaylistModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    LibraryPlaylistModel(QObject *parent = nullptr);
    int trackIdColumn() const { return static_cast<int>(Column::trackId); }
    int titleColumn() const { return static_cast<int>(Column::title); }
    int artistColumn() const { return static_cast<int>(Column::artist); }
    int albumColumn() const { return static_cast<int>(Column::album); }
    int trackNumColumn() const { return static_cast<int>(Column::trackNum); }
    int yearColumn() const { return static_cast<int>(Column::year); }
    int genreColumn() const { return static_cast<int>(Column::genre); }
    int durationColumn() const { return static_cast<int>(Column::duration); }
    int locationColumn() const { return static_cast<int>(Column::location); }

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
