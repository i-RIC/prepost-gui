#ifndef PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/opacitycontainer.h>
#include <QMap>

class NamedGraphicWindowDataItem;
class OpacityContainerWidget;
class PreProcessorGridAttributeCellDataItem;
class PreProcessorGridDataItem;
class QWidgetContainer;

class vtkActor;
class QAction;

class PreProcessorGridAttributeCellGroupDataItem : public PreProcessorDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	PreProcessorGridAttributeCellGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridAttributeCellGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSetting() override;
	void informDataChange(const std::string& name);
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void informGridUpdate();
	const QList<PreProcessorGridAttributeCellDataItem*> conditions() const;
	PreProcessorGridAttributeCellDataItem* cellDataItem(const std::string& name) const;
	void handleStandardItemChange() override;
	void pushOpacityPercentAndUpdateActorSettingCommand(const OpacityContainer& opacity, QUndoCommand* subcommand, bool apply = false);
	const OpacityContainer& opacity() const;
	OpacityContainerWidget* opacityWidget() const;
	QWidgetContainer* colorMapWidgetContainer() const;
	QAction* showAttributeBrowserAction() const;
	void addCustomMenuItems(QMenu* menu) override;
	void initAttributeBrowser();
	void clearAttributeBrowser();
	void fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	bool addToolBarButtons(QToolBar* toolbar) override;
	void applyColorMapSetting(const std::string& name);
	PreProcessorGridDataItem* gridDataItem() const;
	bool colorBarShouldBeVisible(const std::string& name) const;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showAttributeBrowser();

protected:
	PreProcessorGridAttributeCellDataItem* activeChildItem() const;

private:
	vtkIdType findCell(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	std::string m_target;
	vtkActor* m_actor;
	QMap<std::string, PreProcessorGridAttributeCellDataItem*> m_nameMap;

	OpacityContainer m_opacity;
	QAction* m_showAttributeBrowserAction;
	bool m_attributeBrowserFixed;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H
