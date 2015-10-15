#ifndef PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
#define PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H

#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>

class Grid;
class GridImporterInterface;
class PreProcessorGridCreatingConditionDataItem;
class PreProcessorBCSettingGroupDataItem;
class PreProcessorGridAttributeMappingSettingTopDataItem;
class PreProcessorGridDataItem;

class QAction;

class PreProcessorGridAndGridCreatingConditionDataItem : public PreProcessorGridAndGridCreatingConditionDataItemInterface
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorGridAndGridCreatingConditionDataItem(const QString& zonename, const QString& caption, PreProcessorDataItem* parent);
	/// The caption of the dataitem.
	const QString& caption() override {return m_caption;}
	/// The name of the zone that stores the grid information.
	const QString& zoneName() override {return m_zoneName;}
	PreProcessorGridCreatingConditionDataItemInterface* creatingConditionDataItem() const override {
		return m_creatingConditionDataItem;
	}
	PreProcessorBCSettingGroupDataItem* bcSettingGroupDataItem() const {
		return m_bcSettingGroupDataItem;
	}
	PreProcessorGridAttributeMappingSettingTopDataItem* mappingSettingDataItem() const {
		return m_mappingSettingDataItem;
	}
	void addCustomMenuItems(QMenu* menu) override;
	PreProcessorGridDataItemInterface* gridDataItem() const override {return m_gridDataItem;}
	bool isDeletable() const override;
	void handleStandardItemChange() override;
	bool gridEdited() const  override;
	void toggleGridEditFlag() override;
	void setupGridDataItem(Grid* grid);

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
	QString m_zoneName;
	PreProcessorGridCreatingConditionDataItemInterface* m_creatingConditionDataItem;
	PreProcessorBCSettingGroupDataItem* m_bcSettingGroupDataItem;
	PreProcessorGridAttributeMappingSettingTopDataItem* m_mappingSettingDataItem;
	PreProcessorGridDataItemInterface* m_gridDataItem;
};

#endif // PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
