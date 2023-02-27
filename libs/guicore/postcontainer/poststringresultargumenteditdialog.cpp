#include "postsolutioninfo.h"
#include "poststringresultargument.h"
#include "poststringresultargumenteditdialog.h"
#include "postzonedatacontainer.h"
#include "ui_poststringresultargumenteditdialog.h"

#include <misc/stringtool.h>

#include <QMessageBox>

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

#include <algorithm>

namespace {

void setupComboBoxWithNames(vtkDataSetAttributes* att, QComboBox* comboBox)
{
	comboBox->clear();
	for (int i = 0; i < att->GetNumberOfArrays(); ++i) {
		const char* n = att->GetArrayName(i);
		comboBox->addItem(n);
	}
}

void disableSpinBox(QSpinBox* box)
{
	box->setMaximum(1);
	box->setDisabled(true);
}

} // namespace

PostStringResultArgumentEditDialog::PostStringResultArgumentEditDialog(QWidget *parent) :
	QDialog(parent),
	m_argument {new PostStringResultArgument(nullptr)},
	m_rejected {false},
	m_zoneDataContainer {nullptr},
	ui(new Ui::PostStringResultArgumentEditDialog)
{
	ui->setupUi(this);
	connect(ui->positionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePositionChange()));
	connect(ui->resultNameComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleResultChange()));
}

PostStringResultArgumentEditDialog::~PostStringResultArgumentEditDialog()
{
	if (m_rejected) {
		delete m_argument;
	}
	delete ui;
}

void PostStringResultArgumentEditDialog::setZoneDataContainer(PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;

	auto s = container->solutionInfo();
	if (s->baseIterativeNumericalResults().size() > 0 || s->baseIterativeStringResults().size() > 0) {
		m_types.push_back(PostStringResultArgument::Type::BaseIterative);
		ui->positionComboBox->addItem(tr("Global"));
	}

	if (container->data()->data()->GetPointData()->GetNumberOfArrays() > 0) {
		m_types.push_back(PostStringResultArgument::Type::GridNode);
		ui->positionComboBox->addItem(tr("Node"));
	}
	bool cellDataExists = false;
	auto cd = container->data()->data()->GetCellData();
	for (int i = 0; i < cd->GetNumberOfArrays(); ++i) {
		const char* name = cd->GetArrayName(i);
		if (PostZoneDataContainer::hasInputDataPrefix(name)) {continue;}
		cellDataExists = true;
	}
	if (cellDataExists) {
		m_types.push_back(PostStringResultArgument::Type::GridCell);
		ui->positionComboBox->addItem(tr("Cell"));
	}
	if (container->edgeIScalarValueExists()) {
		m_types.push_back(PostStringResultArgument::Type::GridEdgeI);
		ui->positionComboBox->addItem(tr("EdgeI"));
	}
	if (container->edgeJScalarValueExists()) {
		m_types.push_back(PostStringResultArgument::Type::GridEdgeJ);
		ui->positionComboBox->addItem(tr("EdgeJ"));
	}

	handlePositionChange();
}

PostStringResultArgument* PostStringResultArgumentEditDialog::argument()
{
	return m_argument;
}

void PostStringResultArgumentEditDialog::setArgument(PostStringResultArgument* arg)
{
	m_argument->copyValue(*arg);
	updateWidgets();
}

void PostStringResultArgumentEditDialog::accept()
{
	applySetting();
	if (m_argument->refName().size() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("Variable name is empty."));
		return;
	}
	QDialog::accept();
}

void PostStringResultArgumentEditDialog::reject()
{
	m_rejected = true;
	QDialog::reject();
}

void PostStringResultArgumentEditDialog::handlePositionChange()
{
	int idx = ui->positionComboBox->currentIndex();
	auto type = m_types.at(idx);

	ui->resultNameComboBox->clear();
	auto data = m_zoneDataContainer->data()->data();
	auto si = m_zoneDataContainer->solutionInfo();
	auto st = vtkStructuredGrid::SafeDownCast(data);
	auto ust = vtkStructuredGrid::SafeDownCast(data);

	int dim[3];
	if (st != nullptr) {
		st->GetDimensions(dim);
	}

	if (type == PostStringResultArgument::Type::BaseIterative) {
		ui->resultNameComboBox->clear();
		for (auto& pair : si->baseIterativeNumericalResults()) {
			ui->resultNameComboBox->addItem(pair.first.c_str());
		}
		for (auto& pair : si->baseIterativeStringResults()) {
			ui->resultNameComboBox->addItem(pair.first.c_str());
		}
		disableSpinBox(ui->iSpinBox);
		disableSpinBox(ui->jSpinBox);
		disableSpinBox(ui->kSpinBox);
		disableSpinBox(ui->indexSpinBox);
	} else if (type == PostStringResultArgument::Type::GridNode) {
		auto pd = data->GetPointData();
		setupComboBoxWithNames(pd, ui->resultNameComboBox);
		if (st != nullptr) {
			// dim[0] > 1 always.
			ui->iSpinBox->setMaximum(dim[0]);
			ui->iSpinBox->setEnabled(true);
			if (dim[1] > 1) {
				ui->jSpinBox->setMaximum(dim[1]);
				ui->jSpinBox->setEnabled(true);
			} else {
				disableSpinBox(ui->jSpinBox);
			}
			if (dim[2] > 1) {
				ui->kSpinBox->setMaximum(dim[2]);
				ui->kSpinBox->setEnabled(true);
			} else {
				disableSpinBox(ui->kSpinBox);
			}
			disableSpinBox(ui->indexSpinBox);
		} else if (ust != nullptr) {
			ui->indexSpinBox->setMaximum(ust->GetNumberOfPoints());
			ui->indexSpinBox->setEnabled(true);
			disableSpinBox(ui->iSpinBox);
			disableSpinBox(ui->jSpinBox);
			disableSpinBox(ui->kSpinBox);
		}
	} else if (type == PostStringResultArgument::Type::GridCell) {
		auto cd = data->GetCellData();
		setupComboBoxWithNames(cd, ui->resultNameComboBox);
		if (st != nullptr) {
			// dim[0] > 2 always.
			ui->iSpinBox->setMaximum(dim[0] - 1);
			ui->iSpinBox->setEnabled(true);
			if (dim[1] > 1) {
				ui->jSpinBox->setMaximum(dim[1] - 1);
				ui->jSpinBox->setEnabled(true);
			} else {
				disableSpinBox(ui->jSpinBox);
			}
			if (dim[2] > 1) {
				ui->kSpinBox->setMaximum(dim[2] - 1);
				ui->kSpinBox->setEnabled(true);
			} else {
				disableSpinBox(ui->kSpinBox);
			}
			disableSpinBox(ui->indexSpinBox);
		} else if (ust != nullptr) {
			ui->indexSpinBox->setMaximum(ust->GetNumberOfCells());
			ui->indexSpinBox->setEnabled(true);
			disableSpinBox(ui->iSpinBox);
			disableSpinBox(ui->jSpinBox);
			disableSpinBox(ui->kSpinBox);
		}
	} else if (type == PostStringResultArgument::Type::GridEdgeI) {
		auto ed = m_zoneDataContainer->iFaceData()->data()->GetPointData();
		setupComboBoxWithNames(ed, ui->resultNameComboBox);
		if (st != nullptr) {
			ui->iSpinBox->setMaximum(dim[0]);
			ui->iSpinBox->setEnabled(true);
			if (dim[1] > 1) {
				ui->jSpinBox->setMaximum(dim[1] - 1);
				ui->jSpinBox->setEnabled(true);
			} else {
				disableSpinBox(ui->jSpinBox);
			}
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		}
	} else if (type == PostStringResultArgument::Type::GridEdgeJ) {
		auto ed = m_zoneDataContainer->jFaceData()->data()->GetPointData();
		setupComboBoxWithNames(ed, ui->resultNameComboBox);
		if (st != nullptr) {
			if (dim[0] > 1) {
				ui->iSpinBox->setMaximum(dim[0] - 1);
				ui->iSpinBox->setEnabled(true);
			} else {
				disableSpinBox(ui->iSpinBox);
			}
			ui->jSpinBox->setMaximum(dim[1]);
			ui->jSpinBox->setEnabled(true);
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		}
	}
}

void PostStringResultArgumentEditDialog::handleResultChange()
{
	auto txt = ui->resultNameComboBox->currentText();
	ui->vNameEdit->setText(txt.left(1));
}

void PostStringResultArgumentEditDialog::updateWidgets()
{
	auto it = std::find(m_types.begin(), m_types.end(), m_argument->type());
	if (it != m_types.end()) {
		ui->positionComboBox->setCurrentIndex(it - m_types.begin());
	}
	ui->resultNameComboBox->setCurrentText(m_argument->name().c_str());
	ui->vNameEdit->setText(m_argument->refName().c_str());
	ui->iSpinBox->setValue(m_argument->i() + 1);
	ui->jSpinBox->setValue(m_argument->j() + 1);
	ui->kSpinBox->setValue(m_argument->k() + 1);
	ui->indexSpinBox->setValue(m_argument->index() + 1);
	ui->valueForTestEdit->setText(m_argument->valueForTest());
}

void PostStringResultArgumentEditDialog::applySetting()
{
	int posIdx = ui->positionComboBox->currentIndex();
	auto type = m_types.at(posIdx);
	m_argument->setType(type);
	m_argument->setName(iRIC::toStr(ui->resultNameComboBox->currentText()));
	m_argument->setRefName(iRIC::toStr(ui->vNameEdit->text()));
	m_argument->setI(ui->iSpinBox->value() - 1);
	m_argument->setJ(ui->jSpinBox->value() - 1);
	m_argument->setK(ui->kSpinBox->value() - 1);
	m_argument->setIndex(ui->indexSpinBox->value() - 1);
	m_argument->setValueForTest(ui->valueForTestEdit->text());
}
