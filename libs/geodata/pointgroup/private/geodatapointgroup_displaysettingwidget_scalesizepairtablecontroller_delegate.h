#ifndef GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_SCALESIZEPAIRTABLECONTROLLER_DELEGATE_H
#define GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_SCALESIZEPAIRTABLECONTROLLER_DELEGATE_H

#include "geodatapointgroup_displaysettingwidget_scalesizepairtablecontroller.h"

#include <QItemDelegate>

class GeoDataPointGroup::DisplaySettingWidget::ScaleSizePairTableController::Delegate : public QItemDelegate
{
public:
	Delegate(ScaleSizePairTableController* controller, QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	DisplaySetting* setting() const;

	ScaleSizePairTableController* m_controller;
};

#endif // GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_SCALESIZEPAIRTABLECONTROLLER_DELEGATE_H
