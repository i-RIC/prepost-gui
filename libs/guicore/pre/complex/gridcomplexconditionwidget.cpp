#include "ui_gridcomplexconditionwidget.h"

#include "../../base/iricmainwindowinterface.h"
#include "../../project/inputcond/inputconditioncontainerset.h"
#include "../../project/inputcond/inputconditionpage.h"
#include "../../project/inputcond/inputconditionwidgetset.h"
#include "../../project/inputcond/inputconditionwidgetset.h"
#include "../../solverdef/solverdefinition.h"
#include "../../solverdef/solverdefinitiontranslator.h"
#include "gridcomplexconditionwidget.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomDocument>
#include <QDomNode>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCodec>

#include <cgnslib.h>
#include <iriclib.h>

GridComplexConditionWidget::GridComplexConditionWidget(iRICMainWindowInterface* mw, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::GridComplexConditionWidget)
{
	m_modified = false;

	ui->setupUi(this);
	m_containerSet = new InputConditionContainerSet(mw);
	connect(m_containerSet, SIGNAL(modified()), this, SLOT(setModified()));
	m_widgetSet = new InputConditionWidgetSet();

	m_captionContainer.setName("_caption");
	m_colorContainer.setName("_color");
	m_isDefaultContainer.setName("_isdefault");

	setHidden(true);

	connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SIGNAL(captionChanged(QString)));
	connect(ui->isDefaultCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(defaultChecked(bool)));
	connect(ui->isDefaultCheckBox, SIGNAL(toggled(bool)), this, SLOT(handleDefaultCheck(bool)));
}

GridComplexConditionWidget::~GridComplexConditionWidget()
{
	delete ui;
	m_containerSet->clear();
	delete m_containerSet;
	m_widgetSet->clear();
	delete m_widgetSet;
}

void GridComplexConditionWidget::setup(SolverDefinition* def, const QDomElement& elem, const QLocale& locale)
{
	// open solve definition file
	SolverDefinitionTranslator t(def->folder().absolutePath(), locale);
	// setup ContainerSet first.
	m_containerSet->setup(elem, true);
	// setup WidgetSet.
	m_widgetSet->setup(elem, *m_containerSet, t, true);

	InputConditionPage* page = new InputConditionPage(elem, m_widgetSet, t, this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(page);
	ui->settingGroupBox->setLayout(layout);
}

void GridComplexConditionWidget::setNameAndNumber(QString name, int number)
{
	m_containerSet->setComplexProperty(name, number);
	m_captionContainer.setComplexProperty(name, number);
	m_colorContainer.setComplexProperty(name, number);
	m_isDefaultContainer.setComplexProperty(name, number);
}

void GridComplexConditionWidget::load(const int /*fn*/)
{
	m_containerSet->load();
	m_captionContainer.load();
	m_colorContainer.load();
	m_isDefaultContainer.load();

	setCaption(m_captionContainer.value());
	QColor color(m_colorContainer.value());
	ui->colorWidget->setColor(color);
	ui->isDefaultCheckBox->setChecked(m_isDefaultContainer.value() == 1);

	m_modified = false;
}

void GridComplexConditionWidget::save(const int /*fn*/)
{
	m_containerSet->save();
	m_captionContainer.setValue(ui->nameEdit->text());
	m_captionContainer.save();
	m_colorContainer.setValue(ui->colorWidget->color().name());
	m_colorContainer.save();
	m_isDefaultContainer.setValue(0);
	if (ui->isDefaultCheckBox->isChecked()) {m_isDefaultContainer.setValue(1);}
	m_isDefaultContainer.save();

	m_modified = false;
}

void GridComplexConditionWidget::setModified()
{
	m_modified = true;
}

void GridComplexConditionWidget::setCaption(const QString& caption)
{
	ui->nameEdit->setText(caption);
}

const QString GridComplexConditionWidget::caption() const
{
	return ui->nameEdit->text();
}

QColor GridComplexConditionWidget::color() const
{
	return ui->colorWidget->color();
}

void GridComplexConditionWidget::setColor(const QColor& color)
{
	ui->colorWidget->setColor(color);
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

GridComplexConditionWidget::Setting GridComplexConditionWidget::setting()
{
	m_captionContainer.setValue(ui->nameEdit->text());
	m_colorContainer.setValue(ui->colorWidget->color().name());
	m_isDefaultContainer.setValue(ui->isDefaultCheckBox->isChecked() ? 1 : 0);

	GridComplexConditionWidget::Setting setting;
	setting.containerSet = m_containerSet->clone();
	setting.captionContainer = m_captionContainer;
	setting.colorContainer = m_colorContainer;
	setting.isDefaultContainer = m_isDefaultContainer;
	return setting;
}

void GridComplexConditionWidget::setSetting(const GridComplexConditionWidget::Setting& setting)
{
	m_containerSet->copyValues(setting.containerSet);
	m_captionContainer = setting.captionContainer;
	m_colorContainer = setting.colorContainer;
	m_isDefaultContainer = setting.isDefaultContainer;

	ui->nameEdit->setText(m_captionContainer.value());
	ui->colorWidget->setColor(QColor(m_colorContainer.value()));
	ui->isDefaultCheckBox->setChecked(m_isDefaultContainer.value() == 1);
}

void GridComplexConditionWidget::handleDefaultCheck(bool checked)
{
	if (checked) {
		ui->isDefaultCheckBox->setEnabled(false);
	}
}
