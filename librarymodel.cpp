#include "librarymodel.h"


LibraryModel::LibraryModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
/* Make trackId, duration, and location columns read-only. */
    Qt::ItemFlags flags = QAbstractProxyModel::flags(index);
    if (index.column() == trackIdColumn || index.column() == durationColumn || index.column() == locationColumn)
    {
        flags &= ~Qt::ItemIsEditable;
    }
    return flags;
}

bool LibraryModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const int sortingColumn = left.column();
    if (sortingColumn == artistColumn)
    {
        const QList<int> sortingColumnsForArtist = {artistColumn, albumColumn, trackNumColumn, titleColumn};
        return multiColumnLessThan(left, right, sortingColumnsForArtist);
    }
    else if (sortingColumn == albumColumn)
    {
        const QList<int> sortingColumnsForAlbum = {albumColumn, artistColumn, trackNumColumn};
        return multiColumnLessThan(left, right, sortingColumnsForAlbum);
    }
    const QVariant leftData = sourceModel()->data(left);
    const QVariant rightData = sourceModel()->data(right);
    return leftData < rightData;
}

bool LibraryModel::multiColumnLessThan(const QModelIndex &left, const QModelIndex &right, QList<int> columns) const
{
/* Called by lessThan(). Uses the columns listed in @columns to determine if the value of the item referred to by the
 * given index @left is less than the value of the item referred to by the given index @right.
 * E.g., when sorting by Album, if left Album and right Album are the same, I then want to compare Artists to determine
 * if @left is less than @right. If the two Artists are the same, compare by the Track Numbers. So, when sorting
 * by Album, I pass {albumColumn, artistColumn, trackNumColumn} as @columns. This keeps the songs in an album of a
 * specific artist grouped together and in its original tracklisting order (determined by trackNum).
 * The order of the items in @columns matters.
*/
    const int leftRow = left.row();
    const int rightRow = right.row();
    for (const int &column : columns)
    {
        const QModelIndex leftIndex = sourceModel()->index(leftRow, column, QModelIndex());
        const QModelIndex rightIndex = sourceModel()->index(rightRow, column, QModelIndex());
        const QVariant leftData = sourceModel()->data(leftIndex);
        const QVariant rightData = sourceModel()->data(rightIndex);

        if (leftData != rightData)
        {
            return leftData < rightData;
        }
    }
    return false;
}
