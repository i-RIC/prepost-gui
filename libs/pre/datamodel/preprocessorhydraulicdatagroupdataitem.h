#ifndef PREPROCESSORHYDRAULICDATAGROUPDATAITEM_H
#define PREPROCESSORHYDRAULICDATAGROUPDATAITEM_H

#include <guicore/pre/base/preprocessorhydraulicdatagroupdataitemi.h>

#include <set>

class HydraulicData;

class PreProcessorHydraulicDataGroupDataItem : public PreProcessorHydraulicDataGroupDataItemI
{
	Q_OBJECT

public:
	PreProcessorHydraulicDataGroupDataItem(HydraulicDataCreator* creator, PreProcessorDataItem* parent);

	HydraulicDataCreator* creator() const override;
	const QList<PreProcessorHydraulicDataDataItemI*> hydraulicDatas() const override;
	void moveUpItem(int index) override;
	void moveDownItem(int index) override;
	void deleteItem(int index) override;
	void addImportAction(QMenu* menu) override;

public slots:
	void import();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void clear();

	void setUniqueNameAndCaption(HydraulicData* data);
	std::set<QString> usedCaptions() const;

	HydraulicDataCreator* m_creator;
};

#endif // PREPROCESSORHYDRAULICDATAGROUPDATAITEM_H
