#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

#include <memory>

class ColorMapSettingContainerI;
class Post2dBirdEyeWindowNodeScalarDataItem;
class Post2dBirdEyeWindowNodeScalarGroupTopDataItem;

class vtkActor;
class vtkActor2D;

class Post2dBirdEyeWindowNodeScalarGroupDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowNodeScalarGroupDataItem(const std::string& elevationTarget, Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarGroupDataItem();

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

	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* topDataItem() const;
	ColorMapSettingContainerI* colorMapSetting(const std::string& name) const;
	ColorMapSettingContainerI* activeColorMapSetting() const;

	class Impl;
	std::unique_ptr<Impl> impl;

	class Setting;
	class SettingEditWidget;
	class UpdateSettingCommand;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
