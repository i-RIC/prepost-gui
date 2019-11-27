#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H

#include "post3dwindownodevectorparticlegroupdataitem.h"
#include <QList>

struct Post3dWindowStructuredParticleSetSetting {
	enum SpaceMode {smNormal, smSubdivide, smSkip};
	/// Range to generate grid
	StructuredGridRegion::Range3d range;
	SpaceMode spaceMode;
	int spaceSamplingRate;
	int spaceDivision;
	/// Particle color.
	QColor color;
	/// Particle size on screen. Specify by pixels on screen.
	int size;
};

class Post3dWindowNodeVectorParticleGroupStructuredDataItem : public Post3dWindowNodeVectorParticleGroupDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorParticleGroupStructuredDataItem(Post3dWindowDataItem* parent);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	vtkPointSet* newParticles(int i) override;
	void setupParticleSources() override;
	void setupActors() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

private:
	void setDefaultValues();

	QList<vtkExtractGrid*> m_extractGrids;
	QList<vtkSubdivideGrid*> m_subdivideGrids;

	QList<Post3dWindowStructuredParticleSetSetting> m_settings;

	class SetSettingCommand;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
