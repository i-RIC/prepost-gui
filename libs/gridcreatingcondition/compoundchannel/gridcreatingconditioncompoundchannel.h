#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <misc/zdepthrange.h>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkUnstructuredGrid.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <QVector>
#include <QPointF>
#include <QPixmap>
#include <QCursor>
#include <QVariant>
#include <QList>

class QMenu;
class QAction;
class QToolBar;

class GridCreatingConditionCreatorCompoundChannel;
class GridCreatingConditionCompoundChannelAbstractPolygon;
class GridCreatingConditionCompoundChannelGridRegionPolygon;
class GridCreatingConditionCompoundChannelLowWaterChannelPolygon;
class GridCreatingConditionCompoundChannelAbstractLine;
class GridCreatingConditionCompoundChannelCenterLine;
class GridCreatingConditionCompoundChannelSpline;

class GridCreatingConditionCompoundChannelTemporaryGrid
{

public:
	GridCreatingConditionCompoundChannelTemporaryGrid(int isize, int jsize) {
		m_isize = isize;
		m_jsize = jsize;
		QPointF p(0, 0);
		for (int j = 0; j < jsize; ++j) {
			for (int i = 0; i < isize; ++i) {
				m_point.append(p);
			}
		}
	}

	QPointF& point(int i, int j) {return m_point[index(i, j)];}
	int isize() const {return m_isize;}
	int jsize() const {return m_jsize;}

private:
	int index(int i, int j) {return m_isize * j + i;}
	QVector<QPointF> m_point;
	int m_isize;
	int m_jsize;
};


class GridCreatingConditionCompoundChannel : public GridCreatingCondition
{
	Q_OBJECT

private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;

public:
	enum SelectMode {
		smNone,
		smPolygon,
		smLine
	};
	enum Status {
		stDefiningRegion,
		stDefiningLowWaterRegion,
		stDefiningCenterLine,
		stNormal
	};

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

		meTranslateDialog,
		meEditVerticesDialog
	};
	GridCreatingConditionCompoundChannel(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	virtual ~GridCreatingConditionCompoundChannel();

	void setupMenu() override;
	bool addToolBarButtons(QToolBar* /*parent*/) override;
	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* /*v*/) override;
	void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void definePolygon(bool doubleClick);
	void defineLine(bool doubleClick);
	const QColor& color() const;
	void setupScalarArray();
	void clear() override;
	bool ready() const override;
	bool create(QWidget* parent) override;
	void update2Ds() override;
	void showInitialDialog() override;

private slots:
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void restoreMouseEventMode();
	void cancel();
	void reverseCenterLine();

private:
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x, double y) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void updateFilename() override;

	bool checkCondition();
	bool selectObject(QPoint point);
	bool activePolygonHasFourVertices();
	bool activePolylineHasThreeVertices();
	void initParams();
	Grid* createGrid();
	Grid* createGridCore(const GridCreatingConditionCompoundChannelSpline& leftEdge, const GridCreatingConditionCompoundChannelSpline& leftBank, const GridCreatingConditionCompoundChannelSpline& centerLine, const GridCreatingConditionCompoundChannelSpline& rightBank, const GridCreatingConditionCompoundChannelSpline& rightEdge);
	void setupInitialGrid(GridCreatingConditionCompoundChannelTemporaryGrid& grid, const GridCreatingConditionCompoundChannelSpline& leftEdge, const GridCreatingConditionCompoundChannelSpline& leftBank, const GridCreatingConditionCompoundChannelSpline& centerLine, const GridCreatingConditionCompoundChannelSpline& rightBank, const GridCreatingConditionCompoundChannelSpline& rightEdge);
	void applyConstraintToGrid(GridCreatingConditionCompoundChannelTemporaryGrid& grid, const GridCreatingConditionCompoundChannelSpline& leftEdge, const GridCreatingConditionCompoundChannelSpline& leftBank, const GridCreatingConditionCompoundChannelSpline& centerLine, const GridCreatingConditionCompoundChannelSpline& rightBank, const GridCreatingConditionCompoundChannelSpline& rightEdge);
	void relaxGrid(GridCreatingConditionCompoundChannelTemporaryGrid& grid, double relax);

	GridCreatingConditionCompoundChannelSpline getLeftBank(const QVector<QPointF>& centerLine);
	GridCreatingConditionCompoundChannelSpline getRightBank(const QVector<QPointF>& centerLine);
	GridCreatingConditionCompoundChannelSpline getLowWaterEdgeSpline(int headEdge, int middleEdge, int tailEdge, bool discardHead, bool discardTail, const QVector2D& hintVec);

	GridCreatingConditionCompoundChannelSpline getLeftEdge(const QVector<QPointF>& leftBank);
	GridCreatingConditionCompoundChannelSpline getRightEdge(const QVector<QPointF>& rightBank);
	GridCreatingConditionCompoundChannelSpline getRegionEdgeSpline(int splitEdge);
	GridCreatingConditionCompoundChannelSpline getRegionEdgeSpline(int headEdge, int middleEdge, int tailEdge, bool discardHead, bool discardTail, const QVector2D& hintVec);

	void updateMouseEventMode();
	void updateActionStatus();
	ZDepthRange m_depthRange;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	Status m_status;
	SelectMode m_selectMode;
	MouseEventMode m_mouseEventMode;

	GridCreatingConditionCompoundChannelGridRegionPolygon* m_gridRegionPolygon;
	GridCreatingConditionCompoundChannelLowWaterChannelPolygon* m_lowWaterChannelPolygon;
	GridCreatingConditionCompoundChannelCenterLine* m_centerLine;

	GridCreatingConditionCompoundChannelAbstractPolygon* m_selectedPolygon;
	GridCreatingConditionCompoundChannelAbstractLine* m_selectedLine;

	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_reverseCenterLineAction;
	QMenu* m_rightClickingMenu;
	QColor m_color;

	bool m_inhibitSelect;
	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;

	double lastX;
	double lastY;
	bool m_canceled;

	int m_streamWiseDivision;
	int m_leftDivision;
	int m_rightDivision;
	int m_centerDivision;

	double m_relaxation;
	int m_iterations;

private:
	class AddPolygonVertexCommand;
	class AddPolyLineVertexCommand;
	class DefinePolygonNewPointCommand;
	class DefinePolyLineNewPointCommand;
	class EditPolygonCoordinatesCommand;
	class EditPolyLineCoordinatesCommand;
	class MovePolygonCommand;
	class MovePolygonVertexCommand;
	class MovePolyLineCommand;
	class MovePolyLineVertexCommand;
	class RemovePolygonVertexCommand;
	class RemovePolyLineVertexCommand;
	class SwitchStatusCommand;

	class PolygonCoordinatesEditor;
	class PolylineCoordinatesEditor;

public:
	friend class GridCreatingConditionCompoundChannelCreator;
	friend class GridCreatingConditionCompoundChannelAbstractPolygon;
	friend class GridCreatingConditionCompoundChannelAbstractLine;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_H
