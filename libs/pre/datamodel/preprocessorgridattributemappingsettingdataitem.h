#ifndef PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>

class SolverDefinitionGridAttribute;
class PreProcessorRawDataGroupDataItem;
class Grid;
class WaitDialog;

class PreProcessorGridAttributeMappingSettingDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Mapping mode
	enum MappingMode {
		mmFromRawData,         ///< Map attribute value from raw data.
		mmFromOtherAttribute   ///< Map attribute value from other attribute data.
	};
	PreProcessorGridAttributeMappingSettingDataItem(SolverDefinitionGridAttribute* cond, PreProcessorRawDataGroupDataItem* rawdataGroup, PreProcessorDataItem* parent);
	PreProcessorRawDataGroupDataItem* rawdataGroupDataItem() const {return m_rawdataGroupDataItem;}
	SolverDefinitionGridAttribute* condition() const {return m_condition;}
	void setDefaultValue(Grid* grid);
	void executeMapping(Grid* grid, WaitDialog* dialog);
	int mappingCount() const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	MappingMode m_mappingMode;

	PreProcessorRawDataGroupDataItem* m_rawdataGroupDataItem;
	SolverDefinitionGridAttribute* m_condition;
};

#endif // PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGDATAITEM_H
