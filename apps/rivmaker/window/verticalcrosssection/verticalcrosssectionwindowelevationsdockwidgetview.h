#ifndef VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGETVIEW_H
#define VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGETVIEW_H

#include <QStandardItemModel>
#include <QWidget>

namespace Ui {
class VerticalCrossSectionWindowElevationsDockWidgetView;
}

class VerticalCrossSectionWindow;

class VerticalCrossSectionWindowElevationsDockWidgetView : public QWidget
{
	Q_OBJECT

public:
	explicit VerticalCrossSectionWindowElevationsDockWidgetView(QWidget *parent = 0);
	~VerticalCrossSectionWindowElevationsDockWidgetView();

	void updateTable();
	void setCsvExportEnabled(bool enabled);

private slots:
	void handleTableEdit(QStandardItem* editedItem);
	void exportCsv();

private:
	void initTable();
	VerticalCrossSectionWindow* chartWindow() const;

	QStandardItemModel m_model;

	Ui::VerticalCrossSectionWindowElevationsDockWidgetView *ui;

	class ItemDelegete;
};

#endif // VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGETVIEW_H
