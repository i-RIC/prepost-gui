#include "ui_gridattributedimensioncomboboxselectwidget.h"

#include "../../../solverdef/solverdefinitiongridattributedimension.h"
#include "../base/gridattributedimensioncontainer.h"
#include "gridattributedimensioncomboboxselectwidget.h"

GridAttributeDimensionComboboxSelectWidget::GridAttributeDimensionComboboxSelectWidget(GridAttributeDimensionContainer* container, QWidget* parent) :
	GridAttributeDimensionSelectWidget(container, parent),
	ui(new Ui::GridAttributeDimensionComboboxSelectWidget)
{
	ui->setupUi(this);
	connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentIndex(int)));
	ui->captionLabel->setText(container->definition()->caption());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	applyValues();
}

GridAttributeDimensionComboboxSelectWidget::~GridAttributeDimensionComboboxSelectWidget()
{
	delete ui;
}

void GridAttributeDimensionComboboxSelectWidget::doApplyValues()
{
	ui->comboBox->clear();
	for (int i = 0; i < m_container->count(); ++i) {
		QVariant v = m_container->variantValue(i);
		ui->comboBox->addItem(v.toString());
	}
	int currentIndex = m_container->currentIndex();
	if (currentIndex >= 0 && currentIndex < m_container->count()) {
		ui->comboBox->setCurrentIndex(currentIndex);
	}
}

void GridAttributeDimensionComboboxSelectWidget::doSetCurrentIndex(int newIndex)
{
	if (newIndex < 0 || newIndex >= m_container->count()) {
		return;
	}
	if (ui->comboBox->currentIndex() != newIndex) {
		ui->comboBox->setCurrentIndex(newIndex);
	}
	if (m_container->currentIndex() != newIndex) {
		m_container->setCurrentIndex(newIndex);
	}
}
