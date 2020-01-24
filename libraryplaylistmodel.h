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
    const int TRACK_ID_COLUMN = 0;
    const int TITLE_COLUMN = 1;
    const int ARTIST_COLUMN = 2;
    const int ALBUM_COLUMN = 3;
    const int TRACK_NUMBER_COLUMN = 4;
    const int YEAR_COLUMN = 5;
    const int GENRE_COLUMN = 6;
    const int DURATION_COLUMN = 7;
    const int LOCATION_COLUMN = 8;

};

#endif // LIBRARYPLAYLISTMODEL_H
