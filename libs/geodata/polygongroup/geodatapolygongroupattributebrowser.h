#ifndef GEODATAPOLYGONGROUPATTRIBUTEBROWSER_H
#define GEODATAPOLYGONGROUPATTRIBUTEBROWSER_H

#include <QDockWidget>

class GeoDataPolygonGroup;

class QBoxLayout;
class QItemSelectionModel;
class QMainWindow;
class QModelIndex;
class QStandardItemModel;
class QTableView;

class GeoDataPolygonGroupAttributeBrowser : public QDockWidget
{
	Q_OBJECT

public:
	explicit GeoDataPolygonGroupAttributeBrowser(QWidget *parent = 0);
	~GeoDataPolygonGroupAttributeBrowser();

	void setPolygonGroup(GeoDataPolygonGroup* g);
	void update();
	void updateSelection();

private slots:
	void applySelectionToPolygons();
	void handleItemClick(const QModelIndex& index);

private:
	bool selectionCoincides();

	GeoDataPolygonGroup* m_group;
	QWidget* m_mainWidget;
	QBoxLayout* m_mainWidgetLayout;
	QTableView* m_tableView;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;

	class TableItemDelegate;
	TableItemDelegate* m_tableItemDelegate;

	bool m_updating;
};

#endif // GEODATAPOLYGONGROUPATTRIBUTEBROWSER_H
