#ifndef LIBRARYPLAYLISTMODEL_H
#define LIBRARYPLAYLISTMODEL_H

#include <QSortFilterProxyModel>

class LibraryPlaylistModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    LibraryPlaylistModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    enum class ColumnHeader : int
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

    bool artistLessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // LIBRARYPLAYLISTMODEL_H
