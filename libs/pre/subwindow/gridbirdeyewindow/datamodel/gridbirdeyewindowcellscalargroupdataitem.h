#ifndef GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H
#define GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H

#include "../gridbirdeyewindowdataitem.h"

class ColorMapSettingContainerI;
class GridBirdEyeWindowCellScalarDataItem;
class GridBirdEyeWindowCellScalarGroupTopDataItem;

class vtkActor;
class vtkActor2D;

class GridBirdEyeWindowCellScalarGroupDataItem : public GridBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowCellScalarGroupDataItem(const std::string& elevationTarget, GridBirdEyeWindowDataItem* parent);
	~GridBirdEyeWindowCellScalarGroupDataItem();

	const std::string& elevationTarget() const;

	void update();

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	bool addToolBarButtons(QToolBar* toolBar) override;

private:
	void innerUpdateZScale(double scale) override;

private:
	void setupActors();
	void updateActorSetting() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	GridBirdEyeWindowCellScalarGroupTopDataItem* topDataItem() const;
	ColorMapSettingContainerI* colorMapSetting(const std::string& name) const;
	ColorMapSettingContainerI* activeColorMapSetting() const;

	class Impl;
	Impl* impl;

	class Setting;
	class SettingEditWidget;
	class UpdateSettingCommand;
};

#endif // GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H
