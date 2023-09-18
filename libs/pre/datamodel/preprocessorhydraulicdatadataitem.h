#ifndef PREPROCESSORHYDRAULICDATADATAITEM_H
#define PREPROCESSORHYDRAULICDATADATAITEM_H

#include <guicore/pre/base/preprocessorhydraulicdatadataitemi.h>

class PreProcessorHydraulicDataDataItem : public PreProcessorHydraulicDataDataItemI
{
public:
	PreProcessorHydraulicDataDataItem(PreProcessorDataItem* parent);

	HydraulicData* hydraulicData() const override;
	void setHydraulicData(HydraulicData* data) override;
	QStringList containedFiles() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	HydraulicData* m_hydraulicData;
};

#endif // PREPROCESSORHYDRAULICDATADATAITEM_H
