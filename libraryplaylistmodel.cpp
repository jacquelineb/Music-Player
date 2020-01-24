#include "libraryplaylistmodel.h"
#include <QDebug> // Delete this later

LibraryPlaylistModel::LibraryPlaylistModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool LibraryPlaylistModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    const int sortingColumn  = left.column();
    const int artistColumn = static_cast<int>(ColumnHeader::artist);
    if (sortingColumn == artistColumn)
    {
        QString leftArtist = leftData.toString();
        QString rightArtist = rightData.toString();
        if (leftArtist == rightArtist)
        {
            const int leftRow = left.row();
            const int rightRow = right.row();
            const int albumColumn = static_cast<int>(ColumnHeader::album);
            QString leftAlbum = sourceModel()->data(index(leftRow, albumColumn)).toString();
            QString rightAlbum = sourceModel()->data(index(rightRow, albumColumn)).toString();

            if (leftAlbum == rightAlbum)
            {
                const int trackNumColumn = static_cast<int>(ColumnHeader::trackNum);
                int leftTrackNum = sourceModel()->data(index(leftRow, trackNumColumn)).toInt();
                int rightTrackNum = sourceModel()->data(index(rightRow, trackNumColumn)).toInt();

                if (leftTrackNum == rightTrackNum)
                {
                    const int titleColumn = static_cast<int>(ColumnHeader::title);
                    QString leftTrackName = sourceModel()->data(index(leftRow, titleColumn)).toString();
                    QString rightTrackName = sourceModel()->data(index(rightRow, titleColumn)).toString();
                    return leftTrackName < rightTrackName;
                }
                return leftTrackNum < rightTrackNum;
            }
            return leftAlbum < rightAlbum;
        }
        return leftArtist < rightArtist;
    }
    return false;
}

/*
bool LibraryPlaylistModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const int leftRow = left.row();
    const int rightRow = right.row();


    const int SORTING_COLUMN = left.column();
    if (SORTING_COLUMN == static_cast<int>(ColumnHeader::artist))
    {
        qDebug() << "SORTING BY ARTISTS";
        for (const ColumnHeader &columnHeader : artistSortPriority)
        {
           qDebug() << "i is " << static_cast<int>(columnHeader);
           const int columnHeaderIndex = static_cast<int>(columnHeader);
           const QModelIndex leftIndex = sourceModel()->index(leftRow, columnHeaderIndex, QModelIndex());
           const QModelIndex rightIndex = sourceModel()->index(rightRow, columnHeaderIndex, QModelIndex());

           const QString leftArtist = sourceModel()->


        }
    }

    return false;
}
*/


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
