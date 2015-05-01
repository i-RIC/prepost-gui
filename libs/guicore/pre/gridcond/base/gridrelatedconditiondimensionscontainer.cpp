#include "gridrelatedconditiondimensionscontainer.h"
#include "../../../solverdef/solverdefinitiongridrelatedcondition.h"
#include "../../../solverdef/solverdefinitiongridrelatedconditiondimension.h"
#include "gridrelatedconditioncontainer.h"
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>

GridRelatedConditionDimensionsContainer::GridRelatedConditionDimensionsContainer(SolverDefinitionGridRelatedCondition* conddef, QObject* parent)
	: QObject(parent)
{
	const QList<SolverDefinitionGridRelatedConditionDimension*>& dims = conddef->dimensions();
	for (int i = 0; i < dims.count(); ++i) {
		SolverDefinitionGridRelatedConditionDimension* dim = dims.at(i);
		GridRelatedConditionDimensionContainer* cont = dim->buildContainer();
		m_containers.append(cont);
		GridRelatedConditionDimensionSelectWidget* widget = dim->buildSelectWidget(cont);
		m_selectWidgets.append(widget);

		connect(cont, SIGNAL(valuesChanged()), this, SIGNAL(valuesChanged()));
		connect(cont, SIGNAL(currentIndexChanged(bool)), this, SLOT(handleIndexChange(bool)));
	}
}

GridRelatedConditionDimensionsContainer::~GridRelatedConditionDimensionsContainer()
{
	for (int i = 0; i < m_containers.count(); ++i) {
		delete m_containers.at(i);
	}
	for (int i = 0; i < m_selectWidgets.count(); ++i) {
		delete m_selectWidgets.at(i);
	}
}

int GridRelatedConditionDimensionsContainer::currentIndex() const
{
	return calculateIndex();
}

QList<int> GridRelatedConditionDimensionsContainer::calculateIndices(int index)
{
	QList<int> ret;
	int factor = 1;
	for (int i = 0; i < m_containers.size() - 1; ++i) {
		GridRelatedConditionDimensionContainer* container = m_containers.at(i);
		if (container->count() != 0) {
			factor = factor * container->count();
		}
	}
	for (int i = m_containers.size() - 1; i >= 0; --i) {
		GridRelatedConditionDimensionContainer* container = m_containers.at(i);
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

void GridRelatedConditionDimensionsContainer::setCurrentIndex(int newIndex, bool noDraw)
{
	QList<int> indices = calculateIndices(newIndex);
	for (int i = 0; i < m_containers.size(); ++i) {
		GridRelatedConditionDimensionContainer* container = m_containers.at(i);
		container->setCurrentIndex(indices.at(i), noDraw);
	}
}

int GridRelatedConditionDimensionsContainer::maxIndex() const
{
	int max = 1;
	for (int i = 0; i < m_containers.size(); ++i) {
		GridRelatedConditionDimensionContainer* container = m_containers.at(i);
		if (container->count() != 0) {
			max = max * container->count();
		}
	}
	return max - 1;
}

int GridRelatedConditionDimensionsContainer::calculateIndex(GridRelatedConditionDimensionContainer* cont, int index, int size) const
{
	int ret = 0;
	int factor = 1;
	for (int i = 0; i < m_containers.size(); ++i) {
		GridRelatedConditionDimensionContainer* container = m_containers.at(i);
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

void GridRelatedConditionDimensionsContainer::handleIndexChange(bool noDraw)
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
