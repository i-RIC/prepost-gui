#ifndef PREPROCESSORGEODATATOPDATAITEM_H
#define PREPROCESSORGEODATATOPDATAITEM_H

#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include "preprocessorgridtypedataitem.h"

#include <QMap>

class PreProcessorGeoDataGroupDataItem;
class SolverDefinitionGridAttribute;

class PreProcessorGeoDataTopDataItem : public PreProcessorGeoDataTopDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorGeoDataTopDataItem(PreProcessorDataItem* parent);
	~PreProcessorGeoDataTopDataItem();

	SolverDefinitionGridType* gridType() const override;
	const QList<PreProcessorGeoDataGroupDataItemInterface*> groupDataItems() const override;
	PreProcessorGeoDataGroupDataItemInterface* groupDataItem(const std::string& name) const override;
	void informValueRangeChange(const std::string& name);
	void informDataChange();
	void setupActors();
	void updateActorSettings();
	QStringList getGeoDatasNotMapped();
	int saveToCgnsFile() override;
	/// Set all dimensions to the first.
	void setDimensionsToFirst() override;
	void clearDimensionsIfNoDataExists() override;

signals:
	void valueRangeChanged(const std::string& name);
	void dataChanged();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void buildReferenceInformationAttribute();

	std::map<std::string, PreProcessorGeoDataGroupDataItemInterface*> m_itemNameMap;

	SolverDefinitionGridAttribute* m_referenceInformationAttribute;
};

#endif // PREPROCESSORGEODATATOPDATAITEM_H
