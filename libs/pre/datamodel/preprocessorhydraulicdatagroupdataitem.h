#ifndef PREPROCESSORHYDRAULICDATAGROUPDATAITEM_H
#define PREPROCESSORHYDRAULICDATAGROUPDATAITEM_H

#include <guicore/pre/base/preprocessorhydraulicdatagroupdataiteminterface.h>

#include <set>

class HydraulicData;

class PreProcessorHydraulicDataGroupDataItem : public PreProcessorHydraulicDataGroupDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorHydraulicDataGroupDataItem(HydraulicDataCreator* creator, PreProcessorDataItem* parent);

	HydraulicDataCreator* creator() const override;
	const QList<PreProcessorHydraulicDataDataItemInterface*> hydraulicDatas() const override;
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
