#ifndef PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/opacitycontainer.h>
#include <QMap>

#include <string>

class NamedGraphicWindowDataItem;
class OpacityContainerWidget;
class PreProcessorGridAttributeNodeDataItem;
class QWidgetContainer;

class vtkActor;
class QAction;

class PreProcessorGridAttributeNodeGroupDataItem : public PreProcessorDataItem , public TargetedItemI
{
	Q_OBJECT

public:
	PreProcessorGridAttributeNodeGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridAttributeNodeGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSetting() override;
	void informDataChange(const std::string& name);
	void updateZDepthRangeItemCount() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informGridUpdate();
	const QList<PreProcessorGridAttributeNodeDataItem*> conditions() const;
	PreProcessorGridAttributeNodeDataItem* nodeDataItem(const std::string& name) const;
	void handleStandardItemChange() override;
	void setOpacityPercentAndUpdateActorSettings(int o, QUndoCommand* subcommand, bool renderOnRedoOnly = false);
	int opacityPercent() const;
	void informSelectedVerticesChanged(const QVector<vtkIdType>& vertices);
	QAction* showAttributeBrowserAction() const;
	void addCustomMenuItems(QMenu* menu) override;
	void initAttributeBrowser();
	void clearAttributeBrowser();
	void fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	bool addToolBarButtons(QToolBar* toolbar) override;
	void applyColorMapSetting(const std::string& name);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showAttributeBrowser();

protected:
	PreProcessorGridAttributeNodeDataItem* activeChildItem();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);

private:
	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);

	std::string m_target;

	vtkActor* m_actor;

	QAction* m_showAttributeBrowserAction;
	OpacityContainer m_opacity;
	bool m_attributeBrowserFixed;
	QMap<std::string, PreProcessorGridAttributeNodeDataItem*> m_nameMap;

	class ModifyOpacityAndUpdateActorSettingsCommand;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
