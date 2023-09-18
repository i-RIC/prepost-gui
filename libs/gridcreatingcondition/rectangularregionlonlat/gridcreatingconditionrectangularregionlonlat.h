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
class v4Structured2dGrid;

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
	bool create(QWidget* parent) override;
	bool ready() const override;
	void setupMenu() override;
	void setupActors() override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;
	const QPolygonF rectangle();
	void setRectangle(const QPolygonF& rec);
	bool createGrid(double xmin, double xmax, double ymin, double ymax, double step);
	void previewGrid(double xmin, double xmax, double ymin, double ymax, double step);
	void informSelection(PreProcessorGraphicsViewI* v) override;
	void informDeselection(PreProcessorGraphicsViewI* v) override;
	void clear() override;
	void showInitialDialog() override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override;

	void doApplyOffset(double x, double y) override;

	vtkSmartPointer<vtkActor> m_rectangularActor;
	vtkSmartPointer<vtkActor> m_rectangularFrameActor;

	v4Structured2dGrid* m_previewGrid;
	vtkSmartPointer<vtkActor> m_previewActor;

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
	void hidePreviewGrid();

private:
	void createRectangularRegionFromMouse();
	void updateMouseCursor(PreProcessorGraphicsViewI* v);
	void createRectangularRegion(double xmin, double xmax, double ymin, double ymax);

	v4Structured2dGrid* createGridInner(double xmin, double xmax, double ymin, double ymax, double stepSize);
	MouseEventMode m_mouseEventMode;
	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;
};

#endif // GRIDCREATINGCONDITIONRECTANGULARREGION_H
