#ifndef GRIDCREATINGCONDITIONPOISSON_H
#define GRIDCREATINGCONDITIONPOISSON_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

class GridCreatingConditionPoisson : public GridCreatingCondition
{
	Q_OBJECT

public:
	GridCreatingConditionPoisson(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionPoisson();

	bool init() override;
	void showInitialDialog() override;
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

	void buildBankLines();

	void importCenterLine();
	void exportCenterLine();
	void editCenterLineCoordinates();

	void importLeftBankLine();
	void exportLeftBankLine();
	void editLeftBankLineCoordinates();

	void importRightBankLine();
	void exportRightBankLine();
	void editRightBankLineCoordinates();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

	void doApplyOffset(double x, double y) override;

	void pushUpdateLabelsCommand(QUndoCommand* com, bool renderRedoOnly = false);

	class Impl;
	Impl* impl;

	class AddVertexCommand;
	class DefineNewPointCommand;
	class FinishDefiningCommand;
	class MoveCommand;
	class MoveVertexCommand;
	class RemoveVertexCommand;
	class UpdateLabelsCommand;
	class EditCoordinatesCommand;

	class CoordinatesEditor;
};

#ifdef _DEBUG
	#include "private/gridcreatingconditionpoisson_impl.h"
#endif // _DEBUG

#endif // GRIDCREATINGCONDITIONPOISSON_H
