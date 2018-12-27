#ifndef PREPROCESSORHYDRAULICDATATOPDATAITEM_H
#define PREPROCESSORHYDRAULICDATATOPDATAITEM_H

#include <guicore/pre/base/preprocessorhydraulicdatatopdataiteminterface.h>

class PreProcessorHydraulicDataTopDataItem : public PreProcessorHydraulicDataTopDataItemInterface
{
public:
	PreProcessorHydraulicDataTopDataItem(PreProcessorDataItem* parent);

	QList<PreProcessorHydraulicDataGroupDataItemInterface*> groupDataItems() const override;
	PreProcessorHydraulicDataGroupDataItemInterface* groupDataItem(const std::string& name) const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	std::map<std::string, PreProcessorHydraulicDataGroupDataItemInterface*> m_itemNameMap;
};

#endif // PREPROCESSORHYDRAULICDATATOPDATAITEM_H
