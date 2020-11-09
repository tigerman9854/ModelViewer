#pragma once
#include "TreeItem.h"
#include <QAbstractItemModel>

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(TreeItem*, QObject* parent = nullptr);
    ~TreeModel();

    TreeItem* getRootItem();

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
        const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    //bool setData(const QModelIndex& index, const QVariant& value);

private:

    TreeItem* rootItem;
};

// SRC: https://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html