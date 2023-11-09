#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorparticlegroupdataitem.h"

#include <memory>

class Post2dWindowNodeVectorParticleGroupStructuredDataItem : public Post2dWindowNodeVectorParticleGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorParticleGroupStructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorParticleGroupStructuredDataItem();

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
	std::unique_ptr<Impl> impl;

	class Setting;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_H
