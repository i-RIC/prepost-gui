#ifndef PREPROCESSORGRIDTYPEDATAITEMI_H
#define PREPROCESSORGRIDTYPEDATAITEMI_H

#include "../../guicore_global.h"

#include "preprocessordataitem.h"
#include <QList>

class ColorMapSettingContainerI;
class ModifyCommandDialog;
class SolverDefinitionGridType;
class PreProcessorGeoDataTopDataItemI;
class PreProcessorHydraulicDataTopDataItemI;
class PreProcessorGridAndGridCreatingConditionDataItemI;

class QString;

class GUICOREDLL_EXPORT PreProcessorGridTypeDataItemI : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorGridTypeDataItemI(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
		PreProcessorDataItem(itemlabel, icon, parent)
	{}
	virtual ~PreProcessorGridTypeDataItemI() {}

	virtual ColorMapSettingContainerI* colorMapSetting(const std::string& attName) const = 0;
	virtual ModifyCommandDialog* createApplyColorMapSettingDialog(const std::string& name, QWidget *parent) = 0;

	virtual SolverDefinitionGridType* gridType() const = 0;
	virtual const QList<PreProcessorGridAndGridCreatingConditionDataItemI*>& conditions() const = 0;
	virtual PreProcessorGeoDataTopDataItemI* geoDataTop() const = 0;
	virtual PreProcessorHydraulicDataTopDataItemI* hydraulicDataTop() const = 0;

signals:
	void colorMapSettingChanged(const std::string& name);
};

#endif // PREPROCESSORGRIDTYPEDATAITEMI_H
