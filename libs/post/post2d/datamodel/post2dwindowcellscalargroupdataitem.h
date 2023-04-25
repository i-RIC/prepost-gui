#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>

class ColorMapSettingToolBarWidgetI;
class OpacityContainerWidget;
class Post2dWindowCellScalarGroupTopDataItem;

class vtkActor;
class vtkActor2D;

class Post2dWindowCellScalarGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	class Setting : public CompositeContainer {
	public:
		Setting();
		Setting(const Setting& setting);

		Setting& operator=(const Setting& setting);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

		ColorMapSettingContainer colorMapSetting;
		Region2dSettingContainer regionSetting;
		OpacityContainer opacity;
	};

	Post2dWindowCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent);
	~Post2dWindowCellScalarGroupDataItem();

	void update();

	const std::string& target() const;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	bool hasTransparentPart() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	void setupActors();
	void updateActorSetting() override;

	void innerUpdateZScale(double scale) override;

	Post2dWindowCellScalarGroupTopDataItem* topDataItem() const;

	std::string m_target;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	ColorMapSettingToolBarWidgetI* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;

	class PropertyDialog;

public:
	friend class Post2dWindowCellScalarGroupTopDataItem;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_H
