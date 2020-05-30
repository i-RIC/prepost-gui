#ifndef GEODATAPOINT_H
#define GEODATAPOINT_H

#include "gd_point_global.h"

#include <geodata/polydata/geodatapolydata.h>

class QAction;
class QMenu;
class QToolBar;

class GD_POINT_EXPORT GeoDataPoint : public GeoDataPolyData
{
	Q_OBJECT

public:
	enum MouseEventMode {
		meNormal,
		meBeforeDefining,
		meDefining,
		meMoveVertex,
		meMoveVertexPrepare,
		meEditCoordinatesDialog
	};

	GeoDataPoint(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPoint() override;

	void setupMenu() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) override;
	bool inNormalMode() const override;
	bool isDefined() const override;

	void showInitialDialog() override;

	QPointF point() const;
	void setPoint(const QPointF& point);

signals:
	void modified();

public slots:
	void restoreMouseEventMode();

private slots:
	void editCoordinates();

private:
	void doApplyOffset(double x, double y) override;
	int iRICLibType() const override;

	MouseEventMode mouseEventMode() const;
	void setMouseEventMode(MouseEventMode mode);

	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateActorSettings() override;
	void updateMouseEventMode();

	bool isReady() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;

	QAction* coordEditAction() const;

	class FinishPointDefiningCommand;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPointGroup;
	friend class GeoDataPointGroupPoint;
};

#ifdef _DEBUG
	#include "private/geodatapoint_impl.h"
#endif // _DEBUG

#endif // GEODATAPOINT_H
