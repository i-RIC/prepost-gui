#ifndef POST3DWINDOWARROWGROUPDATAITEM_H
#define POST3DWINDOWARROWGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include "post3dwindowfacedataitem.h"

#include <misc/arrowsettingcontainer.h>
#include <misc/arrowshapecontainer.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkGlyph3D.h>
#include <vtkMaskPoints.h>
#include <vtkArrowSource.h>
#include <vtkHedgeHog.h>
#include <vtkWarpVector.h>
#include <vtkAppendFilter.h>
#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkAppendPolyData.h>
#include <vtkTransformFilter.h>
#include <vtkMaskPoints.h>
#include <vtkActor2D.h>
#include <vtkTextActor.h>

#include <QColor>

class Post3dWindowFaceDataItemChangeCommand;
class Post3dWindowArrowGroupDataItemUpdateOnRedo;
class Post3dWindowArrowGroupDataItemUpdateOnUndo;

class Post3dWindowArrowGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	const static int AUTO_AVERAGECOUNT = 20;

	Post3dWindowArrowGroupDataItem(Post3dWindowDataItem* parent);
	void setSetting(const ArrowSettingContainer& setting, const ArrowShapeContainer& shape);

	void updateActorSettings();
	void showSettingDialog();
	QMap<QString, Post3dWindowFaceDataItem::Setting> faceMap();
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map);
	void update();

protected:
	QDialog* propertyDialog(QWidget* parent) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override;
	void innerUpdateZScale(double scale) override;
	void innerUpdate2Ds() override;
	void updateVisibility() override {GraphicsWindowDataItem::updateVisibility();}

private:
	void setupActors();
	void setupAppendFilter();
	void updatePolyData();
	void updateLegendData();
	void updateColorSetting();
	void calculateStandardValue();
	void updateScaleFactor();

	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkSmartPointer<vtkActor> m_arrowActor;
	vtkSmartPointer<vtkPolyDataMapper> m_arrowMapper;
	vtkSmartPointer<vtkAppendPolyData> m_appendPolyData;
	vtkSmartPointer<vtkMaskPoints> m_maskPoints;
	vtkSmartPointer<vtkPolyData> m_polyData;
	vtkSmartPointer<vtkAppendFilter> m_appendFilter;
	vtkSmartPointer<vtkTransformFilter> m_transformFilter;

	vtkSmartPointer<vtkHedgeHog> m_hedgeHog;
	vtkSmartPointer<vtkGlyph3D> m_arrowGlyph;
	vtkSmartPointer<vtkWarpVector> m_warpVector;
	vtkSmartPointer<vtkConeSource> m_arrowSource;

	vtkSmartPointer<vtkTextActor> m_legendTextActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_baseArrowPolyData;
	vtkSmartPointer<vtkPolyData> m_activePoints;
	vtkSmartPointer<vtkActor2D> m_baseArrowActor;

	double m_oldCameraScale;

	double m_scaleFactor;
	ArrowSettingContainer m_arrowSetting;
	ArrowShapeContainer m_arrowShape;

public:
	friend class Post3dWindowArrowGroupSetSetting;
	friend class Post3dWindowArrowGroupSetFaceMap;
	friend class Post3dWindowFaceDataItemChangeCommand;
	friend class Post3dWindowArrowGroupDataItemUpdateOnRedo;
	friend class Post3dWindowArrowGroupDataItemUpdateOnUndo;
};

#endif // POST3DWINDOWARROWGROUPDATAITEM_H
