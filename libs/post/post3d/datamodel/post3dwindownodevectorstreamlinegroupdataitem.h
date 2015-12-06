#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <guicore/misc/targeted/targeteditemi.h>

#include <QMap>
#include <QColor>
#include <QList>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkStreamTracer.h>
#include <vtkClipPolyData.h>
#include <vtkPolyData.h>

class NamedGraphicWindowDataItem;
class Post3dWindowNodeVectorStreamlineDataItem;
class Post3dWindowStreamlineStructuredSetProperty;

class Post3dWindowNodeVectorStreamlineGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	/// Constructor
	Post3dWindowNodeVectorStreamlineGroupDataItem(Post3dWindowDataItem* parent);
	virtual ~Post3dWindowNodeVectorStreamlineGroupDataItem();
	void updateActorSettings();
	void setupClipper();
	void updateZDepthRangeItemCount() override;
	virtual void assignActorZValues(const ZDepthRange& range) override;
	void update();

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	std::string target() const override;
	void setTarget(const std::string& target) override;

	virtual void informGridUpdate();
	virtual void setupActors() = 0;
	vtkPointSet* getRegion();
	void innerUpdateZScale(double zscale) override;
	void applyZScale();
	void setupStreamTracer(vtkStreamTracer* tracer);
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

protected:
	std::string m_target;
	StructuredGridRegion::RegionMode m_regionMode;
	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;

	QList<vtkActor*> m_streamlineActors;
	QList<vtkDataSetMapper*> m_streamlineMappers;
	QList<vtkStreamTracer*> m_streamTracers;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;

private:
	double m_zScale;

public:
	friend class Post3dWindowStreamlineStructuredSetProperty;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
