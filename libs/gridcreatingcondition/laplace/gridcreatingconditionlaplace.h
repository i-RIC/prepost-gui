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

	enum class DeploySetting {
		Ratio,
		Poisson,
	};

	class DeployParameter {
	public:
		DeploySetting setting;
		double manualP;
		double manualQ;

		DeployParameter();
	};

	const static double POISSONPARAM_FACTOR;

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

	void updateDeployParameterForSelectedSubRegion(const DeployParameter& p);

	public slots:
	void requestPreviewSubRegionGrid();
	void hidePreviewGrid();

private slots:
	void buildBankLines();
	void interpolateModeSprine();
	void interpolateModeLinear();
	void newEdgeMode(bool on);
	void joinRegions();
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoorinates();
	bool wholeRegionDivisionSetting(bool gridCreateButton = false);
	void divisionSetting();
	void deploySetting();
	void subRegionDeploySetting();
	void clearDivisionSetting();

	void informCommonRatioUpdate();
	void informPreviewGridPointsUpdate(int i, int j);

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
	class SubRegionDeploySettingCommand;

	class UpdateEdgeLinesForEdgeSelectionCommand;

	class CenterLineCoordinatesEditor;
	class EdgeCoordinatesEditor;
	class BankLinesBuilder;
	class BuildSubGridPointsThread;
	class CommonRatioOptimizeThread;
};

#ifdef _DEBUG
	#include "private/gridcreatingconditionlaplace_impl.h"
#endif // _DEBUG

#endif // GRIDCREATINGCONDITIONLAPLACE_H
