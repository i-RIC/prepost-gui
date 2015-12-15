#ifndef POST3DWINDOWPARTICLESVECTORGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESVECTORGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <misc/arrowshapecontainer.h>
#include <misc/arrowsettingcontainer.h>

#include <postbase/postparticlevectorpropertydialog.h>

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkMaskPoints.h>
#include <vtkTextActor.h>
#include <vtkGlyph3D.h>
#include <vtkAppendPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkHedgeHog.h>
#include <vtkWarpVector.h>
#include <vtkConeSource.h>
#include <vtkTransformFilter.h>

class Post3dWindowParticlesVectorDataItem;
class Post3dWindowParticlesVectorSelectAttribute;
class Post3dWindowParticlesVectorSetProperty;

class Post3dWindowParticlesVectorGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	const static int AUTO_AVERAGECOUNT = 20;
	Post3dWindowParticlesVectorGroupDataItem(Post3dWindowDataItem* p);
	~Post3dWindowParticlesVectorGroupDataItem();
	void updateActorSettings();
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);

public slots:
	void exclusivelyCheck(Post3dWindowParticlesVectorDataItem*);

protected:
	void innerUpdateZScale(double zscale) override;
	void innerUpdate2Ds() override;
	void updateColorSetting();
	void updateMaskSetting();
	void updatePolyData();
	void updateLegendData();
	void informGridUpdate();
	void setAttribute(const QString& attribute);
	QString attribute() const {return m_arrowSetting.attribute();}
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void setupActors();
	void calculateStandardValue();
	void updateScaleFactor();

protected:
	vtkSmartPointer<vtkActor> m_arrowActor;
	vtkSmartPointer<vtkPolyDataMapper> m_arrowMapper;
	vtkSmartPointer<vtkAppendPolyData> m_appendPolyData;
	vtkSmartPointer<vtkPolyData> m_polyData;

	vtkSmartPointer<vtkMaskPoints> m_arrowMask;
	vtkSmartPointer<vtkHedgeHog> m_hedgeHog;
	vtkSmartPointer<vtkGlyph3D> m_arrowGlyph;
	vtkSmartPointer<vtkWarpVector> m_warpVector;
	vtkSmartPointer<vtkConeSource> m_arrowSource;
	vtkSmartPointer<vtkTransformFilter> m_transformFilter;

	vtkSmartPointer<vtkTextActor> m_legendTextActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_baseArrowPolyData;
	vtkSmartPointer<vtkActor2D> m_baseArrowActor;

	ArrowSettingContainer m_arrowSetting;
	ArrowShapeContainer m_arrowShape;

	double m_scaleFactor;
	double m_oldCameraScale;

public:
	friend class Post3dWindowParticlesVectorSelectAttribute;
	friend class Post3dWindowParticlesVectorSetProperty;
};

#endif // POST3DWINDOWPARTICLESVECTORGROUPDATAITEM_H
