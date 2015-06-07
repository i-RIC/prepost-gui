#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorparticlegroupdataitem.h"
#include <QVector2D>
#include <QList>

class Post2dWindowParticleUnstructuredSetProperty;
class Post2dWindowParticleUnstructuredSettingDialog;

struct Post2dWindowUnstructuredParticleSetSetting {
	QVector2D point1;
	QVector2D point2;
	bool pointsSet;
	int numberOfPoints;
	/// Particle color.
	QColor color;
	/// Particle size on screen. Specify by pixels on screen.
	int size;
};

class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem : public Post2dWindowNodeVectorParticleGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem(Post2dWindowDataItem* parent)
		: Post2dWindowNodeVectorParticleGroupDataItem(parent) {
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
	void setSetting(const QVector2D& v1, const QVector2D& v2, int num, int pointSize);
	void clearSetting();
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

	QList<Post2dWindowUnstructuredParticleSetSetting> m_settings;

	Post2dWindowParticleUnstructuredSettingDialog* m_dialog;

	QVector2D m_point1;
	QVector2D m_point2;
	int m_numberOfPoints;

public:
	friend class Post2dWindowParticleUnstructuredSetProperty;
	friend class Post2dWindowParticleUnstructuredSettingDialog;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_H
