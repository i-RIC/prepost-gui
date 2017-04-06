#ifndef GRIDCREATINGCONDITIONPOISSON_H
#define GRIDCREATINGCONDITIONPOISSON_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

class GridCreatingConditionPoisson : public GridCreatingCondition
{
	Q_OBJECT

public:
	GridCreatingConditionPoisson(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionPoisson();

	bool create(QWidget* parent) override;
	bool ready() const override;
	void clear() override;

	void setupActors() override;
	void setupMenu() override;

	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* v) override;
	void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& /*range*/) override;

private slots:
	void restoreMouseEventMode();
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();

	void importCenterLine();
	void exportCenterLine();

	void importLeftBankLine();
	void exportLeftBankLine();

	void importRightBankLine();
	void exportRightBankLine();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void pushUpdateLabelsCommand(QUndoCommand* com);

	class Impl;
	Impl* impl;

	class AddVertexCommand;
	class DefineNewPointCommand;
	class FinishDefiningCommand;
	class MoveCommand;
	class MoveVertexCommand;
	class RemoveVertexCommand;
	class UpdateLabelsCommand;
};

#ifdef _DEBUG
	#include "private/gridcreatingconditionpoisson_impl.h"
#endif // _DEBUG

#endif // GRIDCREATINGCONDITIONPOISSON_H
