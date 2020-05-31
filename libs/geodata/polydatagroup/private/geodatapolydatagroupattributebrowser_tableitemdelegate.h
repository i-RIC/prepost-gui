#ifndef GEODATAPOLYDATAGROUPATTRIBUTEBROWSER_TABLEITEMDELEGATE_H
#define GEODATAPOLYDATAGROUPATTRIBUTEBROWSER_TABLEITEMDELEGATE_H

#include "../geodatapolydatagroupattributebrowser.h"

#include <QStyledItemDelegate>

class GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	TableItemDelegate(QObject *parent = 0);

	void setPolyDataGroup(GeoDataPolyDataGroup* g);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	GeoDataPolyDataGroup* m_group;
};

#endif // GEODATAPOLYDATAGROUPATTRIBUTEBROWSER_TABLEITEMDELEGATE_H
