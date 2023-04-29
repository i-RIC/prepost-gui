#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particlesettingcontainer.h>

#include <vector>

class NamedGraphicWindowDataItem;
class Post2dWindowNodeVectorParticleDataItem;
class Post2dWindowZoneDataItem;

class vtkActor;

class Post2dWindowNodeVectorParticleGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorParticleGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorParticleGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;
	bool isOutput() const;

	void updateActorSetting() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void updateZDepthRangeItemCount() override;
	virtual void assignActorZValues(const ZDepthRange& range) override;
	void update();
	bool exportParticles(const QString& filePrefix, int fileIndex, double time);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void informGridUpdate();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowZoneDataItem* zoneDataItem() const;
	static vtkPolyData* setupPolyDataFromPoints(vtkPoints* points);

	ParticleSettingContainer m_setting;

private:
	virtual int numberOfActors() = 0;
	virtual vtkActor* setupActor(int i) = 0;
	virtual vtkPolyData* newParticles(int i) = 0;
	virtual vtkDataSet* getRegion() = 0;

	void resetParticles();
	void addParticles();

	void setupActors();
	void clearParticleActors();
	void clearParticles();

	void innerUpdateZScale(double scale) override;

	std::vector<vtkPolyData*> m_particles;
	std::vector<vtkActor*> m_particleActors;

	int m_previousStep;
	double m_previousTime;
	int m_nextStepToAddParticles;
	double m_zScale;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
