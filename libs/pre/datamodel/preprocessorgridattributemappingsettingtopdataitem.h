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
	void informRawDataChange();
	void executeMapping();
	void customMapping(bool nomessage = false);
	void customMapping(const QString& attName, bool nomessage = false);
	QAction* customMappingAction(){return m_customMappingAction;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	bool checkDimensions();

	PreProcessorCustomMappingSetting m_mappingSetting;
	QMap<QString, PreProcessorGridAttributeMappingSettingDataItem*> m_itemNameMap;
	QAction* m_executeMappingAction;
	QAction* m_customMappingAction;
};


#endif // PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPDATAITEM_H
