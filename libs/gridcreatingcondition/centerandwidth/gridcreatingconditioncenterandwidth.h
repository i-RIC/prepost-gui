#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_H

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

class QMenu;
class QAction;
class QToolBar;
class GridCreatingConditionCenterAndWidthFinishDefiningCommand;
class GridCreatingConditionCenterAndWidthDefineNewPointCommand;
class GridCreatingConditionCenterAndWidthMoveCommand;
class GridCreatingConditionCenterAndWidthMoveVertexCommand;
class GridCreatingConditionCenterAndWidthAddVertexCommand;
class GridCreatingConditionCenterAndWidthRemoveVertexCommand;
class GridCreatingConditionCenterAndWidthCoordinatesEditDialog;
class GridCreatingConditionCenterAndWidthCoordinatesEditCommand;

class GridCreatingConditionCenterAndWidth : public GridCreatingCondition
{
	Q_OBJECT
private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;
	const static int initialDivision = 100;
public:
	static const int FONTSIZE = 17;
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
	bool create(QWidget* parent);

	bool ready() const;

	void setupActors();
	void setupMenu();
	void setActorProperties(vtkProperty* prop);
	void informSelection(PreProcessorGraphicsViewInterface* v);
	void informDeselection(PreProcessorGraphicsViewInterface* v);
	void viewOperationEnded(PreProcessorGraphicsViewInterface* v);
	void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* v);
	void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* v);
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v);
	void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v);
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v);
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* v);
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActionZValues(const ZDepthRange& /*range*/);
	const QVector<QPointF> polyLine();
	void setPolyLine(const QVector<QPointF>& polyline);
	bool isVertexSelectable(const QVector2D& pos);
	bool isEdgeSelectable(const QVector2D& pos);

	void definePolyLine();
	void createSpline(vtkPoints* points, int division);
	Grid* createGrid();
	void showDialog(QWidget* parent);
	void handleDialogAccepted(QDialog* d);
	void handleDialogRejected(QDialog* d);
	void setIMax(int i);
	void setJMax(int j);
	void setWidth(double w);
	int iMax() {return m_iMax;}
	int jMax() {return m_jMax;}
	double width() {return m_width;}
	double length() {return m_length;}
	bool addToolBarButtons(QToolBar* /*tb*/);
	void clear();
	void showInitialDialog();
protected:
	// @todo not implemented yet.
	void doLoadFromProjectMainFile(const QDomNode& /*node*/);
	// @todo not implemented yet.
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/);
	void loadCenterAndWidthFromProjectMainFile(const QDomNode& /*node*/);
	void saveCenterAndWidthToProjectMainFile(QXmlStreamWriter& /*writer*/);
	void loadExternalData(const QString& /*filename*/);
	void saveExternalData(const QString& /*filename*/);
	void doApplyOffset(double x, double y);

	vtkSmartPointer<vtkPolyLine> m_vtkPolyLine;
	vtkSmartPointer<vtkUnstructuredGrid> m_edgeGrid;
	vtkSmartPointer<vtkUnstructuredGrid> m_vertexGrid;
	vtkSmartPointer<vtkStringArray> m_labelArray;

	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkActor> m_tmpActor;
	vtkSmartPointer<vtkActor2D> m_labelActor;

	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
	vtkSmartPointer<vtkDataSetMapper> m_tmpMapper;
	vtkSmartPointer<vtkLabeledDataMapper> m_labelMapper;

	vtkSmartPointer<vtkParametricSpline> m_spline;
	vtkSmartPointer<vtkCardinalSpline> m_xSpline;
	vtkSmartPointer<vtkCardinalSpline> m_ySpline;
	vtkSmartPointer<vtkCardinalSpline> m_zSpline;

	vtkPoints* m_splinePoints;
	QVector<QPointF> m_polyLine;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_reverseCenterLineAction;
	QMenu* m_rightClickingMenu;

	int m_iMax;
	int m_jMax;
	double m_width;
	double m_length;
	int m_tmpIMax;
	int m_tmpJMax;
	double m_tmpWidth;
private slots:
	void restoreMouseEventMode();
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void deletePolyLine();
	void handleDialogApplied(QDialog* d);
	void reverseCenterLineDirection();
private:
	void updateShapeData();
	void updateMouseEventMode();
	void updateActionStatus();

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
public:
	friend class GridCreatingConditionCreatorCenterAndWidth;
	friend class GridCreatingConditionCenterAndWidthFinishDefiningCommand;
	friend class GridCreatingConditionCenterAndWidthDefineNewPointCommand;
	friend class GridCreatingConditionCenterAndWidthMoveCommand;
	friend class GridCreatingConditionCenterAndWidthMoveVertexCommand;
	friend class GridCreatingConditionCenterAndWidthAddVertexCommand;
	friend class GridCreatingConditionCenterAndWidthRemoveVertexCommand;
	friend class GridCreatingConditionCenterAndWidthCoordinatesEditDialog;
	friend class GridCreatingConditionCenterAndWidthCoordinatesEditCommand;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_H
