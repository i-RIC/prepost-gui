#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <QMap>
#include <QColor>
#include <QList>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkStreamTracer.h>
#include <vtkClipPolyData.h>
#include <vtkPolyData.h>

class Post2dWindowNodeVectorStreamlineDataItem;
class Post2dWindowStructuredGridStreamlineSelectSolution;
class Post2dWindowStreamlineStructuredSetProperty;

class Post2dWindowNodeVectorStreamlineGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowNodeVectorStreamlineGroupDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowNodeVectorStreamlineGroupDataItem();
	void updateActorSettings();
	void setupClipper();
	void updateZDepthRangeItemCount();
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);
	virtual void assignActionZValues(const ZDepthRange& range);
	void update();

public slots:
	void exclusivelyCheck(Post2dWindowNodeVectorStreamlineDataItem*);

protected:
	virtual void informGridUpdate();
	virtual void setupActors() = 0;
	vtkPointSet* getRegion();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void setCurrentSolution(const QString& currentSol);
	const QString& currentSolution(){return m_currentSolution;}

protected:
	void setupStreamTracer(vtkStreamTracer* tracer);

	QString m_currentSolution;
	StructuredGridRegion::RegionMode m_regionMode;
	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;

	QList<vtkActor*> m_streamlineActors;
	QList<vtkDataSetMapper*> m_streamlineMappers;
	QList<vtkStreamTracer*> m_streamTracers;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;

private:
	void setDefaultValues();

public:
	friend class Post2dWindowStructuredGridStreamlineSelectSolution;
	friend class Post2dWindowStreamlineStructuredSetProperty;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
