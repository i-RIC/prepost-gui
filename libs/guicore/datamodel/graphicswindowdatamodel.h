#ifndef GRAPHICSWINDOWDATAMODEL_H
#define GRAPHICSWINDOWDATAMODEL_H

#include "../guicore_global.h"

#include "../project/projectdataitem.h"
#include "graphicswindowsimpledatamodel.h"

class QPoint;
class QMenu;
class QMainWindow;
class QToolBar;
class QModelIndex;
class QGraphicsItem;
class QStandardItem;
class QStandardItemModel;
class GraphicsWindowRootDataItem;
class GraphicsWindowDataItem;
class ObjectBrowserView;
class ProjectData;
class VTKGraphicsView;

class GUICOREDLL_EXPORT GraphicsWindowDataModel : public GraphicsWindowSimpleDataModel
{
	Q_OBJECT

public:
	/// Constructor
	GraphicsWindowDataModel(QMainWindow* w, ProjectDataItem* parent);
	/// Destructor
	~GraphicsWindowDataModel();
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void closeCgnsFile() override;

	/// The model to store the model of object browser.
	QStandardItemModel* itemModel() const {return m_itemModel;}
	void updateExpandState(ObjectBrowserView* view);
	void reflectExpandState(ObjectBrowserView* view);
	void setObjectBrowserView(ObjectBrowserView* v);
	ObjectBrowserView* objectBrowserView() const {return m_objectBrowserView;}
	void fitOnDataLoad();
	QToolBar* operationToolBar() const {return m_operationToolBar;}
	void viewOperationEndedGlobal() override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

public slots:
	/// Handle mouse press event on object browser.
	void handleObjectBrowserPress(const QModelIndex& index, const QPoint& globalPos);
	/// Handle click event on object browser.
	void handleObjectBrowserClick(const QModelIndex& index);
	/// Handle double click event on object browser.
	void handleObjectBrowserDoubleClick(const QModelIndex& index);
	void handleObjectBrowserChange(QStandardItem* changeditem);
	virtual void handleObjectBrowserSelectionChange() {}
	void handleObjectBrowserDeselection(const QModelIndex& previous);
	void handleObjectBrowserSelection(const QModelIndex& current);
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

	void update2Ds();

protected:
	ObjectBrowserView* m_objectBrowserView;
	GraphicsWindowRootDataItem* m_rootDataItem;
	QStandardItemModel* m_itemModel;

	GraphicsWindowDataItem* m_selectedItem;
	QMenu* m_rightClickMenu;

private:
	QToolBar* m_operationToolBar;

	bool m_dataLoaded;
};

#endif // GRAPHICSWINDOWDATAMODEL_H
