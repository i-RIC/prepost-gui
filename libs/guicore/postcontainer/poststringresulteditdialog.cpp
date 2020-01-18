#include "poststringresult.h"
#include "poststringresultargument.h"
#include "poststringresultargumenteditdialog.h"
#include "poststringresulteditdialog.h"
#include "postzonedatacontainer.h"
#include "ui_poststringresulteditdialog.h"

#include <QMessageBox>

#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

PostStringResultEditDialog::PostStringResultEditDialog(QWidget *parent) :
	QDialog(parent),
	m_stringResult {new PostStringResult (nullptr)},
	m_rejected {false},
	m_zoneDataContainer {nullptr},
	ui(new Ui::PostStringResultEditDialog)
{
	ui->setupUi(this);
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addArgument()));
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editSelectedArgument()));
	connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedArgument()));
	connect(ui->testButton, SIGNAL(clicked()), this, SLOT(test()));
}

PostStringResultEditDialog::~PostStringResultEditDialog()
{
	delete m_stringResult;
	delete ui;
}

void PostStringResultEditDialog::setZoneDataContainer(PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;
	m_stringResult->setParent(container);
}

PostStringResult* PostStringResultEditDialog::stringResult() const
{
	return m_stringResult;
}

void PostStringResultEditDialog::setStringResult(PostStringResult* result)
{
	m_stringResult->copyValue(*result);

	ui->scriptEdit->setPlainText(m_stringResult->script());
	updateArgumentsTable();
}

void PostStringResultEditDialog::accept()
{
	if (! check()) {return;}

	QDialog::accept();
}

void PostStringResultEditDialog::addArgument()
{
	PostStringResultArgumentEditDialog dialog(this);
	dialog.setZoneDataContainer(m_zoneDataContainer);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto newArg = dialog.argument();
	m_stringResult->arguments().push_back(newArg);

	updateArgumentsTable();
}

void PostStringResultEditDialog::editSelectedArgument()
{
	if (ui->inputTableWidget->currentRow() < 0) {return;}
	int row = ui->inputTableWidget->currentRow();

	PostStringResultArgumentEditDialog dialog(this);
	dialog.setZoneDataContainer(m_zoneDataContainer);
	auto& args = m_stringResult->arguments();
	auto arg = args.at(row);
	dialog.setArgument(arg);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto newArg = dialog.argument();
	delete args[row];
	args[row] = newArg;

	updateArgumentsTable();
}

void PostStringResultEditDialog::deleteSelectedArgument()
{
	int row = ui->inputTableWidget->currentIndex().row();
	if (row < 0) {return;}

	PostStringResultArgument* arg = m_stringResult->arguments().at(row);
	QString name = arg->refName().c_str();
	int result = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete %1?").arg(name), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}

	m_stringResult->removeArgument(row);
	updateArgumentsTable();

	if (m_stringResult->arguments().size() == 0) {return;}
	if (row == m_stringResult->arguments().size()) {--row;}
	ui->inputTableWidget->selectRow(row);
}

void PostStringResultEditDialog::test()
{
	applySetting();
	QString result;
	bool ok = m_stringResult->testDefinition(this, &result);
	if (ok) {
		ui->testEdit->setPlainText(result);
	} else {
		ui->testEdit->setPlainText("----");
	}
}

bool PostStringResultEditDialog::check()
{
	applySetting();
	return m_stringResult->testDefinition(this);
}

void PostStringResultEditDialog::applySetting()
{
	m_stringResult->setScript(ui->scriptEdit->toPlainText());
}

void PostStringResultEditDialog::updateArgumentsTable()
{
	auto w = ui->inputTableWidget;
	const auto& args = m_stringResult->arguments();

	w->setRowCount(static_cast<int>(args.size()));
	for (int i = 0; i < args.size(); ++i) {
		PostStringResultArgument* a = args.at(i);
		QTableWidgetItem* item = nullptr;
		QString type = setupType(i);
		item = new QTableWidgetItem(type);
		w->setItem(i, 0, item);
		item = new QTableWidgetItem(a->name().c_str());
		w->setItem(i, 1, item);
		item = new QTableWidgetItem(a->refName().c_str());
		w->setItem(i, 2, item);
		QString index = setupIndex(i);
		item = new QTableWidgetItem(index);
		w->setItem(i, 3, item);
		item = new QTableWidgetItem(a->valueForTest());
		w->setItem(i, 4, item);
	}
}

QString PostStringResultEditDialog::setupType(int row)
{
	const auto& args = m_stringResult->arguments();
	PostStringResultArgument* a = args.at(row);

	if (a->type() == PostStringResultArgument::Type::BaseIterative) {
		return tr("Global");
	} else if (a->type() == PostStringResultArgument::Type::GridNode) {
		return tr("Node");
	} else if (a->type() == PostStringResultArgument::Type::GridCell) {
		return tr("Cell");
	} else if (a->type() == PostStringResultArgument::Type::GridEdgeI) {
		return tr("EdgeI");
	} else if (a->type() == PostStringResultArgument::Type::GridEdgeJ) {
		return tr("EdgeJ");
	} else if (a->type() == PostStringResultArgument::Type::GridEdgeK) {
		return tr("EdgeK");
	}
	// to avoid compiler warning
	return "";
}

QString PostStringResultEditDialog::setupIndex(int row)
{
	const auto& args = m_stringResult->arguments();
	PostStringResultArgument* a = args.at(row);

	auto data = m_zoneDataContainer->data();
	auto st = vtkStructuredGrid::SafeDownCast(data);
	auto ust = vtkStructuredGrid::SafeDownCast(data);
	int dim[3];
	if (st != nullptr) {
		st->GetDimensions(dim);
	}

	QStringList vals;
	if (a->type() == PostStringResultArgument::Type::BaseIterative) {
		return "";
	} else if (a->type() == PostStringResultArgument::Type::GridNode) {
		if (st != nullptr) {
			vals.push_back(QString::number(a->i() + 1));
			if (dim[1] > 1) {
				vals.push_back(QString::number(a->j() + 1));
				if (dim[2] > 1) {
					vals.push_back(QString::number(a->k() + 1));
				}
			}
		} else if (ust != nullptr) {
			vals.push_back(QString::number(a->index() + 1));
		}
	} else if (a->type() == PostStringResultArgument::Type::GridCell) {
		if (st != nullptr) {
			vals.push_back(QString::number(a->i() + 1));
			if (dim[1] > 1) {
				vals.push_back(QString::number(a->j() + 1));
				if (dim[2] > 1) {
					vals.push_back(QString::number(a->k() + 1));
				}
			}
		} else if (ust != nullptr) {
			vals.push_back(QString::number(a->index() + 1));
		}
	} else if (a->type() == PostStringResultArgument::Type::GridEdgeI) {
		// @todo implement this
	} else if (a->type() == PostStringResultArgument::Type::GridEdgeJ) {
		// @todo implement this
	} else if (a->type() == PostStringResultArgument::Type::GridEdgeK) {
		// @todo implement this
	}
	return vals.join(", ");
}
