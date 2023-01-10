#include "gridattributedimensioncontainer.h"
#include "gridattributedimensionselectwidget.h"

GridAttributeDimensionSelectWidget::GridAttributeDimensionSelectWidget(GridAttributeDimensionContainer* container, QWidget* parent) :
	QWidget(parent),
	m_container {container},
	m_projectMainFile {nullptr}
{
	connect(m_container, SIGNAL(valuesChanged()), this, SLOT(applyValues()));
	connect(m_container, SIGNAL(currentIndexChanged(int,bool)), this, SLOT(setCurrentIndex(int)));
	connect(this, SIGNAL(currentIndexChanged(int)), m_container, SLOT(setCurrentIndex(int)));
}

GridAttributeDimensionSelectWidget::~GridAttributeDimensionSelectWidget()
{}

GridAttributeDimensionContainer* GridAttributeDimensionSelectWidget::container() const
{
	return m_container;
}

void GridAttributeDimensionSelectWidget::setProjectMainFile(ProjectMainFile* mainFile)
{
	m_projectMainFile = mainFile;
}

void GridAttributeDimensionSelectWidget::applyValues()
{
	doApplyValues();
}

void GridAttributeDimensionSelectWidget::setCurrentIndex(int newIndex)
{
	doSetCurrentIndex(newIndex);
}

ProjectMainFile* GridAttributeDimensionSelectWidget::projectMainFile() const
{
	return m_projectMainFile;
}
