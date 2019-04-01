#ifndef GRIDCREATINGCONDITIONLAPLACE_H
#define GRIDCREATINGCONDITIONLAPLACE_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

class GridCreatingConditionLaplace : public GridCreatingCondition
{
	Q_OBJECT

public:
	enum class DivisionMode {
		Auto,
		Equally,
		Geometric,
	};

	GridCreatingConditionLaplace(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionLaplace() override;

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
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

private slots:
	void buildBankLines();
	void switchModeToShape();
	void switchModeToDivide();
	void interpolateModeSprine();
	void interpolateModeLinear();
	void newEdgeMode(bool on);
	void joinRegions();
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoorinates();
	void wholeRegionDivisionSetting();
	void divisionSetting();
	void deploySetting();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

	void doApplyOffset(double x, double y) override;

	class Impl;
	Impl* impl;

	class PolyLineAddVertexCommand;
	class PolyLineMoveVertexCommand;
	class PolyLineRemoveVertexCommand;
	class PolyLineEditCoordinatesCommand;
	class PolyLineDefineNewVertexCommand;

	class CenterLineStartDefinitionCommand;
	class CenterLineFinishDefiningCommand;
	class CenterLineUpdateLabelsAndSplineCommand;

	class CtrlPointMoveCommand;
	class NewEdgeStartDefinitionCommand;
	class EdgeEditCoordinatesCommand;
	class EdgeSetInterpolationModeCommand;
	class WholeRegionDivisionSettingCommand;
	class DivisionSettingCommand;
	class DeploySettingCommand;

	class UpdateEdgeLinesForEdgeSelectionCommand;

	class CenterLineCoordinatesEditor;
	class EdgeCoordinatesEditor;
	class BankLinesBuilder;
};

#ifdef _DEBUG
	#include "private/gridcreatingconditionlaplace_impl.h"
#endif // _DEBUG

#endif // GRIDCREATINGCONDITIONLAPLACE_H
