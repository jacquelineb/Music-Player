#include "libraryplaylistmodel.h"
#include <QDebug> // Delete this later

LibraryPlaylistModel::LibraryPlaylistModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

/*
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
*/


bool LibraryPlaylistModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const int sortingColumn = left.column();

    if (sortingColumn == static_cast<int>(ColumnHeader::artist))
    {
        /*
        const QList<ColumnHeader> artistSortPriority = {ColumnHeader::artist, ColumnHeader::album, ColumnHeader::trackNum};
        const int leftRow = left.row();
        const int rightRow = right.row();
        for (const ColumnHeader &columnHeader : artistSortPriority)
        {
            qDebug() << "i is " << static_cast<int>(columnHeader);
            const int columnToCompare = static_cast<int>(columnHeader);
            const QModelIndex leftIndex = sourceModel()->index(leftRow, columnToCompare, QModelIndex());
            const QModelIndex rightIndex = sourceModel()->index(rightRow, columnToCompare, QModelIndex());

            const QVariant leftData = sourceModel()->data(leftIndex);
            const QVariant rightData = sourceModel()->data(rightIndex);

            if (leftData != rightData)
            {
                return leftData < rightData;
            }

        }
        */
        return artistLessThan(left, right);
    }
    else if (sortingColumn == static_cast<int>(ColumnHeader::album))
    {
        // sort by album, using multiple columns
        const QList<ColumnHeader> albumSortPriority = {ColumnHeader::album, ColumnHeader::trackNum, ColumnHeader::artist};
        const int leftRow = left.row();
        const int rightRow = right.row();
        for (const ColumnHeader &columnHeader : albumSortPriority)
        {
            const int columnToCompare = static_cast<int>(columnHeader);
            const QModelIndex leftIndex = sourceModel()->index(leftRow, columnToCompare, QModelIndex());
            const QModelIndex rightIndex = sourceModel()->index(rightRow, columnToCompare, QModelIndex());

            const QVariant leftData = sourceModel()->data(leftIndex);
            const QVariant rightData = sourceModel()->data(rightIndex);

            if (leftData != rightData)
            {
                return leftData < rightData;
            }
        }

        //albumLessThan(left, right);
    }
    const QVariant leftData = sourceModel()->data(left);
    const QVariant rightData = sourceModel()->data(right);
    return leftData < rightData;
}

/* YOU CAN PROBABLY IMPLEMENT A FUNCTION SIMILAR TO artistLessThan THAT WILL WORK FOR BOTH artistLessThan and
 * albumLessThan, since they're essentially the same, just with different priority.
 * just need to pass in the priority list (i.e artistSortPriorty or albumSortPriority)
 * signature would be something like priorityBasedLessThan(QModelIndex&, QModelIndex&, priorityList);
*/


bool LibraryPlaylistModel::artistLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    //List of priority when sorting by artist. I.e., if sorting by artist, first sort by the artist column.
    // If two artists are the same, then use the album column to determine an order.
    const QList<ColumnHeader> artistSortPriority = {ColumnHeader::artist, ColumnHeader::album, ColumnHeader::trackNum};
    const int leftRow = left.row();
    const int rightRow = right.row();
    for (const ColumnHeader &columnHeader : artistSortPriority)
    {
        //qDebug() << "sort column is " << static_cast<int>(columnHeader);
        const int columnToCompare = static_cast<int>(columnHeader);
        const QModelIndex leftIndex = sourceModel()->index(leftRow, columnToCompare, QModelIndex());
        const QModelIndex rightIndex = sourceModel()->index(rightRow, columnToCompare, QModelIndex());

        const QVariant leftData = sourceModel()->data(leftIndex);
        const QVariant rightData = sourceModel()->data(rightIndex);

        if (leftData != rightData)
        {
            return leftData < rightData;
        }
    }
    return false;
}



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
