#ifndef COLORMAPENUMERATESETTINGEDITWIDGET_COLORTABLECONTROLLER_H
#define COLORMAPENUMERATESETTINGEDITWIDGET_COLORTABLECONTROLLER_H

#include "../colormapenumeratesettingeditwidget.h"

#include <QObject>

class ColorMapEnumerateSettingEditWidget::ColorTableController : public QObject
{
public:
	ColorTableController(ColorMapEnumerateSettingEditWidget* widget);
	~ColorTableController();

	void applyToTable();

private slots:
	void handleItemClick(const QModelIndex& index);

private:
	void editColor(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c);
	void editTransparent(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c);

	ColorMapEnumerateSettingEditWidget* m_widget;
	QStandardItemModel m_model;

	class Delegate;
	Delegate* m_delegate;
};

#endif // COLORMAPENUMERATESETTINGEDITWIDGET_COLORTABLECONTROLLER_H
