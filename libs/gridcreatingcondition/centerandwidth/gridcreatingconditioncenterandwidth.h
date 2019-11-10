#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_H

#include <guibase/polyline/polylinecontroller.h>
#include <guibase/vtktool/vtklabel2dactor.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <vtkSmartPointer.h>
#include <vtkPolyLine.h>
#include <vtkUnstructuredGrid.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkStringArray.h>
#include <vtkDataSetMapper.h>
#include <vtkParametricSpline.h>
#include <vtkCardinalSpline.h>
#include <vtkLabeledDataMapper.h>
#include <QVector>
#include <QPointF>
#include <QPixmap>
#include <QCursor>

#include <vector>

class QMenu;
class QAction;
class QToolBar;

class GridCreatingConditionCenterAndWidth : public GridCreatingCondition
{
	Q_OBJECT

private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;
	const static int initialDivision = 100;

public:
	enum MouseEventMode {
		meNormal,
		meBeforeDefining,
		meDefining,
		meTranslate,
		meTranslatePrepare,
		meMoveVertex,
		meMoveVertexPrepare,
		meAddVertex,
		meAddVertexPrepare,
		meAddVertexNotPossible,
		meRemoveVertexPrepare,
		meRemoveVertexNotPossible,

		meEditVerticesDialog
	};
	GridCreatingConditionCenterAndWidth(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	virtual ~GridCreatingConditionCenterAndWidth();
	bool create(QWidget* parent) override;

	bool ready() const override;

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
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& /*range*/) override;
	std::vector<QPointF> polyLine();
	void setPolyLine(const std::vector<QPointF>& polyline);

	void definePolyLine();
	void createSpline(vtkPoints* points, int division);
	Grid* createGrid();
	void showDialog(QWidget* parent);
	void handleDialogAccepted(QDialog* d);
	void handleDialogRejected(QDialog* d);
	void setIMax(int i);
	void setJMax(int j);
	void setWidth(double w);
	int iMax() const {return m_iMax;}
	int jMax() const {return m_jMax;}
	double width() const {return m_width;}
	double length() const {return m_length;}
	bool addToolBarButtons(QToolBar* /*tb*/) override;
	void clear() override;
	void showInitialDialog() override;

protected:
	// @todo not implemented yet.
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override;
	// @todo not implemented yet.
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override;
	void loadCenterAndWidthFromProjectMainFile(const QDomNode& /*node*/);
	void saveCenterAndWidthToProjectMainFile(QXmlStreamWriter& /*writer*/);
	void loadExternalData(const QString& /*filename*/) override;
	void saveExternalData(const QString& /*filename*/) override;
	void doApplyOffset(double x, double y) override;

	PolyLineController m_polyLineController;
	vtkLabel2DActor m_upstreamActor;
	vtkLabel2DActor m_downstreamActor;

	vtkSmartPointer<vtkActor> m_previewActor;
	vtkSmartPointer<vtkDataSetMapper> m_previewMapper;
	Grid* m_previewGrid;

	vtkSmartPointer<vtkParametricSpline> m_spline;
	vtkSmartPointer<vtkCardinalSpline> m_xSpline;
	vtkSmartPointer<vtkCardinalSpline> m_ySpline;
	vtkSmartPointer<vtkCardinalSpline> m_zSpline;

	vtkPoints* m_splinePoints;

	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_reverseCenterLineAction;
	QAction* m_importCenterLineAction;
	QAction* m_exportCenterLineAction;

	QMenu* m_rightClickingMenu;

	int m_iMax;
	int m_jMax;
	double m_width;
	double m_length;

	int m_oldIMax;
	int m_oldJMax;
	double m_oldWidth;

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
	void updateMouseEventMode();
	void updateActionStatus();

	void pushUpdateShapeCommand(QUndoCommand* com, bool renderRedoOnly = false);

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	MouseEventMode m_mouseEventMode;

	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;
	double m_distance;
	int m_selectedVertexId;
	int m_selectedEdgeId;

	bool m_isAccepted;
	bool m_isGridCreated;

private:
	class UpdateShapeCommand;
	class AddVertexCommand;
	class EditCoordinatesCommand;
	class FinishDefiningCommand;
	class MoveCommand;
	class MoveVertexCommand;
	class PushVertexCommand;
	class RemoveVertexCommand;

	class CoordinatesEditor;

public:
	friend class GridCreatingConditionCreatorCenterAndWidth;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_H
