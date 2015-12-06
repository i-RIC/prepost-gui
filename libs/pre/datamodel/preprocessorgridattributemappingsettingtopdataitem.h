#ifndef PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPDATAITEM_H

#include "preprocessorcustommappingsetting.h"
#include <guicore/pre/base/preprocessordataitem.h>
#include <QMap>

class PreProcessorGridAttributeMappingSettingDataItem;
class QAction;

class PreProcessorGridAttributeMappingSettingTopDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorGridAttributeMappingSettingTopDataItem(PreProcessorDataItem* parent);
	void setDefaultValues();

public slots:
	void informGeoDataChange();
	void executeMapping();
	void customMapping(bool nomessage = false);
	void customMapping(const std::string& attName, bool nomessage = false);
	QAction* customMappingAction() {return m_customMappingAction;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	bool checkDimensions();

	PreProcessorCustomMappingSetting m_mappingSetting;
	QMap<std::string, PreProcessorGridAttributeMappingSettingDataItem*> m_itemNameMap;
	QAction* m_executeMappingAction;
	QAction* m_customMappingAction;
};


#endif // PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPDATAITEM_H
