#ifndef PREPROCESSORHYDRAULICDATADATAITEM_H
#define PREPROCESSORHYDRAULICDATADATAITEM_H

#include <guicore/pre/base/preprocessorhydraulicdatadataiteminterface.h>

class PreProcessorHydraulicDataDataItem : public PreProcessorHydraulicDataDataItemInterface
{
public:
	PreProcessorHydraulicDataDataItem(PreProcessorDataItem* parent);

	HydraulicData* hydraulicData() const override;
	void setHydraulicData(HydraulicData* data) override;
	QStringList containedFiles() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	HydraulicData* m_hydraulicData;
};

#endif // PREPROCESSORHYDRAULICDATADATAITEM_H
