#include "ui_boundaryconditiondialog.h"

#include "../datamodel/preprocessorbcdataitem.h"
#include "boundaryconditiondialog.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/inputcond/inputconditioncontainerset.h>
#include <guicore/project/inputcond/inputconditionpage.h>
#include <guicore/project/inputcond/inputconditionwidgetset.h>
#include <guicore/project/inputcond/inputconditionwidgetset.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <guicore/solverdef/solverdefinitiontranslator.h>
#include <misc/opacitycontainer.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomDocument>
#include <QDomNode>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCodec>

#include <iriclib_errorcodes.h>

BoundaryConditionDialog::BoundaryConditionDialog(PreProcessorBCDataItem* dataitem, iRICMainWindowInterface* mw, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::BoundaryConditionDialog)
{
	m_modified = false;

	m_dataItem = dataitem;
	ui->setupUi(this);
	m_containerSet = new InputConditionContainerSet();
	connect(m_containerSet, SIGNAL(modified()), this, SLOT(setModified()));
	m_widgetSet = new InputConditionWidgetSet();

	m_captionContainer.setName("_caption");
	m_colorContainer.setName("_color");
	m_opacityContainer.setName("_opacity");
	m_opacityContainer.setDefaultValue(50);
	m_showNameContainer.setName("_showname");
	m_showNameContainer.setDefaultValue(1);
	m_pointSizeContainer.setName("_pointsize");
	m_pointSizeContainer.setDefaultValue(7);

	if (dataitem->condition()->position() == SolverDefinitionBoundaryCondition::pCell) {
		ui->pointSizeLabel->hide();
		ui->pointSizeSpinBox->hide();
	} else if (dataitem->condition()->position() == SolverDefinitionBoundaryCondition::pEdge) {
		ui->pointSizeLabel->setText(tr("Line Width:"));
	}
}

BoundaryConditionDialog::~BoundaryConditionDialog()
{
	delete ui;
	m_containerSet->clear();
	delete m_containerSet;
	m_widgetSet->clear();
	delete m_widgetSet;
}

void BoundaryConditionDialog::setup(SolverDefinition* def, const QDomElement& elem, const QLocale& locale)
{
	// open solve definition file
	SolverDefinitionTranslator t(def->folder().absolutePath(), locale);
	// setup ContainerSet first.
	m_containerSet->setup(elem, *def, t, true);
	// setup WidgetSet.
	m_widgetSet->setup(elem, *m_containerSet, *def, t, true);

	InputConditionPage* page = new InputConditionPage(elem, m_widgetSet, t, this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(page);
	ui->settingGroupBox->setLayout(layout);
}

int BoundaryConditionDialog::load(const iRICLib::H5CgnsConditionGroup& group)
{
	int ier = 0;

	ier = m_containerSet->load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = m_captionContainer.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = m_colorContainer.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = m_opacityContainer.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = m_showNameContainer.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = m_pointSizeContainer.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	setCaption(m_captionContainer.value());
	QColor color(m_colorContainer.value());
	ui->colorWidget->setColor(color);
	ui->transparencyWidget->setOpacityPercent(m_opacityContainer.value());
	ui->showNameCheckBox->setChecked(m_showNameContainer.value() == 1);
	ui->pointSizeSpinBox->setValue(m_pointSizeContainer.value());

	m_modified = false;

	return IRIC_NO_ERROR;
}

int BoundaryConditionDialog::save(iRICLib::H5CgnsConditionGroup* group)
{
	int ier = 0;
	ier = m_containerSet->save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	m_captionContainer.setValue(ui->nameEdit->text());
	ier = m_captionContainer.save(group);

	m_colorContainer.setValue(ui->colorWidget->color().name());
	m_colorContainer.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	m_opacityContainer.setValue(ui->transparencyWidget->opacityPercent());
	m_opacityContainer.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	if (ui->showNameCheckBox->isChecked()) {
		m_showNameContainer.setValue(1);
	} else {
		m_showNameContainer.setValue(0);
	}
	ier = m_showNameContainer.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	m_pointSizeContainer.setValue(ui->pointSizeSpinBox->value());
	ier = m_pointSizeContainer.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	m_modified = false;

	return IRIC_NO_ERROR;
}

void BoundaryConditionDialog::accept()
{
	QString name = ui->nameEdit->text();
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	bool ok = asciiCodec->canEncode(name);
	if (! ok) {
		QMessageBox::warning(this, tr("Warning"), tr("Name has to consist of only English characters."));
		return;
	}
	QDialog::accept();
}

void BoundaryConditionDialog::setModified()
{
	m_modified = true;
}

void BoundaryConditionDialog::setType(const QString& type)
{
	ui->typeWidget->setText(type);
}

void BoundaryConditionDialog::setCaption(const QString& caption)
{
	ui->nameEdit->setText(caption);
}

QString BoundaryConditionDialog::caption() const
{
	return ui->nameEdit->text();
}

void BoundaryConditionDialog::setOpacity(const OpacityContainer& opacity)
{
	ui->transparencyWidget->setOpacity(opacity);
}

OpacityContainer BoundaryConditionDialog::opacity() const
{
	return ui->transparencyWidget->opacity();
}

QColor BoundaryConditionDialog::color() const
{
	return ui->colorWidget->color();
}

void BoundaryConditionDialog::setColor(const QColor& color)
{
	ui->colorWidget->setColor(color);
}

int BoundaryConditionDialog::pointSize() const
{
	return ui->pointSizeSpinBox->value();
}

void BoundaryConditionDialog::setPointSize(int size)
{
	ui->pointSizeSpinBox->setValue(size);
}

bool BoundaryConditionDialog::showName() const
{
	return ui->showNameCheckBox->isChecked();
}

void BoundaryConditionDialog::setShowName(bool showName)
{
	ui->showNameCheckBox->setChecked(showName);
}

void BoundaryConditionDialog::setEditMode()
{
	setWindowTitle(tr("Boundary Condition"));
	ui->nameEdit->setEnabled(true);
	ui->settingGroupBox->show();
}

void BoundaryConditionDialog::setPropertyMode()
{
	setWindowTitle(tr("Boundary Condition Property"));
	ui->nameEdit->setDisabled(true);
	ui->settingGroupBox->hide();
}
