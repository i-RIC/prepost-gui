#include "crosssections.h"
#include "crosssectionspreprocessorcontroller.h"
#include "../base/dataitemview.h"
#include "../crosssection/crosssection.h"

#include "private/crosssections_impl.h"

#include <QDomElement>
#include <QStandardItem>
#include <QXmlStreamWriter>

CrossSections::CrossSections(DataItem* parent) :
	DataItem {parent},
	impl {new Impl {}}
{}

CrossSections::~CrossSections()
{
	delete impl;
}

CrossSection* CrossSections::addCrossSection()
{
	auto newS = new CrossSection(this);
	newS->setName(nextName());

	return newS;
}

std::vector<CrossSection*> CrossSections::crossSectionVector() const
{
	std::vector<CrossSection*> ret;
	for (auto c : childItems()) {
		ret.push_back(dynamic_cast<CrossSection*> (c));
	}
	return ret;
}

QStandardItem* CrossSections::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconFolder.png"), tr("Cross Sections"));
	setupStandardItem(item);
	return item;
}

DataItemController* CrossSections::buildPreProcessorDataItemController(Model* model)
{
	return new CrossSectionsPreProcessorController(model, this);
}

DataItemView* CrossSections::buildPreProcessorDataItemView(Model* model)
{
	return new DataItemView(model, this);
}

QString CrossSections::nextName() const
{
	int id = 1;
	while (1) {
		QString name = QString("X%1").arg(id);
		bool found = false;
		for (CrossSection* cs : crossSectionVector()) {
			if (cs->name() == name) {
				found = true;
				break;
			}
		}
		if (! found) {return name;}

		++ id;
	}
}

void CrossSections::doLoadFromMainFile(const QDomElement& node)
{
	int id = 1;
	auto children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		QDomElement c = children.at(i).toElement();
		CrossSection* cs = new CrossSection(this);
		cs->setId(id);
		cs->loadFromMainFile(c);
		++id;
	}
}

void CrossSections::doSaveToMainFile(QXmlStreamWriter* writer) const
{
	int id = 1;
	for (DataItem* item : childItems()) {
		auto cs = dynamic_cast<CrossSection*> (item);
		cs->setId(id);
		writer->writeStartElement("CrossSection");
		cs->saveToMainFile(writer);
		writer->writeEndElement();
		++id;
	}
}
