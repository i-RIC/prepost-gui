#ifndef CROSSSECTIONS_H
#define CROSSSECTIONS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <vector>

class CrossSection;

class CrossSections : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	CrossSections(DataItem* parent);
	~CrossSections();

	CrossSection* addCrossSection();

	const std::vector<CrossSection*>& crossSectionVector() const;
	std::vector<CrossSection*>& crossSectionVector();

	QStandardItem* buildPreProcessorStandardItem() const override;

private:
	class Impl;
	Impl* impl;
};

#endif // CROSSSECTIONS_H
