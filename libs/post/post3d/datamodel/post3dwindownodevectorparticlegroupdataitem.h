#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <guibase/vtksubdividegrid.h>
#include <guibase/structuredgridregion.h>
#include <guibase/vtkCustomStreamPoints.h>
#include <guicore/misc/targeted/targeteditemi.h>

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

class NamedGraphicWindowDataItem;
class Post3dWindowParticleStructuredSetProperty;

class Post3dWindowNodeVectorParticleGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

private:
	const static int DEFAULT_SIZE = 3;

public:
	enum TimeMode {tmNormal, tmSubdivide, tmSkip};

	Post3dWindowNodeVectorParticleGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorParticleGroupDataItem();

	void updateActorSettings();
	void setupClipper();
	void updateZDepthRangeItemCount() override;
	virtual void assignActorZValues(const ZDepthRange& range) override;
	void update();
	bool exportParticles(const QString& filePrefix, int fileIndex, double time);

	std::string target() const override;
	void setTarget(const std::string& target) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void informGridUpdate();
	vtkPointSet* getRegion();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double zscale) override;
	void applyZScale();

	virtual vtkPointSet* newParticles(int i) = 0;
	virtual void setupActors() = 0;
	virtual void setupParticleSources() = 0;
	void setupStreamTracer();
	void setDefaultValues();
	void resetParticles();
	void addParticles();

	void clearParticleActors();
	void clearParticleGrids();

	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	QList<vtkPolyData*> m_particleGrids;
	QList<vtkActor*> m_particleActors;
	QList<vtkDataSetMapper*> m_particleMappers;

	vtkSmartPointer<vtkStreamPoints> m_streamTracer;
	vtkSmartPointer<vtkCustomStreamPoints> m_streamPoints;

	std::string m_target;
	TimeMode m_timeMode;

	int m_timeSamplingRate;
	int m_timeDivision;
	int m_previousStep;
	double m_previousTime;
	int m_nextStepToAddParticles;
	StructuredGridRegion::RegionMode m_regionMode;

	double m_zScale;

public:
	friend class Post3dWindowParticleStructuredSetProperty;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
