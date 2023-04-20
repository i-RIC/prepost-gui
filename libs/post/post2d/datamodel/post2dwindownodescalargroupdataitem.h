#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>

#include <h5cgnszone.h>

class ColorMapSettingToolBarWidgetI;
class OpacityContainerWidget;
class Post2dWindowPointScalarGroupTopDataItemI;

class vtkActor;
class vtkActor2D;

class Post2dWindowNodeScalarGroupDataItem : public Post2dWindowDataItem
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

	Post2dWindowNodeScalarGroupDataItem(const std::string& target, iRICLib::H5CgnsZone::SolutionPosition position, Post2dWindowDataItem* parent);
	~Post2dWindowNodeScalarGroupDataItem();

	void update();

	const std::string& target() const;
	const ColorMapSettingContainer& colorMapSetting() const;

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

	bool checkKmlExportCondition();
	bool exportKMLHeader(QXmlStreamWriter& writer);
	bool exportKMLFooter(QXmlStreamWriter& writer);
	bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, bool oneShot);

	bool exportContourFigureToShape(const QString& filename, double time);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	void setupActors();
	void updateActorSetting() override;

	void innerUpdateZScale(double scale) override;

	Post2dWindowPointScalarGroupTopDataItemI* topDataItem() const;

	std::string m_target;
	iRICLib::H5CgnsZone::SolutionPosition m_solutionPosition;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	ColorMapSettingToolBarWidgetI* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;

	class PropertyDialog;
	class ShapeExporter;

	ShapeExporter* m_shapeExporter;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_H
