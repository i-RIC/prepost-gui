#ifndef PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/enumcontainert.h>

class SolverDefinitionGridAttribute;
class PreProcessorGeoDataGroupDataItem;
class Grid;
class WaitDialog;

class PreProcessorGridAttributeMappingSettingDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	/// Mapping mode
	enum MappingMode {
		mmFromGeoData,         ///< Map attribute value from raw data.
		mmFromOtherAttribute   ///< Map attribute value from other attribute data.
	};
	PreProcessorGridAttributeMappingSettingDataItem(SolverDefinitionGridAttribute* cond, PreProcessorGeoDataGroupDataItem* geodataGroup, PreProcessorDataItem* parent);
	PreProcessorGeoDataGroupDataItem* geodataGroupDataItem() const {return m_geodataGroupDataItem;}
	SolverDefinitionGridAttribute* condition() const {return m_condition;}
	void setDefaultValue(Grid* grid);
	void executeMapping(Grid* grid, WaitDialog* dialog);
	int mappingCount() const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	EnumContainerT<MappingMode> m_mappingMode;

	PreProcessorGeoDataGroupDataItem* m_geodataGroupDataItem;
	SolverDefinitionGridAttribute* m_condition;
};

#endif // PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGDATAITEM_H
