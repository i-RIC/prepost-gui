#ifndef PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
#define PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H

#include "../pre_global.h"

#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>

class Grid;
class GridImporterInterface;
class PreProcessorGridCreatingConditionDataItem;
class PreProcessorBCGroupDataItem;
class PreProcessorBCSettingGroupDataItem;
class PreProcessorGridAttributeMappingSettingTopDataItem;
class PreProcessorGridDataItem;

class QAction;

class PREDLL_EXPORT PreProcessorGridAndGridCreatingConditionDataItem : public PreProcessorGridAndGridCreatingConditionDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorGridAndGridCreatingConditionDataItem(const std::string& zonename, const QString& caption, PreProcessorDataItem* parent);

	const QString& caption() const override;
	const std::string& zoneName() const override;
	PreProcessorGridCreatingConditionDataItemInterface* creatingConditionDataItem() const override;
	PreProcessorBCSettingGroupDataItem* bcSettingGroupDataItem() const;
	PreProcessorBCGroupDataItem* bcGroupDataItem() const;
	PreProcessorGridAttributeMappingSettingTopDataItem* mappingSettingDataItem() const;
	void addCustomMenuItems(QMenu* menu) override;
	PreProcessorGridDataItemInterface* gridDataItem() const override;
	bool isDeletable() const override;
	void handleStandardItemChange() override;
	bool isGridEdited() const  override;
	void setGridEdited() override;
	void setupGridDataItem(Grid* grid);
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
	bool importFromImporter(GridImporterInterface* importer, const QString& filename, const QString& selectedFilter);

	QAction* m_deleteAction;
	QString m_caption;
	std::string m_zoneName;

	PreProcessorGridCreatingConditionDataItemInterface* m_creatingConditionDataItem;
	PreProcessorBCSettingGroupDataItem* m_bcSettingGroupDataItem;
	PreProcessorBCGroupDataItem* m_bcGroupDataItem;
	PreProcessorGridAttributeMappingSettingTopDataItem* m_mappingSettingDataItem;
	PreProcessorGridDataItemInterface* m_gridDataItem;
};

#endif // PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
