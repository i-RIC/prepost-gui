#ifndef CROSSSECTIONS_H
#define CROSSSECTIONS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

class CrossSections : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	CrossSections(DataItem* parent);
	~CrossSections();

	QStandardItem* buildPreProcessorStandardItem() const override;
};

#endif // CROSSSECTIONS_H
