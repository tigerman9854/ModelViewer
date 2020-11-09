#pragma once
#include <QVector>
#include <QVariant>
#include <QStandardItemModel>

class TreeItem : public QStandardItemModel
{
public:
    explicit TreeItem(const QVector<QVariant>& data, TreeItem* parent = nullptr);
    ~TreeItem();

    TreeItem* child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void insertChild(TreeItem*);
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    TreeItem* parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant& value);

private:
    
    QVector<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem* parentItem;
};

// SRC: https://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html