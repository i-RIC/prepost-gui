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

	std::vector<CrossSection*> crossSectionVector() const;

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemController* buildPreProcessorDataItemController(Model* model) override;
	DataItemView* buildPreProcessorDataItemView(Model* model) override;

private:
	QString nextName() const;

	void doLoadFromMainFile(const QDomElement& node) override;
	void doSaveToMainFile(QXmlStreamWriter* writer) const override;

	class Impl;
	Impl* impl;
};

#endif // CROSSSECTIONS_H
