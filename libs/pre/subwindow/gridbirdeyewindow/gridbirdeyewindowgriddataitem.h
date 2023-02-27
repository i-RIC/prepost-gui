#ifndef GRIDBIRDEYEWINDOWGRIDDATAITEM_H
#define GRIDBIRDEYEWINDOWGRIDDATAITEM_H

#include "gridbirdeyewindowsetting.h"

#include <guicore/datamodel/graphicswindowrootdataitem.h>

class ColorMapLegendSettingContainerI;
class GridBirdEyeWindowDataModel;
class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;

class vtkActor;
class vtkActor2D;
class vtkCubeAxesActor2D;
class vtkOrientationMarkerWidget;

class GridBirdEyeWindowGridDataItem : public GraphicsWindowDataItem
{
public:
	GridBirdEyeWindowGridDataItem(GridBirdEyeWindowDataModel* model);
	~GridBirdEyeWindowGridDataItem();

	void editZScale();
	void editDisplaySetting();

	void updateGrid();

	void handleResize(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private:
	void setupActors();
	void setDefaultSetting();
	void setupLegendSettings();

	void updateActorSettings();
	ColorMapLegendSettingContainerI* activeLegendSetting() const;

	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	PreProcessorGridTypeDataItem* gridTypeDataItem() const;
	PreProcessorGridDataItem* gridDataItem() const;
	GraphicsWindowDataModel* dataModel() const override;

	void pushUpdateActorSettingsCommand(QUndoCommand* command);

	vtkActor* m_actor;
	vtkCubeAxesActor2D* m_cubeAxesActor;
	vtkOrientationMarkerWidget* m_orientationMarkerWidget;

	GridBirdEyeWindowSetting m_setting;
	std::map<std::string, ColorMapLegendSettingContainerI*> m_colorMapLegendSettings;
	std::map<std::string, vtkActor2D*> m_colorMapLegendActors;

	double m_previousZScale;

	GridBirdEyeWindowDataModel* m_model;

	class EditDisplaySettingDialog;
	class UpdateActorSettingCommand;
	class UpdateDisplaySettingCommand;
};

#endif // GRIDBIRDEYEWINDOWGRIDDATAITEM_H
