#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particlesettingcontainer.h>

#include <vector>

class NamedGraphicWindowDataItem;
class Post3dWindowNodeVectorParticleDataItem;
class Post3dWindowZoneDataItem;

class Post3dWindowNodeVectorParticleGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorParticleGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorParticleGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;
	bool isOutput() const;

	void updateActorSetting() override;
	void update();
	bool exportParticles(const QString& filePrefix, int fileIndex, double time);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void informGridUpdate();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post3dWindowZoneDataItem* zoneDataItem() const;
	vtkPolyData* setupPolyDataFromPoints(vtkPoints* points);

	ParticleSettingContainer m_setting;

private:
	virtual int numberOfActors() = 0;
	virtual vtkActor* setupActor(int i) = 0;
	virtual vtkPolyData* newParticles(int i) = 0;
	virtual vtkDataSet* getRegion() = 0;

	void applyZScale();
	void resetParticles();
	void addParticles();

	void setupActors();
	void clearParticleActors();
	void clearParticles();

	void innerUpdateZScale(double zscale) override;

	std::vector<vtkPolyData*> m_particles;
	std::vector<vtkActor*> m_particleActors;

private:
	int m_previousStep;
	double m_previousTime;
	int m_nextStepToAddParticles;
	double m_zScale;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
