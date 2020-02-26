#include "librarymodel.h"
#include <QDebug> // Delete this later

LibraryModel::LibraryModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractProxyModel::flags(index);
    if (index.column() == static_cast<int>(Column::trackId)
        || index.column() == static_cast<int>(Column::duration)
        || index.column() == static_cast<int>(Column::location))
    {
        // Make trackId, duration, and location columns read only
        flags &= ~Qt::ItemIsEditable;
    }
    return flags;
}

bool LibraryModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const int sortingColumn = left.column();
    if (sortingColumn == static_cast<int>(Column::artist))
    {
        const QList<Column> sortingColumnsForArtist = {Column::artist, Column::album, Column::trackNum, Column::title};
        return multiColumnLessThan(left, right, sortingColumnsForArtist);
    }
    else if (sortingColumn == static_cast<int>(Column::album))
    {
        const QList<Column> sortingColumnsForAlbum = {Column::album, Column::artist, Column::trackNum};
        return multiColumnLessThan(left, right, sortingColumnsForAlbum);
    }
    const QVariant leftData = sourceModel()->data(left);
    const QVariant rightData = sourceModel()->data(right);
    return leftData < rightData;
}

bool LibraryModel::multiColumnLessThan(const QModelIndex &left,
                                               const QModelIndex &right,
                                               QList<Column> columns) const
{
/* Called by lessThan(). Uses the columns listed in @columns to determine if the value of the item referred to by the
 * given index @left is less than the value of the item referred to by the given index @right.
 * E.g., when sorting by Album, if left Album and right Album are the same, I then want to compare Artists to determine
 * if @left is less than @right. If the two Artists are the same, compare by the Track Numbers. So, when sorting
 * by Album, I pass {Column::album, Column::artist, Column::trackNum} as @columns. This keeps the songs in an album of a
 * specific artist grouped together and in its original tracklisting order (determined by trackNum).
 * The order of the items in @columns matters.
*/
    const int leftRow = left.row();
    const int rightRow = right.row();
    for (const Column &column : columns)
    {
        const int columnToCompare = static_cast<int>(column);
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
