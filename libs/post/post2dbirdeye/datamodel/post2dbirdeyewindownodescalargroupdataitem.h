#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>

#include <unordered_map>

class Post2dBirdEyeWindowNodeScalarDataItem;
class Post2dBirdEyeWindowNodeScalarGroupTopDataItem;

class vtkActor;
class vtkActor2D;

class Post2dBirdEyeWindowNodeScalarGroupDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	class Setting : public CompositeContainer {
	public:
		enum class ColorMode {
			Custom,
			ByScalar
		};

		Setting();
		Setting(const Setting& setting);

		Setting& operator=(const Setting& setting);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

		Region2dSettingContainer regionSetting;

		EnumContainerT<ColorMode> colorMode;
		ColorContainer customColor;
		StringContainer colorTarget;

		OpacityContainer opacity;
	};

	Post2dBirdEyeWindowNodeScalarGroupDataItem(const std::string& elevationTarget, Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarGroupDataItem();

	const std::string& elevationTarget() const;

	void update();

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doHandleResize(VTKGraphicsView* v) override;

private:
	void innerUpdateZScale(double scale) override;

private:
	void setupActors();
	void updateActorSettings();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* topDataItem() const;
	ColorMapSettingContainer* colorMapSetting(const std::string& name) const;
	ColorMapSettingContainer* activeColorMapSetting() const;

	std::string m_elevationTarget;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;

	class PropertyDialog;
	class UpdateActorSettingsCommand;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
