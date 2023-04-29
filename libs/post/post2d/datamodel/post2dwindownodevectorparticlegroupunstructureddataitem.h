#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorparticlegroupdataitem.h"
#include <misc/compositecontainer.h>
#include <misc/qpointfcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/intcontainer.h>
#include <misc/colorcontainer.h>

#include <QList>

class Post2dWindowParticleUnstructuredSettingDialog;

class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem : public Post2dWindowNodeVectorParticleGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorParticleGroupUnstructuredDataItem();

	void showPropertyDialog() override;
	void assignActorZValues(const ZDepthRange& range) override;

	class SettingEditWidget;

signals:
	void point1Changed(const QPointF point);
	void point2Changed(const QPointF point);

private:
	int numberOfActors() override;
	vtkActor* setupActor(int i) override;
	vtkPolyData* newParticles(int i) override;
	vtkDataSet* getRegion() override;

	void setupPreviewActor();

	QDialog* propertyDialog(QWidget* parent) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void hidePreviewSetting();

	void setupDefaultValues();
	void updatePreview(const QPointF p1, const QPointF p2, int numPoints);
	std::vector<QPointF> generatePoints(const QPointF p1, const QPointF p2, int numPoints);
	void disablePreviewActor();

	class Impl;
	Impl* impl;

	class Setting;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_H
