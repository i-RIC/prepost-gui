#ifndef GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
#define GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H

#include "../gridbirdeyewindowdataitem.h"

class ColorMapSettingContainerI;
class GridBirdEyeWindowNodeScalarDataItem;
class GridBirdEyeWindowNodeScalarGroupTopDataItem;

class vtkActor;
class vtkActor2D;

class GridBirdEyeWindowNodeScalarGroupDataItem : public GridBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowNodeScalarGroupDataItem(const std::string& elevationTarget, GridBirdEyeWindowDataItem* parent);
	~GridBirdEyeWindowNodeScalarGroupDataItem();

	const std::string& elevationTarget() const;

	void update();

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
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

	GridBirdEyeWindowNodeScalarGroupTopDataItem* topDataItem() const;
	ColorMapSettingContainerI* colorMapSetting(const std::string& name) const;
	ColorMapSettingContainerI* activeColorMapSetting() const;

	class Impl;
	Impl* impl;

	class Setting;
	class SettingEditWidget;
	class UpdateSettingCommand;
};

#endif // GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
