// This file is part of LinVstManager.

#ifndef CUSTOMSORTFILTERPROXYMODEL_H
#define CUSTOMSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CustomSortFilterProxyModel(QObject *parent = nullptr);

    void setHideBlacklisted(bool hideBlacklisted);
    void setShowIndexColumn(bool showIndexColumn);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;

private:
    bool mHideBlacklisted;
    bool mShowIndexColumn;
};

#endif // CUSTOMSORTFILTERPROXYMODEL_H
