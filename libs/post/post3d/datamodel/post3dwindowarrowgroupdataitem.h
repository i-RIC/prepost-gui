#ifndef POST3DWINDOWARROWGROUPDATAITEM_H
#define POST3DWINDOWARROWGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include "post3dwindowfacedataitem.h"

#include <guibase/arrowsettingcontainer.h>
#include <guibase/vtktool/vtkarrowlegendactors.h>
#include <guibase/vtktool/vtkarrowsactor.h>

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

class Post3dWindowArrowGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowArrowGroupDataItem(Post3dWindowDataItem* parent);
	void setSetting(const ArrowSettingContainer& setting);

	void updateActorSettings();
	QMap<QString, Post3dWindowFaceDataItem::Setting> faceMap();
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map);
	void update();

protected:
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

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkArrowsActor m_arrowsActor;
        vtkArrowLegendActors m_legendActors;

	vtkSmartPointer<vtkAppendPolyData> m_appendPolyData;
	vtkSmartPointer<vtkMaskPoints> m_maskPoints;
	vtkSmartPointer<vtkAppendFilter> m_appendFilter;
	vtkSmartPointer<vtkTransformFilter> m_transformFilter;

	ArrowSettingContainer m_setting;

	class SetSettingCommand;
	class UpdateOnUndoCommand;
	class UpdateOnRedoCommand;

public:
	friend class Post3dWindowArrowGroupSetFaceMap;
	friend class Post3dWindowFaceDataItemChangeCommand;
};

#endif // POST3DWINDOWARROWGROUPDATAITEM_H
