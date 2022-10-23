#ifndef COLORMAPSETTINGEDITWIDGET_COLORTABLECONTROLLER_DELEGATE_H
#define COLORMAPSETTINGEDITWIDGET_COLORTABLECONTROLLER_DELEGATE_H

#include "colormapsettingeditwidget_colortablecontroller.h"

#include <QItemDelegate>

class ColorMapSettingEditWidget::ColorTableController::Delegate : public QItemDelegate
{
public:
	Delegate(ColorTableController* controller, QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	ColorMapSettingContainer* setting() const;

	ColorTableController* m_controller;
};

#endif // COLORMAPSETTINGEDITWIDGET_COLORTABLECONTROLLER_DELEGATE_H
