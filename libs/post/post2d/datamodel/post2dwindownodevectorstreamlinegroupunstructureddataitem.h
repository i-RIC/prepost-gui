#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <QVector2D>

class Post2dWindowStreamlineUnstructuredSetProperty;
class Post2dWindowStreamlineUnstructuredSettingDialog;

struct Post2dWindowUnstructuredStreamlineSetSetting {
	QVector2D point1;
	QVector2D point2;
	bool pointsSet;
	int numberOfPoints;
	/// Particle color.
	QColor color;
	/// Streamline width on screen. Specify by pixels on screen.
	int width;
};

class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem  : public Post2dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT
public:
	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem();
	void showPropertyDialog() override;
	void assignActorZValues(const ZDepthRange& range) override;
protected:
	vtkPointSet* getSource(int i);
	void setupActors() override;

	QDialog* propertyDialog(QWidget* parent) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent*, VTKGraphicsView*) override;
	void setSetting(const QVector2D& v1, const QVector2D& v2, int num);
	void clearSetting();
private slots:
	void exitDialogMode();
private:
	void setupDefaultValues();
	void setupTmpSource();
	QList<vtkUnstructuredGrid*> m_sourcePoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_previewPoints;
	vtkSmartPointer<vtkDataSetMapper> m_previewMapper;
	vtkSmartPointer<vtkActor> m_previewActor;

	QList<Post2dWindowUnstructuredStreamlineSetSetting> m_settings;

	Post2dWindowStreamlineUnstructuredSettingDialog* m_dialog;

	QVector2D m_point1;
	QVector2D m_point2;
	int m_numberOfPoints;

public:
	friend class Post2dWindowStreamlineUnstructuredSetProperty;
	friend class Post2dWindowStreamlineUnstructuredSettingDialog;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H
