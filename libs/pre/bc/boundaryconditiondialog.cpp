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
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomDocument>
#include <QDomNode>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCodec>

#include <cgnslib.h>
#include <iriclib.h>

BoundaryConditionDialog::BoundaryConditionDialog(PreProcessorBCDataItem* dataitem, iRICMainWindowInterface* mw, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::BoundaryConditionDialog)
{
	m_modified = false;

	m_dataItem = dataitem;
	ui->setupUi(this);
	m_containerSet = new InputConditionContainerSet(mw);
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
	m_containerSet->setup(elem, true);
	// setup WidgetSet.
	m_widgetSet->setup(elem, *m_containerSet, t, true);

	InputConditionPage* page = new InputConditionPage(elem, m_widgetSet, t, this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(page);
	ui->settingGroupBox->setLayout(layout);
}

void BoundaryConditionDialog::setNameAndNumber(QString name, int number)
{
	m_containerSet->setBCProperty(name, number);
	m_captionContainer.setBCProperty(name, number);
	m_colorContainer.setBCProperty(name, number);
	m_opacityContainer.setBCProperty(name, number);
	m_showNameContainer.setBCProperty(name, number);
	m_pointSizeContainer.setBCProperty(name, number);
}

void BoundaryConditionDialog::load(const int /*fn*/)
{
	m_containerSet->load();
	m_captionContainer.load();
	m_colorContainer.load();
	m_opacityContainer.load();
	m_showNameContainer.load();
	m_pointSizeContainer.load();

	setCaption(m_captionContainer.value());
	QColor color(m_colorContainer.value());
	ui->colorWidget->setColor(color);
	ui->transparencyWidget->setOpacity(m_opacityContainer.value());
	ui->showNameCheckBox->setChecked(m_showNameContainer.value() == 1);
	ui->pointSizeSpinBox->setValue(m_pointSizeContainer.value());

	m_modified = false;
}

void BoundaryConditionDialog::save(const int /*fn*/)
{
	m_containerSet->save();
	m_captionContainer.setValue(ui->nameEdit->text());
	m_captionContainer.save();
	m_colorContainer.setValue(ui->colorWidget->color().name());
	m_colorContainer.save();
	m_opacityContainer.setValue(ui->transparencyWidget->opacity());
	m_opacityContainer.save();
	if (ui->showNameCheckBox->isChecked()) {
		m_showNameContainer.setValue(1);
	} else {
		m_showNameContainer.setValue(0);
	}
	m_showNameContainer.save();
	m_pointSizeContainer.setValue(ui->pointSizeSpinBox->value());
	m_pointSizeContainer.save();

	m_modified = false;
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

void BoundaryConditionDialog::setOpacityPercent(int opacity)
{
	ui->transparencyWidget->setOpacity(opacity);
}

int BoundaryConditionDialog::opacityPercent() const
{
	return ui->transparencyWidget->opacity();
}

QColor BoundaryConditionDialog::color()
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
