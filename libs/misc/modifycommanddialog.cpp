#include "iricundostack.h"
#include "modifycommanddialog.h"
#include "modifycommandwidget.h"
#include "ui_modifycommanddialog.h"

ModifyCommandDialog::ModifyCommandDialog(QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_widget {nullptr},
	ui(new Ui::ModifyCommandDialog)
{
	ui->setupUi(this);
	ui->importButton->hide();
	ui->exportButton->hide();

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &ModifyCommandDialog::handleButtonClick);
	connect(ui->importButton, &QPushButton::clicked, this, &ModifyCommandDialog::importClicked);
	connect(ui->exportButton, &QPushButton::clicked, this, &ModifyCommandDialog::exportClicked);
}

ModifyCommandDialog::~ModifyCommandDialog()
{
	delete ui;
}

ModifyCommandWidget* ModifyCommandDialog::widget() const
{
	return m_widget;
}

void ModifyCommandDialog::setWidget(ModifyCommandWidget* widget)
{
	m_widget = widget;
	ui->widget->setWidget(widget);
}

void ModifyCommandDialog::showImportButton()
{
	ui->importButton->show();
}

void ModifyCommandDialog::showExportButton()
{
	ui->exportButton->show();
}

void ModifyCommandDialog::accept()
{
	pushCommand(m_widget->createModifyCommand(false));
	QDialog::accept();
}

void ModifyCommandDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void ModifyCommandDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void ModifyCommandDialog::apply()
{
	pushCommand(m_widget->createModifyCommand(true));
	m_applied = true;
}
