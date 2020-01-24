#ifndef LIBRARYPLAYLISTMODEL_H
#define LIBRARYPLAYLISTMODEL_H

#include <QSortFilterProxyModel>

class LibraryPlaylistModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    LibraryPlaylistModel(QObject *parent = nullptr);
    void printRows();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    enum class ColumnHeader
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

    const QList<ColumnHeader> artistSortPriority = {ColumnHeader::artist, ColumnHeader::album, ColumnHeader::trackNum};
};

#endif // LIBRARYPLAYLISTMODEL_H
