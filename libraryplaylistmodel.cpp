#include "libraryplaylistmodel.h"
#include <QDebug> // Delete this later

LibraryPlaylistModel::LibraryPlaylistModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool LibraryPlaylistModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const int sortingColumn = left.column();
    if (sortingColumn == static_cast<int>(ColumnHeader::artist))
    {
        return artistLessThan(left, right);
    }
    else if (sortingColumn == static_cast<int>(ColumnHeader::album))
    {
    /* YOU CAN PROBABLY IMPLEMENT A FUNCTION SIMILAR TO artistLessThan THAT WILL WORK FOR BOTH artistLessThan and
    * albumLessThan, since they're essentially the same, just with different priority.
    * just need to pass in the priority list (i.e artistSortPriorty or albumSortPriority)
    * signature would be something like priorityBasedLessThan(QModelIndex&, QModelIndex&, priorityList);
    */
        // sort by album, using multiple columns
        const QList<ColumnHeader> albumSortPriority = {ColumnHeader::album, ColumnHeader::artist, ColumnHeader::trackNum};
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
    }
    const QVariant leftData = sourceModel()->data(left);
    const QVariant rightData = sourceModel()->data(right);
    return leftData < rightData;
}

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
