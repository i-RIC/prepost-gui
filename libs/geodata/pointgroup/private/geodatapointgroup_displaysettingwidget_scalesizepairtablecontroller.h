#ifndef GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_SCALESIZEPAIRTABLECONTROLLER_H
#define GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_SCALESIZEPAIRTABLECONTROLLER_H

#include "../public/geodatapointgroup_displaysettingwidget.h"

#include <QStandardItemModel>
#include <QObject>

class QItemSelection;

class GeoDataPointGroup::DisplaySettingWidget::ScaleSizePairTableController : public QObject
{
	Q_OBJECT

public:
	ScaleSizePairTableController(DisplaySettingWidget* widget);
	~ScaleSizePairTableController();

	void applyToTable();

private slots:
	void handleItemClick(const QModelIndex& index);
	void handleTableSelectionChange(const QItemSelection& selected, const QItemSelection& deselected);

private:
	DisplaySettingWidget* m_widget;
	QStandardItemModel m_model;

	class Delegate;
	Delegate* m_delegate;
};

#endif // GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_SCALESIZEPAIRTABLECONTROLLER_H
