#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/vtksubdividegrid.h>
#include <guibase/structuredgridregion.h>
#include <guibase/vtkCustomStreamPoints.h>
#include <QMap>
#include <QColor>
#include <QList>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkStreamPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGrid.h>
#include <vtkClipPolyData.h>

class Post2dWindowNodeVectorParticleDataItem;
class Post2dWindowGridParticleSelectSolution;
class Post2dWindowParticleStructuredSetProperty;

class Post2dWindowNodeVectorParticleGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
private:
	const static int DEFAULT_SIZE = 3;
public:
	enum TimeMode {tmNormal, tmSubdivide, tmSkip};
	/// Constructor
	Post2dWindowNodeVectorParticleGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorParticleGroupDataItem();
	void updateActorSettings();
	void setupClipper();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);
	void updateZDepthRangeItemCount();
	virtual void assignActionZValues(const ZDepthRange& range);
	void update();
	bool exportParticles(const QString& filePrefix, int fileIndex, double time);
	const QString& currentSolution() {return m_currentSolution;}

public slots:
	void exclusivelyCheck(Post2dWindowNodeVectorParticleDataItem*);

protected:
	void informGridUpdate();
	vtkPointSet* getRegion();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void setCurrentSolution(const QString& currentSol);

	virtual vtkPointSet* newParticles(int i) = 0;
	virtual void setupActors() = 0;
	virtual void setupParticleSources() = 0;
	void setupStreamTracer();
	void setDefaultValues();
	void resetParticles();
	void addParticles();

	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	QList<vtkPolyData*> m_particleGrids;
	QList<vtkActor*> m_particleActors;
	QList<vtkDataSetMapper*> m_particleMappers;

	vtkSmartPointer<vtkStreamPoints> m_streamTracer;
	vtkSmartPointer<vtkCustomStreamPoints> m_streamPoints;

	QString m_currentSolution;
	TimeMode m_timeMode;

	int m_timeSamplingRate;
	int m_timeDivision;
	unsigned int m_previousStep;
	double m_previousTime;
	int m_nextStepToAddParticles;
	int m_particleSize;
	StructuredGridRegion::RegionMode m_regionMode;
public:
	friend class Post2dWindowParticleStructuredSetProperty;
	friend class Post2dWindowGridParticleSelectSolution;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
