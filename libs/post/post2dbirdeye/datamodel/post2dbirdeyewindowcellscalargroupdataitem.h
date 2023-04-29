#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

class ColorMapSettingContainer;
class Post2dBirdEyeWindowCellScalarDataItem;
class Post2dBirdEyeWindowCellScalarGroupTopDataItem;

class vtkActor;
class vtkActor2D;

class Post2dBirdEyeWindowCellScalarGroupDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowCellScalarGroupDataItem(const std::string& elevationTarget, Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowCellScalarGroupDataItem();

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

	Post2dBirdEyeWindowCellScalarGroupTopDataItem* topDataItem() const;
	ColorMapSettingContainer* colorMapSetting(const std::string& name) const;
	ColorMapSettingContainer* activeColorMapSetting() const;

	class Impl;
	Impl* impl;

	class Setting;
	class SettingEditWidget;
	class UpdateSettingCommand;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H
