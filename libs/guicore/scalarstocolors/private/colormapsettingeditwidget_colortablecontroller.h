#ifndef COLORMAPSETTINGEDITWIDGET_COLORTABLECONTROLLER_H
#define COLORMAPSETTINGEDITWIDGET_COLORTABLECONTROLLER_H

#include "../colormapsettingeditwidget.h"

#include <QStandardItemModel>
#include <QObject>

class ColorMapSettingEditWidget::ColorTableController : public QObject
{
	Q_OBJECT

public:
	ColorTableController(ColorMapSettingEditWidget* widget);
	~ColorTableController();

	void applyToTable();

private slots:
	void handleItemClick(const QModelIndex& index);
	void handleTableSelectionChange(const QItemSelection& selected, const QItemSelection& deselected);

private:
	void editColor(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c);
	void editTransparent(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c);

	ColorMapSettingEditWidget* m_widget;
	QStandardItemModel m_model;

	class Delegate;
	Delegate* m_delegate;
};

#endif // COLORMAPSETTINGEDITWIDGET_COLORTABLECONTROLLER_H
