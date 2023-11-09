#ifndef PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEMI_H
#define PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

#include <string>

class QString;
class PreProcessorGridDataItemI;
class PreProcessorGridCreatingConditionDataItemI;

class GUICOREDLL_EXPORT PreProcessorGridAndGridCreatingConditionDataItemI : public PreProcessorDataItem
{
public:
	PreProcessorGridAndGridCreatingConditionDataItemI(const QString& caption, PreProcessorDataItem* parent);
	virtual ~PreProcessorGridAndGridCreatingConditionDataItemI();

	virtual const QString& caption() const = 0;
	virtual PreProcessorGridDataItemI* gridDataItem() const = 0;
	virtual const std::string& zoneName() const = 0;
	virtual PreProcessorGridCreatingConditionDataItemI* creatingConditionDataItem() const = 0;
	virtual bool isGridEdited() const = 0;
	virtual void setGridEdited() = 0;
};

#endif // PREPROCESSORGRIDANDGRIDCREATINGCONDITIONDATAITEMI_H
