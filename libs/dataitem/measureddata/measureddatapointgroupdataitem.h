#ifndef MEASUREDDATAPOINTGROUPDATAITEM_H
#define MEASUREDDATAPOINTGROUPDATAITEM_H

#include "measureddata_api.h"
#include "measureddatapointsetting.h"

#include <guibase/widget/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/misc/targeted/targeteditemi.h>

#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/boolcontainer.h>
#include <misc/opacitycontainer.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkScalarBarWidget.h>

#include <unordered_map>
#include <string>

class MeasuredData;
class MeasuredDataPointDataItem;
class NamedGraphicWindowDataItem;
class vtkLODActor;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;
class LookupTableContainer;

class MEASUREDDATA_API MeasuredDataPointGroupDataItem : public GraphicsWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	MeasuredDataPointGroupDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataPointGroupDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	bool hasTransparentPart() override;
	LookupTableContainer* lookupTable(const std::string& target) const;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doApplyOffset(double x, double y) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void updateVisibility(bool visible) override;

private:
	void setupActors();
	void updateActorSettings();
	void createRangeClippedPolyData();
	void createValueClippedPolyData();
	void setupPointSetting();
	void setupIsolineSetting();
	void setupColorContourSetting();
	void setupColorFringeSetting();
	void setupScalarBarSetting();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	MeasuredDataPointSetting m_setting;

	// for scalar bar
	std::unordered_map<std::string, LookupTableContainer*> m_lookupTables;
	std::unordered_map<std::string, QString> m_colorbarTitleMap;

	vtkLODActor* m_contourActor;
	vtkPolyDataMapper* m_contourMapper;
	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;
	vtkLODActor* m_fringeActor;
	vtkPolyDataMapper* m_fringeMapper;
	vtkActor* m_pointsActor;
	vtkPolyDataMapper* m_pointsMapper;
	vtkActor* m_blackPointsActor;
	vtkPolyDataMapper* m_blackPointsMapper;
	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	vtkSmartPointer<vtkPolyData> m_valueClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_colorContourPolyData;

	class SetSettingCommand;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_H
