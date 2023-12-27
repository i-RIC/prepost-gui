#ifndef PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
#define PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H

#include "../pre_global.h"

#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataitemi.h>

class v4InputGrid;
class GridImporterI;
class PreProcessorGridCreatingConditionDataItem;
class PreProcessorBCGroupDataItem;
class PreProcessorBCSettingGroupDataItem;
class PreProcessorGridAttributeMappingSettingTopDataItem;
class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;

class QAction;
class QDomElement;

class PREDLL_EXPORT PreProcessorGridAndGridCreatingConditionDataItem : public PreProcessorGridAndGridCreatingConditionDataItemI
{
	Q_OBJECT

public:
	PreProcessorGridAndGridCreatingConditionDataItem(const std::string& zonename, const QString& caption, PreProcessorDataItem* parent);
	~PreProcessorGridAndGridCreatingConditionDataItem();

	const QString& caption() const override;
	const std::string& zoneName() const override;
	PreProcessorGridCreatingConditionDataItemI* creatingConditionDataItem() const override;
	PreProcessorBCSettingGroupDataItem* bcSettingGroupDataItem() const;
	PreProcessorBCGroupDataItem* bcGroupDataItem() const;
	PreProcessorGridAttributeMappingSettingTopDataItem* mappingSettingDataItem() const;
	void addCustomMenuItems(QMenu* menu) override;
	PreProcessorGridDataItemI* gridDataItem() const override;
	bool isDeletable() const override;
	void handleStandardItemChange() override;
	bool isGridEdited() const  override;
	void setGridEdited() override;
	void setupGridDataItem(v4InputGrid* grid);
	int loadFromCgnsFile() override;

	bool importGridFromCgnsFile(const QString& filename);

public slots:
	void informGridCreation();
	void deleteGridAndCondition();
	void importGrid();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void saveExternalData(const QString&) override;

private:
	bool importFromImporter(GridImporterI* importer, const QString& filename, const QString& selectedFilter);
	PreProcessorGridTypeDataItem* gridTypeDataItem() const;

	QAction* m_deleteAction;
	QString m_caption;
	std::string m_zoneName;

	PreProcessorGridCreatingConditionDataItemI* m_creatingConditionDataItem;
	PreProcessorBCSettingGroupDataItem* m_bcSettingGroupDataItem;
	PreProcessorBCGroupDataItem* m_bcGroupDataItem;
	PreProcessorGridAttributeMappingSettingTopDataItem* m_mappingSettingDataItem;
	PreProcessorGridDataItem* m_gridDataItem;

	QDomElement* m_gridSetting;
};

#endif // PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
