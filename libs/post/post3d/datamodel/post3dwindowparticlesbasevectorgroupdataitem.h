#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guibase/arrowsettingcontainer.h>
#include <guicore/misc/targeted/targeteditemi.h>

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

class NamedGraphicWindowDataItem;

class Post3dWindowParticlesBaseVectorGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	const static int AUTO_AVERAGECOUNT = 20;

	Post3dWindowParticlesBaseVectorGroupDataItem(Post3dWindowDataItem* p);
	~Post3dWindowParticlesBaseVectorGroupDataItem();

	void updateActorSettings();
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void innerUpdateZScale(double zscale) override;
	void innerUpdate2Ds() override;
	void updateColorSetting();
	void updateMaskSetting();
	void updatePolyData();
	void updateLegendData();
	void informGridUpdate();

private:
	std::string target() const override;
	void setTarget(const std::string& target);

	void setupActors();
	void calculateStandardValue();
	void updateScaleFactor();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

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


	ArrowSettingContainer m_setting;

	double m_scaleFactor;

	class SetSettingCommand;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
