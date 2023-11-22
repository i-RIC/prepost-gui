#ifndef V4POSTCALCULATEDRESULTEDITDIALOG_TABLEDELEGATE_H
#define V4POSTCALCULATEDRESULTEDITDIALOG_TABLEDELEGATE_H

#include "../v4postcalculatedresulteditdialog.h"

#include <QItemDelegate>

class v4PostCalculatedResultEditDialog::TableDelegate : public QItemDelegate
{
public:
	TableDelegate(QObject* parent = nullptr);

	void setCalculatedResult(v4PostCalculatedResult* result);
	void setCalculatedResultIndex(int index);
	void setZoneDataContainer(v4PostZoneDataContainer* c);

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	// void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	v4PostCalculatedResult* m_result;
	int m_resultIndex;
	v4PostZoneDataContainer* m_zoneDataContainer;
};

#endif // V4POSTCALCULATEDRESULTEDITDIALOG_TABLEDELEGATE_H
