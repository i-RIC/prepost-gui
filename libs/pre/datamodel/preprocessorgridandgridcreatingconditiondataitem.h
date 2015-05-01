#ifndef PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
#define PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H

#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>

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
	const QString& caption() {return m_caption;}
	/// The name of the zone that stores the grid information.
	const QString& zoneName() {return m_zoneName;}
	PreProcessorGridCreatingConditionDataItemInterface* creatingConditionDataItem() const {
		return m_creatingConditionDataItem;
	}
	PreProcessorBCSettingGroupDataItem* bcSettingGroupDataItem() const {
		return m_bcSettingGroupDataItem;
	}
	PreProcessorGridAttributeMappingSettingTopDataItem* mappingSettingDataItem() const {
		return m_mappingSettingDataItem;
	}
	void addCustomMenuItems(QMenu* menu);
	PreProcessorGridDataItemInterface* gridDataItem() const {return m_gridDataItem;}
	bool isDeletable();
	void handleStandardItemChange();
	bool gridEdited() const ;
	void toggleGridEditFlag();
public slots:
	void informGridCreation();
	void deleteGridAndCondition();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void saveExternalData(const QString&);
private:
	QAction* m_deleteAction;
	QString m_caption;
	QString m_zoneName;
	PreProcessorGridCreatingConditionDataItemInterface* m_creatingConditionDataItem;
	PreProcessorBCSettingGroupDataItem* m_bcSettingGroupDataItem;
	PreProcessorGridAttributeMappingSettingTopDataItem* m_mappingSettingDataItem;
	PreProcessorGridDataItemInterface* m_gridDataItem;
};

#endif // PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEM_H
