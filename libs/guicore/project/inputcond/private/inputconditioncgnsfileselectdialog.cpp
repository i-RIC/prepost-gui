#include "inputconditioncgnsfile.h"
#include "inputconditioncgnsfileselectdialog.h"
#include "inputconditionwidgetcgnsfile.h"
#include "../inputconditionwidgetset.h"
#include "ui_inputconditioncgnsfileselectdialog.h"

#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

InputConditionCgnsFileSelectDialog::InputConditionCgnsFileSelectDialog(QString* cgnsFileForGrid, InputConditionWidgetSet* wset, QWidget *parent) :
	QDialog(parent),
	m_cgnsFileForGrid {cgnsFileForGrid},
	ui(new Ui::InputConditionCgnsFileSelectDialog)
{
	ui->setupUi(this);
	auto cgnsMap = wset->m_cgnsFiles;

	int row = 0;
	for (auto it = cgnsMap.begin(); it != cgnsMap.end(); ++it) {
		auto name = it.key();
		auto cgnsFile = it.value();
		m_cgnsFiles.push_back(cgnsFile);

		auto newLabel = new QLabel(this);
		auto label = dynamic_cast<QLabel*> (wset->widget(name + "_caption"));
		if (label == nullptr) {
			newLabel->setText(tr("CGNS File"));
		} else {
			newLabel->setText(label->text());
		}
		ui->gridLayout->addWidget(newLabel, row, 0);
		InputConditionWidgetCgnsFile* widget = dynamic_cast<InputConditionWidgetCgnsFile*> (wset->widget(name))->clone();
		ui->gridLayout->addWidget(widget, row, 1);
		++ row;

		connect(widget, SIGNAL(valueChanged()), this, SLOT(checkStatus()));
	}

	checkStatus();
}

InputConditionCgnsFileSelectDialog::~InputConditionCgnsFileSelectDialog()
{
	delete ui;
}

InputConditionCgnsFile* InputConditionCgnsFileSelectDialog::firstCgnsFile() const
{
	if (m_cgnsFiles.size() == 0) {return nullptr;}

	return m_cgnsFiles.at(0);
}

void InputConditionCgnsFileSelectDialog::accept()
{
	if (m_cgnsFileForGrid != nullptr) {
		int ret = QMessageBox::information(this, tr("Information"), tr("Do you want to import grid from the CGNS file?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::Yes) {
			if (m_cgnsFiles.size() == 1) {
				*m_cgnsFileForGrid = m_cgnsFiles.at(0)->fileName();
			} else {
				QStringList cgnsNames;
				for (auto cgnsFile : m_cgnsFiles) {
					cgnsNames.push_back(cgnsFile->fileName());
					bool ok;
					QString selected = QInputDialog::getItem(this, tr("Select CGNS file"), tr("Please select from which CGNS file to read grid."), cgnsNames, 0, false, &ok);
					if (! ok) {return;}

					*m_cgnsFileForGrid = selected;
				}
			}
		}
	}
	QDialog::accept();
}

void InputConditionCgnsFileSelectDialog::checkStatus()
{
	bool allOK = true;

	for (auto cgnsFile : m_cgnsFiles) {
		allOK &= cgnsFile->isEffective();
	}
	QPushButton* okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setEnabled(allOK);
}
