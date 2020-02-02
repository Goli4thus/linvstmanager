// This file is part of LinVstManager.

#include "customsortfilterproxymodel.h"
#include "enums.h"

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    mHideBlacklisted = false;
}

bool CustomSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QList<QModelIndex> modelIndices;
    int retVal;

    // Get source model indices of all columsn of current row
    for (int i=0; i < sourceModel()->columnCount(); i++) {
        modelIndices.append(sourceModel()->index(sourceRow, i, sourceParent));
    }


    // Filter out 'blacklisted' rows if they need to be hidden
    QString strStatus = sourceModel()->data(modelIndices.at(TableColumnPosType::Status)).toString();
    if (mHideBlacklisted && (strStatus == "Blacklisted")) {
        retVal = false;
    } else {
        // Apply RegExp filter (empty if not set by user via FilterBar; therefore always a match)
        bool match = false;
        for (int i=0; i < modelIndices.size(); i++) {
            if ((i == TableColumnPosType::Index) && (!mShowIndexColumn)) {
                // Skip evaluating 'index' column if it is hidden.
                continue;
            }
            if (sourceModel()->data(modelIndices.at(i)).toString().contains(filterRegExp())) {
                match = true;
                break;
            }
        }

        if (match) {
            retVal = true;
        } else {
            retVal = false;
        }
    }

    return retVal;
}

bool CustomSortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    int show;
    if (source_column == TableColumnPosType::Index) {
        if (mShowIndexColumn) {
            show = true;
        } else {
            show = false;
        }
    } else {
        show = true;
    }
    return show;
}

void CustomSortFilterProxyModel::setHideBlacklisted(bool hideBlacklisted)
{
    mHideBlacklisted = hideBlacklisted;
    invalidateFilter();
}

void CustomSortFilterProxyModel::setShowIndexColumn(bool showIndexColumn)
{
    mShowIndexColumn = showIndexColumn;
    invalidateFilter();
}
