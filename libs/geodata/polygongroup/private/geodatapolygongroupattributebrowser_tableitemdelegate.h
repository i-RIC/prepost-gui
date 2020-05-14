#ifndef GEODATAPOLYGONGROUPATTRIBUTEBROWSER_TABLEITEMDELEGATE_H
#define GEODATAPOLYGONGROUPATTRIBUTEBROWSER_TABLEITEMDELEGATE_H

#include "../geodatapolygongroupattributebrowser.h"

#include <QStyledItemDelegate>

class GeoDataPolygonGroupAttributeBrowser::TableItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	TableItemDelegate(QObject *parent = 0);

	void setPolygonGroup(GeoDataPolygonGroup* g);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	GeoDataPolygonGroup* m_polygonGroup;
};

#endif // GEODATAPOLYGONGROUPATTRIBUTEBROWSER_TABLEITEMDELEGATE_H
