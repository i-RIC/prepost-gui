#ifndef GRIDCREATINGCONDITION_H
#define GRIDCREATINGCONDITION_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include <QString>

class QWidget;
class QKeyEvent;
class QMouseEvent;
class QMenu;
class QDialog;
class QToolBar;
class QUndoCommand;
class Grid;
class GridCreatingConditionCreator;
class PreProcessorGridCreatingConditionDataItemInterface;
class PreProcessorGraphicsViewInterface;
class PreProcessorDataModelInterface;
class PreProcessorWindowInterface;
class ZDepthRange;
class vtkRenderer;
class vtkActorCollection;
class vtkActor2DCollection;
class vtkPoints;

/// The container class that holds grid creating condition.
class GUICOREDLL_EXPORT GridCreatingCondition : public ProjectDataItem
{
	Q_OBJECT

public:
	static const int MAXGRIDSIZE = 100000000;
	static const QString XML_FILENAME;

	GridCreatingCondition(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingCondition();
	/// Create a new grid.
	virtual bool create(QWidget* parent) = 0;
	/// Ready to create a grid.
	virtual bool ready() const = 0;
	GridCreatingConditionCreator* creator() const;
	const QString& name() const;

	bool loadFromExportData(const QString& folder);
	bool saveToExportData(const QString& folder);
	static QStringList exportDataFileList();

	QMenu* menu() const;
	/// setup VTK actors.
	virtual bool init() {return true;}
	virtual void showInitialDialog() {}
	virtual void setupActors() {}
	virtual void setupMenu() {}
	virtual void handleStandardItemChange() {}
	virtual void handleStandardItemDoubleClicked() {}
	virtual void informSelection(PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void informDeselection(PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void viewOperationEnded(PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void addCustomMenuItems(QMenu* /*menu*/) {}
	virtual void updateZDepthRangeItemCount(ZDepthRange& range);
	virtual void assignActorZValues(const ZDepthRange& /*range*/) {}
	virtual bool addToolBarButtons(QToolBar* /*tb*/) {return false;}
	virtual void update2Ds() {}
	/// Clear the condition, to make it the default status.
	virtual void clear() = 0;
	void updateVisibility();
	void updateVisibilityWithoutRendering();
	PreProcessorWindowInterface* preProcessorWindow() const;
	void applyOffset(double x, double y) {doApplyOffset(x, y);}

signals:
	void gridCreated(Grid* g);
	void tmpGridCreated(Grid* g);

protected:
	ProjectData* projectData();
	vtkRenderer* renderer();
	vtkActorCollection* actorCollection();
	vtkActor2DCollection* actor2DCollection();
	virtual void updateFilename() {}
	PreProcessorGridCreatingConditionDataItemInterface* gccDataItem() const;
	PreProcessorGraphicsViewInterface* graphicsView() const;
	PreProcessorDataModelInterface* dataModel() const;
	void renderGraphicsView();
	void pushCommand(QUndoCommand* com);
	void pushRenderCommand(QUndoCommand* com);
	void pushRenderRedoOnlyCommand(QUndoCommand* com);
	PreProcessorGridCreatingConditionDataItemInterface* m_conditionDataItem;
	GridCreatingConditionCreator* m_creator;
	QMenu* m_menu;

private:
	virtual void doLoadFromProjectMainFile(const QDomNode& node) = 0;
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer) = 0;
	virtual void loadExternalData(const QString& filename);
	virtual void saveExternalData(const QString& filename);
};

#endif // GRIDCREATINGCONDITION_H
