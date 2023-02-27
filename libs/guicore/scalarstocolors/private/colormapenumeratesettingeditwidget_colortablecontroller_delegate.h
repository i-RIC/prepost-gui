#ifndef COLORMAPENUMERATESETTINGEDITWIDGET_COLORTABLECONTROLLER_DELEGATE_H
#define COLORMAPENUMERATESETTINGEDITWIDGET_COLORTABLECONTROLLER_DELEGATE_H

#include "colormapenumeratesettingeditwidget_colortablecontroller.h"

#include <QItemDelegate>

class ColorMapEnumerateSettingEditWidget::ColorTableController::Delegate : public QItemDelegate
{
public:
	Delegate(ColorTableController* controller, QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	ColorMapEnumerateSettingContainer* setting() const;

	ColorTableController* m_controller;
};

#endif // COLORMAPENUMERATESETTINGEDITWIDGET_COLORTABLECONTROLLER_DELEGATE_H
