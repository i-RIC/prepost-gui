#ifndef PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H
#define PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H

#include "../../guicore_global.h"

#include "preprocessordataitem.h"
#include <QList>

class ColorMapSettingContainerI;
class ModifyCommandDialog;
class SolverDefinitionGridType;
class PreProcessorGeoDataTopDataItemInterface;
class PreProcessorHydraulicDataTopDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;

class QString;

class GUICOREDLL_EXPORT PreProcessorGridTypeDataItemInterface : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorGridTypeDataItemInterface(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
		PreProcessorDataItem(itemlabel, icon, parent)
	{}
	virtual ~PreProcessorGridTypeDataItemInterface() {}

	virtual ColorMapSettingContainerI* colorMapSetting(const std::string& attName) const = 0;
	virtual ModifyCommandDialog* createApplyColorMapSettingDialog(const std::string& name, QWidget *parent) = 0;

	virtual SolverDefinitionGridType* gridType() const = 0;
	virtual const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions() const = 0;
	virtual PreProcessorGeoDataTopDataItemInterface* geoDataTop() const = 0;
	virtual PreProcessorHydraulicDataTopDataItemInterface* hydraulicDataTop() const = 0;

signals:
	void colorMapSettingChanged(const std::string& name);
};

#endif // PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H
