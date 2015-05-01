#ifndef PREPROCESSORGRIDCREATINGCONDITIONDATAITEM_H
#define PREPROCESSORGRIDCREATINGCONDITIONDATAITEM_H

#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>

class GridCreatingCondition;
class QAction;
class Grid;

class PreProcessorGridCreatingConditionDataItem : public PreProcessorGridCreatingConditionDataItemInterface
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorGridCreatingConditionDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridCreatingConditionDataItem();
	void setCondition(GridCreatingCondition* condition);
	GridCreatingCondition* condition() {return m_condition;}
	void addCustomMenuItems(QMenu* menu);
	bool addToolBarButtons(QToolBar* /*tb*/);
	//void handleStandardItemChange();
	void handleStandardItemClicked();
	void handleStandardItemDoubleClicked();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void viewOperationEnded(VTKGraphicsView* v);
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	QStringList containedFiles();
	void moveGrid();
	QAction* createAction() {return m_createAction;}
	QAction* switchAction() {return m_switchAlgorithmAction;}
	QAction* deleteAction() {return m_deleteAction;}
	QAction* clearAction() {return m_clearAction;}
	QMenu* menu();
	virtual void updateZDepthRangeItemCount() {m_zDepthRange.setItemCount(3);}
protected:
	void assignActionZValues(const ZDepthRange& range);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void innerUpdate2Ds();
	void doApplyOffset(double x, double y);
public slots:
	void silentDeleteCondition();
private slots:
	void resetCondition();
	void deleteCondition();
	void createGrid();
	void handleNewGrid(Grid* newgrid);
	void handleTmpGrid(Grid* tmpgrid);
	void switchAlgorithm();
signals:
	void gridCreated();
private:
	GridCreatingCondition* m_condition;
	/// Action to create grid.
	QAction* m_createAction;
	/// Action to switch grid creating algorithm.
	QAction* m_switchAlgorithmAction;
	/// Action to delete grid creating condition.
	QAction* m_deleteAction;
	/// Action to clear grid creating condition.
	QAction* m_clearAction;
public:
	friend class GridCreatingCondition;
};

#endif // PREPROCESSORGRIDCREATINGCONDITIONDATAITEM_H
