#include "ui_gridrelatedconditiondimensioncomboboxselectwidget.h"
#include "gridrelatedconditiondimensioncomboboxselectwidget.h"
#include "../base/gridrelatedconditiondimensioncontainer.h"
#include "../../../solverdef/solverdefinitiongridrelatedconditiondimension.h"

GridRelatedConditionDimensionComboboxSelectWidget::GridRelatedConditionDimensionComboboxSelectWidget(GridRelatedConditionDimensionContainer* container, QWidget* parent) :
	GridRelatedConditionDimensionSelectWidget(container, parent),
	ui(new Ui::GridRelatedConditionDimensionComboboxSelectWidget)
{
	ui->setupUi(this);
	connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentIndex(int)));
	ui->captionLabel->setText(container->definition()->caption());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	applyValues();
}

GridRelatedConditionDimensionComboboxSelectWidget::~GridRelatedConditionDimensionComboboxSelectWidget()
{
	delete ui;
}

void GridRelatedConditionDimensionComboboxSelectWidget::doApplyValues()
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

void GridRelatedConditionDimensionComboboxSelectWidget::doSetCurrentIndex(int newIndex)
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
