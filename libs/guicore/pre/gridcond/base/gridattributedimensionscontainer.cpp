#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "../../../solverdef/solverdefinitiongridattributedimension.h"
#include "gridattributecontainer.h"
#include "gridattributedimensionscontainer.h"

#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>

GridAttributeDimensionsContainer::GridAttributeDimensionsContainer(SolverDefinitionGridAttribute* conddef, QObject* parent)
	: QObject(parent)
{
	const QList<SolverDefinitionGridAttributeDimension*>& dims = conddef->dimensions();
	for (int i = 0; i < dims.count(); ++i) {
		SolverDefinitionGridAttributeDimension* dim = dims.at(i);
		GridAttributeDimensionContainer* cont = dim->buildContainer();
		m_containers.append(cont);
		GridAttributeDimensionSelectWidget* widget = dim->buildSelectWidget(cont);
		m_selectWidgets.append(widget);

		connect(cont, SIGNAL(valuesChanged()), this, SIGNAL(valuesChanged()));
		connect(cont, SIGNAL(currentIndexChanged(bool)), this, SLOT(handleIndexChange(bool)));
	}
}

GridAttributeDimensionsContainer::~GridAttributeDimensionsContainer()
{
	for (auto c : m_containers) {
		delete c;
	}
	for (auto w : m_selectWidgets) {
		delete w;
	}
}

int GridAttributeDimensionsContainer::currentIndex() const
{
	return calculateIndex();
}

QList<int> GridAttributeDimensionsContainer::calculateIndices(int index)
{
	QList<int> ret;
	int factor = 1;
	for (int i = 0; i < m_containers.size() - 1; ++i) {
		GridAttributeDimensionContainer* container = m_containers.at(i);
		if (container->count() != 0) {
			factor = factor * container->count();
		}
	}
	for (int i = m_containers.size() - 1; i >= 0; --i) {
		GridAttributeDimensionContainer* container = m_containers.at(i);
		int index2 = index / factor;
		ret.push_front(index2);
		index = index % factor;
		if (i != 0) {
			container = m_containers.at(i - 1);
			if (container->count() != 0) {
				factor = factor / container->count();
			}
		}
	}
	return ret;
}

void GridAttributeDimensionsContainer::setCurrentIndex(int newIndex, bool noDraw)
{
	QList<int> indices = calculateIndices(newIndex);
	for (int i = 0; i < m_containers.size(); ++i) {
		GridAttributeDimensionContainer* container = m_containers.at(i);
		container->setCurrentIndex(indices.at(i), noDraw);
	}
}

int GridAttributeDimensionsContainer::maxIndex() const
{
	int max = 1;
	for (int i = 0; i < m_containers.size(); ++i) {
		GridAttributeDimensionContainer* container = m_containers.at(i);
		if (container->count() != 0) {
			max = max * container->count();
		}
	}
	return max - 1;
}

int GridAttributeDimensionsContainer::calculateIndex(GridAttributeDimensionContainer* cont, int index, int size) const
{
	int ret = 0;
	int factor = 1;
	for (int i = 0; i < m_containers.size(); ++i) {
		GridAttributeDimensionContainer* container = m_containers.at(i);
		if (cont != nullptr && cont == container) {
			ret = ret + factor * index;
			if (size != 0) {
				factor = factor * size;
			}
		} else {
			ret = ret + factor * container->currentIndex();
			if (container->count() != 0) {
				factor = factor * container->count();
			}
		}
	}
	return ret;
}

void GridAttributeDimensionsContainer::handleIndexChange(bool noDraw)
{
	int newIndex = currentIndex();
	emit currentIndexChanged();
	emit currentIndexChanged(m_currentIndex, newIndex);
	m_currentIndex = newIndex;

	PreProcessorRawDataGroupDataItemInterface* gItem =
		dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent());
	if (gItem != nullptr && ! noDraw) {
		gItem->renderGraphicsView();
	}
}
