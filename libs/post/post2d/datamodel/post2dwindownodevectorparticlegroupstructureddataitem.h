#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorparticlegroupdataitem.h"
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/colorcontainer.h>
#include <misc/intcontainer.h>
#include <QList>

class Post2dWindowNodeVectorParticleGroupStructuredDataItem : public Post2dWindowNodeVectorParticleGroupDataItem
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		enum SpaceMode {smNormal, smSubdivide, smSkip};

		/// Constructor
		Setting();

		StructuredGridRegion::Range2d range;
		EnumContainerT<SpaceMode> spaceMode;
		IntContainer spaceSamplingRate;
		IntContainer spaceDivision;
		ColorContainer color;
		IntContainer size;
	};

	Post2dWindowNodeVectorParticleGroupStructuredDataItem(Post2dWindowDataItem* parent) :
		Post2dWindowNodeVectorParticleGroupDataItem {parent}
	{
		setDefaultValues();
	}

protected:
	vtkPointSet* newParticles(int i) override;
	void setupParticleSources() override;
	void setupActors() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void setDefaultValues();

	QList<vtkExtractGrid*> m_extractGrids;
	QList<vtkSubdivideGrid*> m_subdivideGrids;

	QList<Setting> m_stSettings;

	class SetSettingCommand;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
