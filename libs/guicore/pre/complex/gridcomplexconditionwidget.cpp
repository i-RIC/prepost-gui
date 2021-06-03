#include "ui_gridcomplexconditionwidget.h"

#include "../../solverdef/solverdefinition.h"
#include "gridcomplexconditionwidget.h"
#include "private/gridcomplexconditionwidget_impl.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomDocument>
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

	connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SIGNAL(captionChanged(QString)));
	connect(ui->isDefaultCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(defaultChecked(bool)));
	connect(ui->isDefaultCheckBox, SIGNAL(toggled(bool)), this, SLOT(handleDefaultCheck(bool)));
}

GridComplexConditionWidget::~GridComplexConditionWidget()
{
	delete ui;
	if (impl->m_group != nullptr) {
		auto w = impl->m_group->widget();
		w->setParent(nullptr);
		w->hide();
	}
	delete impl;
}

void GridComplexConditionWidget::setup(SolverDefinition* def, const QDomElement& elem)
{
	impl->m_group = new GridComplexConditionGroup(def, elem);
	setWidget(impl->m_group->widget());
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

GridComplexConditionGroup* GridComplexConditionWidget::group()
{
	impl->m_group->setCaption(ui->nameEdit->text());
	impl->m_group->setColor(ui->colorWidget->color());
	impl->m_group->setIsDefault(ui->isDefaultCheckBox->isChecked());

	return impl->m_group;
}

void GridComplexConditionWidget::setGroup(GridComplexConditionGroup* group)
{
	if (group != nullptr){
		ui->nameEdit->setText(group->caption());
		ui->colorWidget->setColor(group->color());
		ui->isDefaultCheckBox->setChecked(group->isDefault());
		auto w = group->widget();
		w->show();
		setWidget(w);
	}

	delete impl->m_group;
	impl->m_group = group;
}

void GridComplexConditionWidget::handleDefaultCheck(bool checked)
{
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
