#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
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
#include <vtkPolyData.h>
#include <vtkClipPolyData.h>

class Post3dWindowNodeVectorParticleDataItem;
class Post3dWindowGridParticleSelectSolution;
class Post3dWindowParticleStructuredSetProperty;

class Post3dWindowNodeVectorParticleGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
private:
	const static int DEFAULT_SIZE = 3;
public:
	enum TimeMode{tmNormal, tmSubdivide, tmSkip};
	/// Constructor
	Post3dWindowNodeVectorParticleGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorParticleGroupDataItem();
	void updateActorSettings();
	void setupClipper();
	void updateZDepthRangeItemCount();
	virtual void assignActionZValues(const ZDepthRange& range);
	void update();
	bool exportParticles(const QString& filePrefix, int fileIndex, double time);
	const QString& currentSolution(){return m_currentSolution;}
public slots:
	void exclusivelyCheck(Post3dWindowNodeVectorParticleDataItem*);
protected:
	void informGridUpdate();
	vtkPointSet* getRegion();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void setCurrentSolution(const QString& currentSol);
	void innerUpdateZScale(double zscale);
	void applyZScale();

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
	StructuredGridRegion::RegionMode m_regionMode;

	double m_zScale;
public:
	friend class Post3dWindowParticleStructuredSetProperty;
	friend class Post3dWindowGridParticleSelectSolution;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
