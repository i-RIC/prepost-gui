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
	struct Setting : public CompositeContainer
	{
		/// Constructor
		Setting();
		/// Copy constructor
		Setting(const Setting& s);
		/// Copy operator
		Setting& operator=(const Setting& s);

		QPointFContainer point1;
		QPointFContainer point2;
		BoolContainer pointsSet;
		IntContainer numberOfPoints;
		ColorContainer color;
		IntContainer size;
	};

	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem(Post2dWindowDataItem* parent) :
		Post2dWindowNodeVectorParticleGroupDataItem {parent}
	{
		setDefaultValues();
		setupTmpSource();
	}
	~Post2dWindowNodeVectorParticleGroupUnstructuredDataItem();
	void showPropertyDialog() override;
	void assignActorZValues(const ZDepthRange& range) override;

protected:
	void setupActors() override;
	vtkPointSet* newParticles(int i) override;
	void setupParticleSources() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent*, VTKGraphicsView*) override;
	void setSetting(const QPointF& v1, const QPointF& v2, int num, int pointSize);
	void hidePreviewSetting();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private slots:
	void exitDialogMode();

private:
	void setDefaultValues();
	void setupTmpSource();
	QList<vtkUnstructuredGrid*> m_sourcePoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_previewPoints;
	vtkSmartPointer<vtkDataSetMapper> m_previewMapper;
	vtkSmartPointer<vtkActor> m_previewActor;

	QList<Setting> m_unstSettings;

	Post2dWindowParticleUnstructuredSettingDialog* m_dialog;

	QPointF m_point1;
	QPointF m_point2;
	int m_numberOfPoints;

	class SetSettingCommand;

public:
	friend class Post2dWindowParticleUnstructuredSettingDialog;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_H
