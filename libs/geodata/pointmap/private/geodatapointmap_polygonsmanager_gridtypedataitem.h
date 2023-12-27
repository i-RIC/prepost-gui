#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_GRIDTYPEDATAITEM_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_GRIDTYPEDATAITEM_H

#include "geodatapointmap_polygonsmanager.h"

#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>

class GeoDataPointmap::PolygonsManager::GridTypeDataItem : public PreProcessorGridTypeDataItemInterface
{
public:
	GridTypeDataItem(SolverDefinitionGridAttribute* cond, RootDataItem* rootDataItem, PreProcessorDataItem* parent);

	GeoDataTopDataItem* geoDataTopDataItem() const;

	ColorMapSettingContainerI* colorMapSetting(const std::string& attName) const override;
	ModifyCommandDialog* createApplyColorMapSettingDialog(const std::string& name, QWidget *parent) override;

	SolverDefinitionGridType* gridType() const override;
	const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions() const override;
	PreProcessorGeoDataTopDataItemInterface* geoDataTop() const override;
	PreProcessorHydraulicDataTopDataItemInterface* hydraulicDataTop() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GeoDataTopDataItem* m_geoDataTopDataItem;

	class ModifyDialog;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_GRIDTYPEDATAITEM_H
