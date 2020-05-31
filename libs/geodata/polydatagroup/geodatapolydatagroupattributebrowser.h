#ifndef GEODATAPOLYDATAGROUPATTRIBUTEBROWSER_H
#define GEODATAPOLYDATAGROUPATTRIBUTEBROWSER_H

#include <QDockWidget>

class GeoDataPolyDataGroup;

class QBoxLayout;
class QItemSelectionModel;
class QMainWindow;
class QModelIndex;
class QStandardItemModel;
class QTableView;

class GeoDataPolyDataGroupAttributeBrowser : public QDockWidget
{
	Q_OBJECT

public:
	explicit GeoDataPolyDataGroupAttributeBrowser(QWidget *parent = 0);
	~GeoDataPolyDataGroupAttributeBrowser();

	void setPolyDataGroup(GeoDataPolyDataGroup* g);
	void update();
	void updateSelection();

private slots:
	void applySelectionToPolyData();
	void handleItemClick(const QModelIndex& index);

private:
	bool selectionCoincides();

	GeoDataPolyDataGroup* m_group;
	QWidget* m_mainWidget;
	QBoxLayout* m_mainWidgetLayout;
	QTableView* m_tableView;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;

	class TableItemDelegate;
	TableItemDelegate* m_tableItemDelegate;

	bool m_updating;
};

#endif // GEODATAPOLYDATAGROUPATTRIBUTEBROWSER_H
