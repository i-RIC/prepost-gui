#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorparticlegroupdataitem.h"
#include <QList>

class Post2dWindowParticleStructuredSetProperty;

struct Post2dWindowStructuredParticleSetSetting {
	enum SpaceMode {smNormal, smSubdivide, smSkip};
	/// Range to generate grid
	StructuredGridRegion::Range2d range;
	SpaceMode spaceMode;
	int spaceSamplingRate;
	int spaceDivision;
	/// Particle color.
	QColor color;
	/// Particle size on screen. Specify by pixels on screen.
	int size;
};

class Post2dWindowNodeVectorParticleGroupStructuredDataItem : public Post2dWindowNodeVectorParticleGroupDataItem
{
	Q_OBJECT
public:
	Post2dWindowNodeVectorParticleGroupStructuredDataItem(Post2dWindowDataItem* parent)
		: Post2dWindowNodeVectorParticleGroupDataItem(parent) {
		setDefaultValues();
	}
protected:
	vtkPointSet* newParticles(int i);
	void setupParticleSources();
	void setupActors();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	void setDefaultValues();

	QList<vtkExtractGrid*> m_extractGrids;
	QList<vtkSubdivideGrid*> m_subdivideGrids;

	QList<Post2dWindowStructuredParticleSetSetting> m_settings;
public:
	friend class Post2dWindowParticleStructuredSetProperty;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
