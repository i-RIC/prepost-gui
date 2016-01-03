#include "scalarstocolorseditwidget.h"

ScalarsToColorsEditWidget::ScalarsToColorsEditWidget(QWidget* parent) :
	QWidget {parent},
	m_container {nullptr}
{}

ScalarsToColorsEditWidget::~ScalarsToColorsEditWidget()
{}

ScalarsToColorsContainer* ScalarsToColorsEditWidget::container() const
{
	return m_container;
}

void ScalarsToColorsEditWidget::setContainer(ScalarsToColorsContainer* c)
{
	m_container = c;
	setupWidget();
}
