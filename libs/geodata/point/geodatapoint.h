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

	void setPointSize(int pointSize);

	void setupMenu() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void informSelection(PreProcessorGraphicsViewI* v) override;
	void informDeselection(PreProcessorGraphicsViewI* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewI* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) override;
	bool inNormalMode() const override;
	bool isDefined() const override;

	void showInitialDialog() override;

	QPointF point() const;
	void setPoint(const QPointF& point);

	QDialog* propertyDialog(QWidget* parent) override;
	void showPropertyDialog() override;

	GeoDataProxy* getProxy() override;

	class DisplaySettingWidget;

signals:
	void modified();

public slots:
	void restoreMouseEventMode();

private slots:
	void editCoordinates();

private:
	void doApplyOffset(double x, double y) override;

	MouseEventMode mouseEventMode() const;
	void setMouseEventMode(MouseEventMode mode);

	void updateMouseCursor(PreProcessorGraphicsViewI* v);
	void updateScalarValues() override;
	void updateActorSetting() override;
	void updateMouseEventMode();

	bool isReady() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;

	QAction* coordEditAction() const;

	class EditCoordinatesCommand;
	class FinishPointDefiningCommand;
	class MoveVertexCommand;
	class SetVertexCommand;

	class CoordinatesEditor;
	class DisplaySetting;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPointGroup;
	friend class GeoDataPointGroupPoint;
	friend class GeoDataPointProxy;
};

#endif // GEODATAPOINT_H
