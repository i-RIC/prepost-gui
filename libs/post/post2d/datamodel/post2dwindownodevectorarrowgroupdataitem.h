#ifndef POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/arrowshapecontainer.h>
#include <misc/arrowsettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/stringcontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkGlyph3D.h>
#include <vtkHedgeHog.h>
#include <vtkMaskPoints.h>
#include <vtkArrowSource.h>
#include <vtkConeSource.h>
#include <vtkWarpVector.h>
#include <vtkExtractGrid.h>
#include <vtkAppendPolyData.h>
#include <vtkActor2D.h>
#include <vtkTextActor.h>
#include <vtkClipPolyData.h>

#include <QColor>

class vtkPointSet;
class NamedGraphicWindowDataItem;
class Post2dWindowNodeVectorArrowDataItem;

class Post2dWindowNodeVectorArrowGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	const static int AUTO_AVERAGECOUNT = 20;

	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);
		Setting& operator=(const Setting& s);

		ArrowSettingContainer arrowSetting;
		ArrowShapeContainer arrowShape;

		DoubleContainer oldCameraScale;
		DoubleContainer scaleFactor;
		EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	};

	Post2dWindowNodeVectorArrowGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorArrowGroupDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void updateActorSettings();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void addCustomMenuItems(QMenu* menu) override;
	void update();

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	std::string target() const override;
	void setTarget(const std::string& target) override;

	void innerUpdate2Ds() override;
	void updateColorSetting();
	void updatePolyData();
	void updateLegendData();
	void informGridUpdate();
	virtual void updateActivePoints() = 0;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void setupActors();
	void calculateStandardValue();
	void updateScaleFactor();

protected:
	vtkSmartPointer<vtkActor> m_arrowActor;
	vtkSmartPointer<vtkPolyDataMapper> m_arrowMapper;
	vtkSmartPointer<vtkAppendPolyData> m_appendPolyData;
	vtkSmartPointer<vtkPolyData> m_polyData;

	vtkSmartPointer<vtkHedgeHog> m_hedgeHog;
	vtkSmartPointer<vtkGlyph3D> m_arrowGlyph;
	vtkSmartPointer<vtkWarpVector> m_warpVector;
	vtkSmartPointer<vtkConeSource> m_arrowSource;

	vtkSmartPointer<vtkTextActor> m_legendTextActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_baseArrowPolyData;
	vtkSmartPointer<vtkActor2D> m_baseArrowActor;
	vtkSmartPointer<vtkPolyData> m_activePoints;

	Setting m_setting;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
