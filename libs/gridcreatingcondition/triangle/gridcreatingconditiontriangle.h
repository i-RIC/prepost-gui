#ifndef GRIDCREATINGCONDITIONTRIANGLE_H
#define GRIDCREATINGCONDITIONTRIANGLE_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <misc/zdepthrange.h>
#include <QPixmap>
#include <QCursor>
#include <QColor>

class QMenu;
class QAction;
class QToolBar;

class GridCreatingConditionCreatorTriangle;
class GridCreatingConditionTriangleAbstractPolygon;
class GridCreatingConditionTriangleGridRegionPolygon;
class GridCreatingConditionTriangleRemeshPolygon;
class GridCreatingConditionTriangleHolePolygon;
class GridCreatingConditionTriangleAbstractLine;
class GridCreatingConditionTriangleDivisionLine;

class GridCreatingConditionTriangle : public GridCreatingCondition
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
	/// Constructor
	GridCreatingConditionTriangle(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	virtual ~GridCreatingConditionTriangle();
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
	const QColor& color() const {return m_color;}
	QColor doubleToColor(double d) const;
	void setupScalarArray();
	void clear() override;
	bool ready() const override {return true;}
	bool create(QWidget* parent) override;
	void showInitialDialog() override;

private slots:
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void restoreMouseEventMode();
	void addRefinementPolygon();
	void addHolePolygon();
	void addDivisionLine();
	void redivideBreakline();
	void deletePolygon(bool force = false);
	void deleteLine(bool force = false);
	void cancel() {m_canceled = true;}

protected:
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void setColor(const QColor& color);
	void setColor(double r, double g, double b);
	void updateFilename() override {
		setFilename("gridcreatingcondition.dat");
	}
	void doApplyOffset(double x, double y) override;

private:
	void unionLines(QPolygonF& gridPol, const QVector<QPointF>& line);
	bool checkCondition();
	void divideDivisionLine(GridCreatingConditionTriangleDivisionLine& line, int n);
	bool selectObject(QPoint point);
	void deselectAll();
	bool activePolygonHasFourVertices();
	bool activePolylineHasThreeVertices();
	void initParams();
	Grid* createGrid();
	void updateMouseEventMode();
	void updateActionStatus();
	ZDepthRange m_depthRange;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	SelectMode m_selectMode;
	MouseEventMode m_mouseEventMode;

	GridCreatingConditionTriangleGridRegionPolygon* m_gridRegionPolygon;
	QList<GridCreatingConditionTriangleRemeshPolygon*> m_remeshPolygons;
	QList<GridCreatingConditionTriangleHolePolygon*> m_holePolygons;
	QList<GridCreatingConditionTriangleDivisionLine*> m_divisionLines;

	GridCreatingConditionTriangleAbstractPolygon* m_selectedPolygon;
	GridCreatingConditionTriangleAbstractLine* m_selectedLine;

	QAction* m_defineModeAction;
	QAction* m_refineModeAction;
	QAction* m_holeModeAction;
	QAction* m_divlineModeAction;
	QAction* m_deleteAction;

	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_editColorSettingAction;
	QAction* m_editMaxAreaAction;
	QAction* m_redivideBreaklineAction;
	QMenu* m_rightClickingMenu;
	QColor m_color;

	bool m_angleConstraint;
	double m_angle;
	bool m_areaConstraint;
	double m_area;

	bool m_inhibitSelect;
	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;

	double lastX;
	double lastY;
	bool m_canceled;

private:
	class AddDivisionLineCommand;
	class AddHolePolygonCommand;
	class AddPolygonVertexCommand;
	class AddPolyLineVertexCommand;
	class AddRemeshPolygonCommand;
	class DefinePolygonNewPointCommand;
	class EditPolygonCoordinatesCommand;
	class EditPolyLineCoordinatesCommand;
	class FinishPolygonDefiningCommand;
	class FinishPolyLineDefiningCommand;
	class DefinePolyLineNewPointCommand;
	class MovePolygonCommand;
	class MovePolygonVertexCommand;
	class MovePolyLineCommand;
	class MovePolyLineVertexCommand;
	class RemovePolygonVertexCommand;
	class RemovePolyLineVertexCommand;

public:
	friend class GridCreatingConditionTriangleAbstractLine;
	friend class GridCreatingConditionTriangleAbstractPolygon;
	friend class GridCreatingConditionTriangleCreator;
	friend class GridCreatingConditionTrianglePolygonCoordinatesEditDialog;
	friend class GridCreatingConditionTrianglePolyLineCoordinatesEditDialog;

	friend class GridCreatingConditionTrianglePolylineFinishDefiningCommand;

};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_H
