#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
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

class Post2dWindowNodeVectorStreamlineDataItem;

class Post2dWindowNodeVectorStreamlineGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		/// Constructor
		Setting();
		/// Copy constructor
		Setting(const Setting& s);
		/// Copy operator
		Setting& operator=(const Setting& s);

		StringContainer currentSolution;
		EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	};

	/// Constructor
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
	void exclusivelyCheck(Post2dWindowNodeVectorStreamlineDataItem*);

protected:
	virtual void informGridUpdate();
	virtual void setupActors() = 0;
	vtkPointSet* getRegion();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void setCurrentSolution(const QString& currentSol);
	QString currentSolution() const {return m_setting.currentSolution;}

protected:
	void setupStreamTracer(vtkStreamTracer* tracer);

	Setting m_setting;

	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;

	std::vector<vtkActor*> m_streamlineActors;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;

private:
	class SelectSolutionCommand;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
