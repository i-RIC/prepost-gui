#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guibase/arrowsettingcontainer.h>
#include <guibase/vtktool/vtkarrowlegendactors.h>
#include <guicore/misc/targeted/targeteditemi.h>

class NamedGraphicWindowDataItem;

class vtkActor;
class vtkActor2D;
class vtkAppendPolyData;
class vtkGlyph3D;
class vtkHedgeHog;
class vtkMaskPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTextActor;
class vtkTransformFilter;
class vtkUnstructuredGrid;
class vtkWarpVector;

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
	vtkActor* m_arrowActor;
	vtkPolyDataMapper* m_arrowMapper;
	vtkAppendPolyData* m_appendPolyData;
	vtkPolyData* m_polyData;

	vtkMaskPoints* m_arrowMask;
	vtkHedgeHog* m_hedgeHog;
	vtkGlyph3D* m_arrowGlyph;
	vtkWarpVector* m_warpVector;
	vtkConeSource* m_arrowSource;
	vtkTransformFilter* m_transformFilter;

	vtkArrowLegendActors m_legendActors;

	ArrowSettingContainer m_setting;

	double m_scaleFactor;

	class SetSettingCommand;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
