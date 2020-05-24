#ifndef GEODATAPOINT_H
#define GEODATAPOINT_H

#include "gd_point_global.h"
#include "geodatapointcolorsimplesettingdialog.h"

#include <guicore/pre/geodata/geodata.h>

class vtkActor;
class vtkCellArray;
class vtkMapper;
class vtkPoints;
class vtkPolyData;

class QAction;
class QMenu;
class QToolBar;

class GD_POINT_EXPORT GeoDataPoint : public GeoData
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

	void setCaptionAndEmitEdited(const QString& caption);
	void setVariantValueAndEmitEdited(const QVariant& value);

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

	void definePoint(bool doubleClick, bool noEditVal = false);
	QColor color() const;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* d) override;

	void showInitialDialog() override;

	const QVariant& variantValue() const;
	void setVariantValue(const QVariant& v, bool disableInform = false);

	QPointF point() const;
	void setPoint(const QPointF& point);

	bool getValueRange(double* min, double* max) override;
	void updateFilename() override;

signals:
	void modified();
	void nameAndValueEdited();

public slots:
	void editName();
	void editValue();
	void editNameAndValue();
	void restoreMouseEventMode();
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex) override;
	void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after) override;

private slots:
	void editCoordinates();
	void editColorSetting();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x, double y) override;
	int iRICLibType() const override;

	void setColor(const QColor& color);

	MouseEventMode mouseEventMode() const;
	void setMouseEventMode(MouseEventMode mode);

	GeoDataPointColorSimpleSettingDialog::Setting colorSetting() const;
	void setColorSetting(const GeoDataPointColorSimpleSettingDialog::Setting& setting);

	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateActorSettings();
	void initParams();
	void updateMouseEventMode();
	void emitNameAndValueEdited();

	QAction* editNameAction() const;
	QAction* editValueAction() const;
	QAction* editNameAndValueAction() const;
	QAction* coordEditAction() const;

	class EditNameAndValueCommand;
	class EditPropertyCommand;
	class FinishPointDefiningCommand;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPointGroup;
};

#ifdef _DEBUG
	#include "private/geodatapoint_impl.h"
#endif // _DEBUG

#endif // GEODATAPOINT_H
