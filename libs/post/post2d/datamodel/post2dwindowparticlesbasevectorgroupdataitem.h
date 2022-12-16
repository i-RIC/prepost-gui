#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/arrowsettingcontainer.h>
#include <guibase/vtktool/vtkarrowlegendactors.h>
#include <guicore/misc/targeted/targeteditemi.h>

class NamedGraphicWindowDataItem;

class vtkAppendPolyData;
class vtkActor;
class vtkActor2D;
class vtkConeSource;
class vtkGlyph3D;
class vtkHedgeHog;
class vtkMaskPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTextActor;
class vtkUnstructuredGrid;
class vtkWarpVector;

class Post2dWindowParticlesBaseVectorGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	const static int AUTO_AVERAGECOUNT = 20;

	Post2dWindowParticlesBaseVectorGroupDataItem(Post2dWindowDataItem* p);
	~Post2dWindowParticlesBaseVectorGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSettings();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void update();

	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void innerUpdate2Ds() override;
	void updateColorSetting();
	void updateMaskSetting();
	void updatePolyData();
	void updateLegendData();
	void informGridUpdate();

private:
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

	vtkArrowLegendActors m_legendActors;

	ArrowSettingContainer m_setting;

	double m_scaleFactor;

	class SetSettingCommand;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
