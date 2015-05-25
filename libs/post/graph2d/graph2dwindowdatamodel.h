#ifndef GRAPH2DWINDOWDATAMODEL_H
#define GRAPH2DWINDOWDATAMODEL_H

#include "graph2d_global.h"
#include <guicore/project/projectdataitem.h>

#include <QObject>

class Graph2dWindowView;
class QPoint;
class QMenu;
class QMainWindow;
class QToolBar;
class QModelIndex;
class QGraphicsItem;
class QStandardItem;
class QStandardItemModel;
class Graph2dWindowRootDataItem;
class Graph2dWindowDataItem;
class ObjectBrowserView;
class ProjectData;
class QWidget;
class Graph2dWindowVerticalAxisSetting;

class GRAPH2D_EXPORT Graph2dWindowDataModel : public ProjectDataItem
{
	Q_OBJECT
public:
	enum AxisSide {
		asLeft,
		asRight
	};
	static const int MOVEWIDTH = 1;
	/// Constructor
	Graph2dWindowDataModel(QMainWindow* w, ProjectDataItem* parent);
	/// Destructor
	~Graph2dWindowDataModel();
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void closeCgnsFile() override;

	/// The view to display pre-processor main region
	Graph2dWindowView* view() const {return m_view;}
	/// The model to store the model of object browser.
	QStandardItemModel* itemModel() const {return m_itemModel;}
	void updateExpandState(ObjectBrowserView* view);
	void reflectExpandState(ObjectBrowserView* view);
	QMainWindow* mainWindow() const {return m_mainWindow;}
	void setObjectBrowserView(ObjectBrowserView* v);
	ObjectBrowserView* objectBrowserView() const {return m_objectBrowserView;}
	Graph2dWindowVerticalAxisSetting* leftAxisSetting() const {return m_leftAxisSetting;}
	Graph2dWindowVerticalAxisSetting* rightAxisSetting() const {return m_rightAxisSetting;}
protected:
	virtual void getYAxisValueRange(AxisSide, double* min, double* max) = 0;
	virtual void applyAxisSetting() = 0;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
public slots:
	/// Handle mouse press event on object browser.
	void handleObjectBrowserPress(const QModelIndex& index, const QPoint& globalPos);
	/// Handle click event on object browser.
	void handleObjectBrowserClick(const QModelIndex& index);
	/// Handle double click event on object browser.
	void handleObjectBrowserDoubleClick(const QModelIndex& index);
	/// Delete the item currently selected on object browser.
	void deleteItem(const QModelIndex& index);
	/// Move up the item.
	void moveUpItem(const QModelIndex& index);
	/// Move down the item.
	void moveDownItem(const QModelIndex& index);
	/// Show Property dialog of the item currently selected on object browser.
	void showPropertyDialog(const QModelIndex& index);
	/// Update the operation toolbar.
	void updateOperationToolBar(const QModelIndex& index, QWidget* parent);
	QToolBar* operationToolBar() const {return m_operationToolBar;}
	void verticalAxisLeftSetting();
	void verticalAxisRightSetting();
	void titleSetting();
	void setLegendVisible(bool visible);
protected slots:
	void handleObjectBrowserChange(QStandardItem* changeditem);
	virtual void handleObjectBrowserSelectionChange() {}
	void handleObjectBrowserDeselection(const QModelIndex& previous);
	void handleObjectBrowserSelection(const QModelIndex& current);
protected:
	QMainWindow* m_mainWindow;
	Graph2dWindowView* m_view;
	ObjectBrowserView* m_objectBrowserView;
	Graph2dWindowRootDataItem* m_rootDataItem;
	QStandardItemModel* m_itemModel;

	Graph2dWindowDataItem* m_selectedItem;
	QMenu* m_rightClickMenu;

	Graph2dWindowVerticalAxisSetting* m_leftAxisSetting;
	Graph2dWindowVerticalAxisSetting* m_rightAxisSetting;
	QString m_title;
private:
	qreal moveWidth();
	void moveCenter(int x, int y);
	QToolBar* m_operationToolBar;

	bool m_dataLoaded;
};

#endif // GRAPH2DWINDOWDATAMODEL_H
