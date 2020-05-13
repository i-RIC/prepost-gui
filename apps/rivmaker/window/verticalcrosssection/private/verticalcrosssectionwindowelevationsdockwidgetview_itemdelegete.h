#ifndef VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGETVIEW_ITEMDELEGETE_H
#define VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGETVIEW_ITEMDELEGETE_H

#include "../verticalcrosssectionwindowelevationsdockwidgetview.h"

#include <QStyledItemDelegate>

class VerticalCrossSectionWindowElevationsDockWidgetView::ItemDelegete : public QStyledItemDelegate
{
public:
	ItemDelegete(QObject* parent = nullptr);

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const override;
};

#endif // VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGETVIEW_ITEMDELEGETE_H
