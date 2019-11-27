#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <guibase/vtksubdividegrid.h>
#include <guibase/structuredgridregion.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intarraycontainer.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>

#include <QMap>
#include <QColor>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGrid.h>
#include <vtkPolyData.h>
#include <vtkClipPolyData.h>

#include <vector>

class NamedGraphicWindowDataItem;
class Post3dWindowParticleStructuredSetProperty;

class Post3dWindowNodeVectorParticleGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

private:
	const static int DEFAULT_SIZE = 3;

public:
	enum GenerateMode {gmPeriodical, gmArbitrary};
	enum TimeMode {tmNormal, tmSubdivide, tmSkip};

	Post3dWindowNodeVectorParticleGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorParticleGroupDataItem();

	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		StringContainer target;
		EnumContainerT<GenerateMode> generateMode;
		EnumContainerT<TimeMode> timeMode;

		IntContainer timeSamplingRate;
		IntContainer timeDivision;
		IntArrayContainer arbitraryTimes;
		EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	};

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
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void informGridUpdate();
	vtkPointSet* getRegion();
	void innerUpdateZScale(double zscale) override;
	void applyZScale();

	virtual vtkPointSet* newParticles(int i) = 0;
	virtual void setupActors() = 0;
	virtual void setupParticleSources() = 0;
	void setDefaultValues();
	void resetParticles();
	void addParticles();

	vtkPolyData* setupPolyDataFromPoints(vtkPoints* points);
	void clearParticleActors();
	void clearParticleGrids();

protected:
	std::vector<vtkActor*> m_particleActors;
	std::vector<vtkDataSetMapper*> m_particleMappers;

	Setting m_setting;

private:
	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	std::vector<vtkPolyData*> m_particleGrids;

	int m_previousStep;
	double m_previousTime;
	int m_nextStepToAddParticles;

	double m_zScale;

public:
	friend class Post3dWindowParticleStructuredSetProperty;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
