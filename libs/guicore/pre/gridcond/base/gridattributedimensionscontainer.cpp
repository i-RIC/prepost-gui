#include "../../../project/projectdata.h"
#include "../../../project/projectdataitem.h"
#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "../../../solverdef/solverdefinitiongridattributedimension.h"
#include "../../base/preprocessorgeodatagroupdataitemi.h"
#include "gridattributecontainer.h"
#include "gridattributedimensionscontainer.h"
#include "private/gridattributedimensionscontainer_impl.h"

GridAttributeDimensionsContainer::GridAttributeDimensionsContainer(SolverDefinitionGridAttribute* conddef, ProjectDataItem* parent) :
	ProjectDataItem {parent},
	impl {new Impl {}}
{
	for (auto dim : conddef->dimensions()) {
		GridAttributeDimensionContainer* cont = dim->buildContainer();
		impl->m_containers.push_back(cont);

		connect(cont, SIGNAL(valuesChanged()), this, SIGNAL(valuesChanged()));
		connect(cont, SIGNAL(currentIndexChanged(bool)), this, SLOT(handleIndexChange(bool)));
	}
}

GridAttributeDimensionsContainer::~GridAttributeDimensionsContainer()
{
	for (auto c : impl->m_containers) {
		delete c;
	}
}

const std::vector<GridAttributeDimensionContainer *>& GridAttributeDimensionsContainer::containers() const
{
	return impl->m_containers;
}

std::vector<GridAttributeDimensionContainer *>& GridAttributeDimensionsContainer::containers()
{
	return impl->m_containers;
}

std::vector<GridAttributeDimensionSelectWidget*> GridAttributeDimensionsContainer::buildSelectWidgets()
{
	std::vector<GridAttributeDimensionSelectWidget*> ret;
	for (auto c : impl->m_containers) {
		auto widget = c->definition()->buildSelectWidget(c, projectData()->mainfile());
		ret.push_back(widget);
	}

	return ret;
}

void GridAttributeDimensionsContainer::clear()
{
	for (auto c: impl->m_containers) {
		c->clear();
	}
	emit valuesChanged();
}

int GridAttributeDimensionsContainer::currentIndex() const
{
	return calculateIndex();
}

std::vector<int> GridAttributeDimensionsContainer::calculateIndices(int index) const
{
	std::vector<int> ret;
	int factor = 1;
	for (int i = 0; i < static_cast<int> (impl->m_containers.size()) - 1; ++i) {
		GridAttributeDimensionContainer* container = impl->m_containers.at(i);
		if (container->count() != 0) {
			factor = factor * container->count();
		}
	}
	for (int i = static_cast<int>(impl->m_containers.size()) - 1; i >= 0; --i) {
		GridAttributeDimensionContainer* container = impl->m_containers.at(i);
		int index2 = index / factor;
		ret.insert(ret.begin(), index2);
		index = index % factor;
		if (i != 0) {
			container = impl->m_containers.at(i - 1);
			if (container->count() != 0) {
				factor = factor / container->count();
			}
		}
	}
	return ret;
}

void GridAttributeDimensionsContainer::setCurrentIndex(int newIndex, bool noDraw)
{
	std::vector<int> indices = calculateIndices(newIndex);
	int i = 0;
	for (auto container : impl->m_containers) {
		container->setCurrentIndex(indices.at(i), noDraw);
		++i;
	}
}

int GridAttributeDimensionsContainer::maxIndex() const
{
	int max = 1;
	for (auto container : impl->m_containers) {
		if (container->count() == 0) {continue;}

		max = max * container->count();
	}
	return max - 1;
}


ProjectData* GridAttributeDimensionsContainer::projectData()
{
	return ProjectDataItem::projectData();
}

int GridAttributeDimensionsContainer::calculateIndex(GridAttributeDimensionContainer* cont, int index, int size) const
{
	int ret = 0;
	int factor = 1;
	for (auto container : impl->m_containers) {
		if (cont == container) {
			ret = ret + factor * index;
			if (size != 0) {
				factor = factor * size;
			}
		} else {
			ret = ret + factor * container->currentIndex();
			if (container->count() == 0) {continue;}
			factor = factor * container->count();
		}
	}
	return ret;
}

void GridAttributeDimensionsContainer::handleIndexChange(bool noDraw)
{
	int newIndex = currentIndex();
	emit currentIndexChanged();
	emit currentIndexChanged(impl->m_currentIndex, newIndex);
	impl->m_currentIndex = newIndex;

	PreProcessorGeoDataGroupDataItemI* gItem =
		dynamic_cast<PreProcessorGeoDataGroupDataItemI*>(parent());
	if (gItem != nullptr && ! noDraw) {
		gItem->renderGraphicsView();
	}
}

void GridAttributeDimensionsContainer::doLoadFromProjectMainFile(const QDomNode&)
{}

void GridAttributeDimensionsContainer::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
