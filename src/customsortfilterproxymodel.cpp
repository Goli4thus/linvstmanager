// This file is part of LinVstManager.

#include "customsortfilterproxymodel.h"
#include "enums.h"

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    mHideBlacklisted = false;
    mShowIndexColumn = false;
}

bool CustomSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QVector<QModelIndex> modelIndices;
    int retVal;
    QString strAllColumns;

    // Get source model indices of all columsn of current row
    for (int i=0; i < sourceModel()->columnCount(); i++) {
        modelIndices.append(sourceModel()->index(sourceRow, i, sourceParent));
    }


    // Filter out 'blacklisted' rows if they need to be hidden
    QString strStatus = sourceModel()->data(modelIndices.at(nsMW::TableColumnPosType::Status)).toString();
    if (mHideBlacklisted && (strStatus == "Blacklisted")) {
        retVal = false;
    } else {
        // Apply RegExp filter
        // (pattern is empty if not set by user via FilterBar; therefore always a match)
        // Apply regex accross all columns of current row
        // First step: Construct a combined string of all columns of current row (space separated).
        strAllColumns.clear();
        for (int i=0; i < modelIndices.size(); i++) {
            if ((i == nsMW::TableColumnPosType::Index) && (!mShowIndexColumn)) {
                // Skip evaluating 'index' column if it is hidden.
                continue;
            } else {
                strAllColumns.append(sourceModel()->data(modelIndices.at(i)).toString() + " ");
            }
        }

        // Second step: Check for match
        if (strAllColumns.contains(filterRegExp())) {
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
    if (source_column == nsMW::TableColumnPosType::Index) {
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
