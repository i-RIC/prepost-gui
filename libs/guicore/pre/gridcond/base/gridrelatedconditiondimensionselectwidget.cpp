#include "gridrelatedconditiondimensioncontainer.h"
#include "gridrelatedconditiondimensionselectwidget.h"

GridRelatedConditionDimensionSelectWidget::GridRelatedConditionDimensionSelectWidget(GridRelatedConditionDimensionContainer* container, QWidget* parent)
	: QWidget(parent)
{
	m_container = container;
	connect(m_container, SIGNAL(valuesChanged()), this, SLOT(applyValues()));
	connect(m_container, SIGNAL(currentIndexChanged(int,bool)), this, SLOT(setCurrentIndex(int)));
	connect(this, SIGNAL(currentIndexChanged(int)), m_container, SLOT(setCurrentIndex(int)));
}

GridRelatedConditionDimensionSelectWidget::~GridRelatedConditionDimensionSelectWidget()
{

}

void GridRelatedConditionDimensionSelectWidget::applyValues()
{
	doApplyValues();
}

void GridRelatedConditionDimensionSelectWidget::setCurrentIndex(int newIndex)
{
	doSetCurrentIndex(newIndex);
}
