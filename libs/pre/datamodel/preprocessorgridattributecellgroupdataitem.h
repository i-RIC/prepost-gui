#ifndef PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/opacitycontainer.h>
#include <QMap>

class QAction;
class NamedGraphicWindowDataItem;
class PreProcessorGridAttributeCellDataItem;

class vtkActor;

class PreProcessorGridAttributeCellGroupDataItem : public PreProcessorDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	PreProcessorGridAttributeCellGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridAttributeCellGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSettings();
	void informDataChange(const std::string& name);
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void informGridUpdate();
	const QList<PreProcessorGridAttributeCellDataItem*> conditions() const;
	PreProcessorGridAttributeCellDataItem* cellDataItem(const std::string& name) const;
	void handleStandardItemChange() override;
	void setOpacityPercentAndUpdateActorSettings(int o, QUndoCommand* subcommand, bool renderOnRedoOnly = false);
	int opacityPercent() const;
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
	PreProcessorGridAttributeCellDataItem* activeChildItem();

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

	class ModifyOpacityAndUpdateActorSettingsCommand;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H
