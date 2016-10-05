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
	GraphicsWindowDataModel(QMainWindow* w, ProjectDataItem* parent);
	~GraphicsWindowDataModel();

	QToolBar* operationToolBar() const;
	QStandardItemModel* itemModel() const;

	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void closeCgnsFile() override;

	void updateExpandState(ObjectBrowserView* view);
	void reflectExpandState(ObjectBrowserView* view);

	ObjectBrowserView* objectBrowserView() const;
	void setObjectBrowserView(ObjectBrowserView* v);

	void fitOnDataLoad();
	void viewOperationEndedGlobal() override;

public slots:
	void handleObjectBrowserPress(const QModelIndex& index, const QPoint& globalPos);
	void handleObjectBrowserClick(const QModelIndex& index);
	void handleObjectBrowserDoubleClick(const QModelIndex& index);
	void handleObjectBrowserChange(QStandardItem* changeditem);

	virtual void handleObjectBrowserSelectionChange();
	void handleObjectBrowserSelection(const QModelIndex& current);
	void handleObjectBrowserDeselection(const QModelIndex& previous);

	void moveUpItem(const QModelIndex& index);
	void moveDownItem(const QModelIndex& index);
	void deleteItem(const QModelIndex& index, bool noDraw = false);

	/// Show Property dialog of the item currently selected on object browser.
	void showPropertyDialog(const QModelIndex& index);
	/// Update the operation toolbar.
	void updateOperationToolBar(const QModelIndex& index, QWidget* parent);

	void update2Ds();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

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
