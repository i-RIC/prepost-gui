#ifndef MEASUREDDATAVECTORGROUPDATAITEM_H
#define MEASUREDDATAVECTORGROUPDATAITEM_H

#include "../datamodel/graphicswindowdataitem.h"
#include "../project/measureddata.h"
#include "../misc/targeted/targeteditemi.h"

#include <misc/enumcontainert.h>
#include <misc/colorcontainer.h>
#include <misc/doublecontainer.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkGlyph3D.h>
#include <vtkHedgeHog.h>
#include <vtkConeSource.h>
#include <vtkWarpVector.h>
#include <vtkAppendPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkActor2D.h>
#include <vtkTextActor.h>
#include <vtkClipPolyData.h>

#include <QColor>

class MeasuredDataVectorDataItem;
class NamedGraphicWindowDataItem;
class vtkPointSet;

class MeasuredDataVectorGroupDataItem : public GraphicsWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	const static int STANDARD_LENGTH = 100;
	const static int AUTO_AVERAGECOUNT = 20;
	const static double MINLIMIT;

	class Setting : public CompositeContainer
	{
	public:
		Setting();
		Setting(const Setting& s);
		MeasuredDataVectorGroupDataItem::Setting& operator=(const Setting& s);

		StringContainer scalarValueName;
		StringContainer solution;
		ColorContainer color;
		DoubleContainer oldCameraScale;
		DoubleContainer scaleFactor;

		EnumContainerT<MeasuredData::ArrowColorMode> colorMode;
		EnumContainerT<MeasuredData::ArrowLengthMode> lengthMode;
		DoubleContainer standardValue;
		DoubleContainer legendLength;
		DoubleContainer minimumValue;
	};

	MeasuredDataVectorGroupDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataVectorGroupDataItem();

	void updateActorSettings();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();

	std::string target() const override;
	void setTarget(const std::string &target) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showSettingDialog() {showPropertyDialog();}

private:
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	vtkPointSet* getPointSet();
	void innerUpdate2Ds() override;
	void updateColorSetting();
	void updatePolyData();
	void updateLegendData();
	void informGridUpdate();
	void doApplyOffset(double x, double y) override;

	void setupActors();
	void calculateStandardValue();
	void updateScaleFactor();

	vtkSmartPointer<vtkActor> m_arrowActor;
	vtkSmartPointer<vtkPolyDataMapper> m_arrowMapper;
	vtkSmartPointer<vtkAppendPolyData> m_appendPolyData;
	vtkSmartPointer<vtkPolyData> m_polyData;
	vtkSmartPointer<vtkUnstructuredGrid> m_activePoints;

	vtkSmartPointer<vtkHedgeHog> m_hedgeHog;
	vtkSmartPointer<vtkGlyph3D> m_arrowGlyph;
	vtkSmartPointer<vtkWarpVector> m_warpVector;
	vtkSmartPointer<vtkConeSource> m_arrowSource;

	vtkSmartPointer<vtkTextActor> m_legendTextActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_baseArrowPolyData;
	vtkSmartPointer<vtkActor2D> m_baseArrowActor;

	Setting m_setting;

	class SetSettingCommand;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_H
