#ifndef GRIDCREATINGCONDITIONRECTANGULARREGIONLONLAT_H
#define GRIDCREATINGCONDITIONRECTANGULARREGIONLONLAT_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkStructuredGrid.h>

#include <QPointF>
#include <QPolygonF>

class QMenu;
class Structured2DGrid;

class GridCreatingConditionRectangularRegionLonLat : public GridCreatingCondition
{
	Q_OBJECT
public:
	enum MouseEventMode {
		meNormal,
		meDragging,
		meCreateDialog
	};
	GridCreatingConditionRectangularRegionLonLat(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionRectangularRegionLonLat();
	bool create(QWidget* parent);
	bool ready() const;
	void setupMenu();
	void setupActors();
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActionZValues(const ZDepthRange& range);
	const QPolygonF rectangle();
	void setRectangle(const QPolygonF& rec);
	bool createGrid(double xmin, double xmax, double ymin, double ymax, double step);
	void previewGrid(double xmin, double xmax, double ymin, double ymax, double step);
	void informSelection(PreProcessorGraphicsViewInterface* v);
	void informDeselection(PreProcessorGraphicsViewInterface* v);
	void clear();
	void showInitialDialog();
protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/);
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/);

	vtkSmartPointer<vtkActor> m_rectangularActor;
	vtkSmartPointer<vtkActor> m_rectangularFrameActor;

	QPoint m_startPoint;
	QPoint m_endPoint;

	double m_xMin;
	double m_xMax;
	double m_yMin;
	double m_yMax;
	double m_stepSize;

//	bool m_isDialogOpening;
//	bool m_isCreatingBoundingBox;
private slots:
	void hideTmpGrid();
private:
	void createRectangularRegionFromMouse();
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void createRectangularRegion(double xmin, double xmax, double ymin, double ymax);
	Structured2DGrid* createGridInner(double xmin, double xmax, double ymin, double ymax, double stepSize);
	MouseEventMode m_mouseEventMode;
	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;
};

#endif // GRIDCREATINGCONDITIONRECTANGULARREGION_H
