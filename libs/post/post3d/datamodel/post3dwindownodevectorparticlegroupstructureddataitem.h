#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H

#include "post3dwindownodevectorparticlegroupdataitem.h"
#include <QList>

class Post3dWindowParticleStructuredSetProperty;

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
	Post3dWindowNodeVectorParticleGroupStructuredDataItem(Post3dWindowDataItem* parent)
		: Post3dWindowNodeVectorParticleGroupDataItem(parent) {
		setDefaultValues();
	}
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	vtkPointSet* newParticles(int i);
	void setupParticleSources();
	void setupActors();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
private:
	void setDefaultValues();

	QList<vtkExtractGrid*> m_extractGrids;
	QList<vtkSubdivideGrid*> m_subdivideGrids;

	QList<Post3dWindowStructuredParticleSetSetting> m_settings;
public:
	friend class Post3dWindowParticleStructuredSetProperty;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
