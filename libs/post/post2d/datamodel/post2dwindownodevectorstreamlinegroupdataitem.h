#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>
#include <misc/enumcontainert.h>

#include <QMap>
#include <QColor>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkStreamTracer.h>
#include <vtkClipPolyData.h>
#include <vtkPolyData.h>

#include <vector>

class NamedGraphicWindowDataItem;
class Post2dWindowNodeVectorStreamlineDataItem;

class Post2dWindowNodeVectorStreamlineGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		StringContainer target;
		EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	};

	Post2dWindowNodeVectorStreamlineGroupDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowNodeVectorStreamlineGroupDataItem();

	void updateActorSettings();
	void setupClipper();
	void updateZDepthRangeItemCount() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	virtual void assignActorZValues(const ZDepthRange& range) override;
	void update();

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	std::string target() const override;
	void setTarget(const std::string& target);

	virtual void informGridUpdate();
	virtual void setupActors() = 0;
	vtkPointSet* getRegion();

	void setupStreamTracer(vtkStreamTracer* tracer);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

	Setting m_setting;
	double m_zScale;

	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;

	std::vector<vtkActor*> m_streamlineActors;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
