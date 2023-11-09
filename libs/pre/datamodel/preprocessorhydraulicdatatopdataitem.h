#ifndef PREPROCESSORHYDRAULICDATATOPDATAITEM_H
#define PREPROCESSORHYDRAULICDATATOPDATAITEM_H

#include <guicore/pre/base/preprocessorhydraulicdatatopdataitemi.h>

class PreProcessorHydraulicDataTopDataItem : public PreProcessorHydraulicDataTopDataItemI
{
public:
	PreProcessorHydraulicDataTopDataItem(PreProcessorDataItem* parent);

	QList<PreProcessorHydraulicDataGroupDataItemI*> groupDataItems() const override;
	PreProcessorHydraulicDataGroupDataItemI* groupDataItem(const std::string& name) const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	std::map<std::string, PreProcessorHydraulicDataGroupDataItemI*> m_itemNameMap;
};

#endif // PREPROCESSORHYDRAULICDATATOPDATAITEM_H
