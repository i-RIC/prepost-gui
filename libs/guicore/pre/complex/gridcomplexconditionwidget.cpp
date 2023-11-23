#include "ui_gridcomplexconditionwidget.h"

#include "../../solverdef/solverdefinition.h"
#include "gridcomplexconditionwidget.h"
#include "private/gridcomplexconditionwidget_impl.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomNode>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCodec>

GridComplexConditionWidget::Impl::Impl() :
	m_group {nullptr}
{}

GridComplexConditionWidget::Impl::~Impl()
{}

// public interfaces

GridComplexConditionWidget::GridComplexConditionWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::GridComplexConditionWidget),
	impl {new Impl {}}
{
	ui->setupUi(this);
	setHidden(true);

	connect(ui->nameEdit, &QLineEdit::textChanged, this, &GridComplexConditionWidget::captionChanged);
	connect(ui->nameEdit, &QLineEdit::textChanged, this, &GridComplexConditionWidget::handleCaptionEdit);
	connect(ui->colorWidget, &ColorEditWidget::colorChanged, this, &GridComplexConditionWidget::handleColorEdit);
	connect(ui->isDefaultCheckBox, &QCheckBox::toggled, this, &GridComplexConditionWidget::defaultChecked);
	connect(ui->isDefaultCheckBox, &QCheckBox::toggled, this, &GridComplexConditionWidget::handleDefaultCheck);
}

GridComplexConditionWidget::~GridComplexConditionWidget()
{
	delete ui;
}

QString GridComplexConditionWidget::caption() const
{
	return ui->nameEdit->text();
}

void GridComplexConditionWidget::setCaption(const QString& caption)
{
	ui->nameEdit->setText(caption);
}

QColor GridComplexConditionWidget::color() const
{
	return ui->colorWidget->color();
}

void GridComplexConditionWidget::setColor(const QColor& color)
{
	return ui->colorWidget->setColor(color);
}

bool GridComplexConditionWidget::isDefault() const
{
	return ui->isDefaultCheckBox->isChecked();
}

void GridComplexConditionWidget::setIsDefault(bool def)
{
	ui->isDefaultCheckBox->setChecked(def);
	ui->isDefaultCheckBox->setDisabled(def);
}

void GridComplexConditionWidget::hideWidgetsNotForCalculationCondition()
{
	ui->isDefaultCheckBox->hide();
	ui->colorLabel->hide();
	ui->colorWidget->hide();
}

GridComplexConditionGroup* GridComplexConditionWidget::group()
{
	return impl->m_group;
}

void GridComplexConditionWidget::setGroup(GridComplexConditionGroup* group)
{
	impl->m_group = group;

	if (group != nullptr){
		ui->nameEdit->setText(group->caption());
		ui->colorWidget->setColor(group->color());
		ui->isDefaultCheckBox->setChecked(group->isDefault());
		auto w = group->widget();
		setWidget(w);
		w->show();

		connect(group, &GridComplexConditionGroup::captionChanged, this, &GridComplexConditionWidget::setCaption);
		connect(group, &GridComplexConditionGroup::colorChanged, this, &GridComplexConditionWidget::setColor);
		connect(group, &GridComplexConditionGroup::isDefaultChanged, this, &GridComplexConditionWidget::setIsDefault);
	}
}

void GridComplexConditionWidget::handleCaptionEdit(const QString caption)
{
	impl->m_group->setCaption(caption);
}

void GridComplexConditionWidget::handleColorEdit(const QColor& color)
{
	impl->m_group->setColor(color);
}

void GridComplexConditionWidget::handleDefaultCheck(bool checked)
{
	impl->m_group->setIsDefault(checked);

	if (checked) {
		ui->isDefaultCheckBox->setEnabled(false);
	}
}

void GridComplexConditionWidget::setWidget(QWidget* w)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(w);
	ui->settingGroupBox->setLayout(layout);
}
