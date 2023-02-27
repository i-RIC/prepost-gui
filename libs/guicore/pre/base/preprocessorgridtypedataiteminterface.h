#ifndef PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H
#define PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H

#include "preprocessordataitem.h"
#include <QList>

class ColorMapSettingContainerI;
class SolverDefinitionGridType;
class PreProcessorGeoDataTopDataItemInterface;
class PreProcessorHydraulicDataTopDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;

class QString;

class PreProcessorGridTypeDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorGridTypeDataItemInterface(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
		PreProcessorDataItem(itemlabel, icon, parent)
	{}
	virtual ~PreProcessorGridTypeDataItemInterface() {}

	virtual ColorMapSettingContainerI* colorMapSetting(const std::string& attName) const = 0;

	virtual SolverDefinitionGridType* gridType() const = 0;
	virtual const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions() const = 0;
	virtual PreProcessorGeoDataTopDataItemInterface* geoDataTop() const = 0;
	virtual PreProcessorHydraulicDataTopDataItemInterface* hydraulicDataTop() const = 0;
};

#endif // PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H
