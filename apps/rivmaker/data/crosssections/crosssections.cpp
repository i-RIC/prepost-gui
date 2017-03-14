#include "crosssections.h"
#include "../crosssection/crosssection.h"

#include "private/crosssections_impl.h"

#include <QStandardItem>

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
	childItems().push_back(newS);

	return newS;
}

const std::vector<CrossSection*>& CrossSections::crossSectionVector() const
{
	return impl->m_crossSectionVector;
}

std::vector<CrossSection*>& CrossSections::crossSectionVector()
{
	return impl->m_crossSectionVector;
}

QStandardItem* CrossSections::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("Cross Sections"));
	setupStandardItem(item);
	return item;
}
