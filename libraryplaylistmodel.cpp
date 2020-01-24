#include "libraryplaylistmodel.h"
#include <QDebug> // Delete this later

LibraryPlaylistModel::LibraryPlaylistModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}


bool LibraryPlaylistModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const int leftRow = left.row();
    const int rightRow = right.row();
    qDebug() << leftRow;
    qDebug() << rightRow;

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    const int SORTING_COLUMN = left.column();

    qDebug() << "LibraryPlaylistModel Line 15: " << left.row() << " " << left.column();
    qDebug() << "LibraryPlaylistModel Line 16: " << right.row() << " " << right.column();

    /*
    QString leftArtist = sourceModel()->data(index(left.row(), ARTIST_COLUMN)).toString();
    QString rightArtist = sourceModel()->data(index(left.row(), ARTIST_COLUMN)).toString();
    */

    if (SORTING_COLUMN == ARTIST_COLUMN)
    {
        qDebug() << "SORTING BY ARTIST";
        QString leftArtist = leftData.toString();
        QString rightArtist = rightData.toString();

        qDebug() << leftArtist << rightArtist;
        if (leftArtist == rightArtist)
        {
            QString leftAlbum = data(index(left.row(), ALBUM_COLUMN)).toString();
            QString rightAlbum = data(index(right.row(), ALBUM_COLUMN)).toString();
            qDebug() << leftAlbum << " " << rightAlbum;
            if (leftAlbum == rightAlbum)
            {
                qDebug() << "THE ALBUMS ARE THE SAME";

                int leftTrackNum = data(index(left.row(), TRACK_NUMBER_COLUMN)).toInt();
                int rightTrackNum = data(index(right.row(), TRACK_NUMBER_COLUMN)).toInt();
                qDebug() << leftTrackNum << " " << rightTrackNum;

                if (leftTrackNum == rightTrackNum)
                {
                    QString leftTrackName = data(index(left.row(), TITLE_COLUMN)).toString();
                    QString rightTrackName = data(index(right.row(), TITLE_COLUMN)).toString();
                    return leftTrackName < rightTrackName;
                }
                return leftTrackNum < rightTrackNum;
            }
            return leftAlbum < rightAlbum;
            //return QString::localeAwareCompare(leftAlbum, rightAlbum) < 0;
        }
        return leftArtist < rightArtist;
    }

    return false;
}

/*
bool artistLessThan()
{
}
*/


void LibraryPlaylistModel::printRows()
{
    qDebug() << "Printing rows";

    for (int i = 0; i < rowCount(); i++)
    {
        QString rowData;
        for (int j = 0; j < columnCount(); j++)
        {
            QString rd = data(index(i,j), Qt::DisplayRole).toString();
            rowData += " " + rd;
        }
        qDebug() << rowData;
    }


    qDebug() << data(index(0, 2)).toString();
    qDebug() << sourceModel()->data(index(0,2));
}
