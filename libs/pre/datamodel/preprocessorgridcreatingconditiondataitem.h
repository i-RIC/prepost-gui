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

	GridCreatingCondition* condition() const override;
	void setCondition(GridCreatingCondition* condition) override;

	PreProcessorGridTypeDataItemInterface* gridTypeDataItem() const override;

	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* /*tb*/) override;
	//void handleStandardItemChange() override;
	void handleStandardItemClicked() override;
	void handleStandardItemDoubleClicked() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void viewOperationEnded(VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	QStringList containedFiles() override;
	void moveGrid();
	QAction* createAction() override {return m_createAction;}
	QAction* switchAction() override {return m_switchAlgorithmAction;}
	QAction* deleteAction() {return m_deleteAction;}
	QAction* clearAction() override {return m_clearAction;}
	QMenu* menu();
	virtual void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(3);}

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdate2Ds() override;
	void doApplyOffset(double x, double y) override;

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
