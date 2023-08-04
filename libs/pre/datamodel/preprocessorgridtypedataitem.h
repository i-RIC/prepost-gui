#ifndef PREPROCESSORGRIDTYPEDATAITEM_H
#define PREPROCESSORGRIDTYPEDATAITEM_H

#include "../pre_global.h"
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>

#include <guicore/image/imagesettingcontainer.h>

#include <QList>
#include <QMap>

#include <unordered_map>

class ColorMapSettingToolBarWidgetController;
class SolverDefinitionGridAttribute;
class SolverDefinitionGridType;
class PreProcessorGeoDataTopDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;

class vtkActor2D;

class QAction;

class PREDLL_EXPORT PreProcessorGridTypeDataItem : public PreProcessorGridTypeDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	~PreProcessorGridTypeDataItem();

	const std::string& name() const;
	SolverDefinitionGridType* gridType() const override;

	PreProcessorGeoDataTopDataItemInterface* geoDataTop() const override;
	PreProcessorHydraulicDataTopDataItemInterface* hydraulicDataTop() const override;

	const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions() const override;
	PreProcessorGridAndGridCreatingConditionDataItemInterface* condition(const std::string& name) const;

	bool isChildDeletable(const PreProcessorGridAndGridCreatingConditionDataItemInterface* child) const;
	void addCustomMenuItems(QMenu* menu) override;
	bool isChildCaptionAvailable(const QString& caption);
	ColorMapSettingContainerI* colorMapSetting(const std::string& attName) const override;
	std::unordered_map<std::string, ColorMapSettingContainerI*> colorMapSettings() const;
	void updateColorBarVisibility(const std::string& attName);

	QAction* addNewGridAction() const;
	bool isGridEdited() const;
	void setGridEdited();
	ModifyCommandDialog* createApplyColorMapSettingDialog(const std::string& name, QWidget *parent) override;
	QUndoCommand* createApplyColorMapSettingCommand(const std::string& name, QUndoCommand* command, bool apply = false);

	ColorMapSettingToolBarWidgetController* createToolBarWidgetController(const std::string& name, QWidget* parent);

public slots:
	void addNewCondition();
	void changeValueRange(const std::string& name);
	void applyColorMapSetting(const std::string& name);

private:
	void unregisterChild(GraphicsWindowDataItem* child) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void updateNewGridActionStatus();
	void setupColorMapSettingContainers();
	void setupColorMapSettingContainer(SolverDefinitionGridAttribute* att);

	QString nextChildCaption();
	std::string nextChildZonename();

	SolverDefinitionGridType* m_gridType;
	PreProcessorGeoDataTopDataItemInterface* m_geoDataTop;
	PreProcessorHydraulicDataTopDataItemInterface* m_hydraulicDataTop;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettingContainers;
	std::unordered_map<std::string, vtkActor2D*> m_colorMapLegendActors;
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> m_conditions;
	/// Action to add new condition.
	QAction* m_addNewGridAction;

	class ApplyColorMapSettingAndRenderCommand;
	class ApplyColorMapSettingDialog;
	class ToolBarWidgetController;
};

#endif // PREPROCESSORGRIDTYPEDATAITEM_H
