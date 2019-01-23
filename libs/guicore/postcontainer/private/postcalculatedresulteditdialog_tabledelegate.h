#ifndef POSTCALCULATEDRESULTEDITDIALOG_TABLEDELEGATE_H
#define POSTCALCULATEDRESULTEDITDIALOG_TABLEDELEGATE_H

#include "../postcalculatedresulteditdialog.h"

#include <QItemDelegate>

class PostCalculatedResultEditDialog::TableDelegate : public QItemDelegate
{
public:
	TableDelegate(QObject* parent = nullptr);

	void setCalculatedResult(PostCalculatedResult* result);
	void setCalculatedResultIndex(int index);
	void setZoneDataContainer(PostZoneDataContainer* c);

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	// void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	PostCalculatedResult* m_result;
	int m_resultIndex;
	PostZoneDataContainer* m_zoneDataContainer;
};

#endif // POSTCALCULATEDRESULTEDITDIALOG_TABLEDELEGATE_H
