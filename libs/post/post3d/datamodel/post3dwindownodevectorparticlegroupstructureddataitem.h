#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H

#include "post3dwindownodevectorparticlegroupdataitem.h"

class Post3dWindowNodeVectorParticleGroupStructuredDataItem : public Post3dWindowNodeVectorParticleGroupDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorParticleGroupStructuredDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorParticleGroupStructuredDataItem();

	class SettingEditWidget;

public slots:
	void showPropertyDialog() override;

private:
	int numberOfActors() override;
	vtkActor* setupActor(int i) override;
	vtkPolyData* newParticles(int i) override;
	vtkDataSet* getRegion() override;

	QDialog* propertyDialog(QWidget* parent) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void setDefaultValues();

	class Impl;
	Impl* impl;

	class Setting;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
