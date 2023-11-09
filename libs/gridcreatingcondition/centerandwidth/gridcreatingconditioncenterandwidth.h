#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

#include <vector>

class QMenu;
class QPointF;
class QToolBar;

class GridCreatingConditionCenterAndWidth : public GridCreatingCondition
{
	Q_OBJECT

private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;
	const static int initialDivision = 100;

public:
	GridCreatingConditionCenterAndWidth(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionCenterAndWidth();

	bool create(QWidget* parent) override;
	bool ready() const override;
	void setupActors() override;
	void setupMenu() override;
	void informSelection(PreProcessorGraphicsViewI* v) override;
	void informDeselection(PreProcessorGraphicsViewI* v) override;
	void viewOperationEnded(PreProcessorGraphicsViewI* v) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	std::vector<QPointF> polyLine();
	void setPolyLine(const std::vector<QPointF>& polyline);

	void definePolyLine();
	void createSpline(vtkPoints* points, int division);
	v4InputGrid* createGrid();
	void showDialog(QWidget* parent);
	void handleDialogAccepted(QDialog* d);
	void handleDialogRejected(QDialog* d);
	void setIMax(int i);
	void setJMax(int j);
	void setWidth(double w);

	int iMax() const;
	int jMax() const;
	double width() const;
	double length() const;

	bool addToolBarButtons(QToolBar* tb) override;
	void clear() override;
	void showInitialDialog() override;

protected:
	void loadCenterAndWidthFromProjectMainFile(const QDomNode& node);
	void saveCenterAndWidthToProjectMainFile(QXmlStreamWriter& writer);

private slots:
	void restoreMouseEventMode();
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void deletePolyLine();
	void handleDialogApplied(QDialog* d);
	void reverseCenterLineDirection();

	void importCenterLine();
	void exportCenterLine();

private:
	void updateShapeData();

	void pushUpdateShapeCommand(QUndoCommand* com, bool renderRedoOnly = false);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;

	class UpdateShapeCommand;
	class AddVertexCommand;
	class EditCoordinatesCommand;
	class FinishDefiningCommand;
	class MoveCommand;
	class MoveVertexCommand;
	class PushVertexCommand;
	class RemoveVertexCommand;

	class CoordinatesEditor;
	class Setting;

	class Impl;
	Impl* impl;

public:
	friend class GridCreatingConditionCreatorCenterAndWidth;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_H
