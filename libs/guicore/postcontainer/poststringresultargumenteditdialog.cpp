#include "../grid/v4structured2dgrid.h"
#include "../grid/v4structured3dgrid.h"
#include "../grid/v4unstructured2dgrid.h"
#include "postsolutioninfo.h"
#include "poststringresultargument.h"
#include "poststringresultargumenteditdialog.h"
#include "v4postzonedatacontainer.h"
#include "v4solutiongrid.h"
#include "ui_poststringresultargumenteditdialog.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>
#include <misc/stringtool.h>

#include <QMessageBox>

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

void PostStringResultArgumentEditDialog::setZoneDataContainer(v4PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;

	auto s = container->solutionInfo();
	if (s->baseIterativeNumericalResults().size() > 0 || s->baseIterativeStringResults().size() > 0) {
		m_types.push_back(PostStringResultArgument::Type::BaseIterative);
		ui->positionComboBox->addItem(tr("Global"));
	}

	if (container->gridData()->grid()->vtkData()->data()->GetPointData()->GetNumberOfArrays() > 0) {
		m_types.push_back(PostStringResultArgument::Type::GridNode);
		ui->positionComboBox->addItem(tr("Node"));
	}
	bool cellDataExists = false;
	auto cd = container->gridData()->grid()->vtkData()->data()->GetCellData();
	for (int i = 0; i < cd->GetNumberOfArrays(); ++i) {
		cellDataExists = true;
	}
	if (cellDataExists) {
		m_types.push_back(PostStringResultArgument::Type::GridCell);
		ui->positionComboBox->addItem(tr("Cell"));
	}
	if (container->gridData()->scalarValueExists(v4SolutionGrid::Position::IFace)) {
		m_types.push_back(PostStringResultArgument::Type::GridEdgeI);
		ui->positionComboBox->addItem(tr("EdgeI"));
	}
	if (container->gridData()->scalarValueExists(v4SolutionGrid::Position::JFace)) {
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
	auto grid = m_zoneDataContainer->gridData()->grid();
	auto si = m_zoneDataContainer->solutionInfo();
	auto uGrid = dynamic_cast<v4Unstructured2dGrid*> (grid);
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (grid);

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
		auto pd = grid->vtkData()->data()->GetPointData();
		setupComboBoxWithNames(pd, ui->resultNameComboBox);

		if (uGrid != nullptr) {
			ui->indexSpinBox->setMaximum(uGrid->nodeCount());
			ui->indexSpinBox->setEnabled(true);
			disableSpinBox(ui->iSpinBox);
			disableSpinBox(ui->jSpinBox);
			disableSpinBox(ui->kSpinBox);
		} else if (sGrid2d != nullptr) {
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid2d->dimensionI());
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid2d->dimensionJ());
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		} else if (sGrid3d != nullptr) {
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid3d->dimensionI());
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid3d->dimensionJ());
			ui->kSpinBox->setEnabled(true);
			ui->kSpinBox->setMaximum(sGrid3d->dimensionK());
			disableSpinBox(ui->indexSpinBox);
		}
	} else if (type == PostStringResultArgument::Type::GridCell) {
		auto cd = grid->vtkData()->data()->GetCellData();
		setupComboBoxWithNames(cd, ui->resultNameComboBox);
		if (uGrid != nullptr) {
			ui->indexSpinBox->setEnabled(true);
			ui->indexSpinBox->setMaximum(uGrid->cellCount());
			disableSpinBox(ui->iSpinBox);
			disableSpinBox(ui->jSpinBox);
			disableSpinBox(ui->kSpinBox);
		} else if (sGrid2d != nullptr) {
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid2d->dimensionI() - 1);
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid2d->dimensionJ() - 1);
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		} else if (sGrid3d != nullptr) {
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid3d->dimensionI() - 1);
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid3d->dimensionJ() - 1);
			ui->kSpinBox->setEnabled(true);
			ui->kSpinBox->setMaximum(sGrid3d->dimensionK() - 1);
			disableSpinBox(ui->indexSpinBox);
		}
	} else if (type == PostStringResultArgument::Type::GridEdgeI) {
		if (sGrid2d != nullptr) {
			setupComboBoxWithNames(sGrid2d->vtkIEdgeData()->data()->GetCellData(), ui->resultNameComboBox);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid2d->dimensionI());
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid2d->dimensionJ() - 1);
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		} else if (sGrid3d != nullptr) {
			setupComboBoxWithNames(sGrid3d->vtkIFaceData()->data()->GetCellData(), ui->resultNameComboBox);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid3d->dimensionI());
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid3d->dimensionJ() - 1);
			ui->kSpinBox->setEnabled(true);
			ui->kSpinBox->setMaximum(sGrid3d->dimensionK() - 1);
			disableSpinBox(ui->indexSpinBox);
		}
	} else if (type == PostStringResultArgument::Type::GridEdgeJ) {
		if (sGrid2d != nullptr) {
			setupComboBoxWithNames(sGrid2d->vtkJEdgeData()->data()->GetCellData(), ui->resultNameComboBox);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid2d->dimensionI() - 1);
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid2d->dimensionJ());
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		} else if (sGrid3d != nullptr) {
			setupComboBoxWithNames(sGrid3d->vtkJFaceData()->data()->GetCellData(), ui->resultNameComboBox);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid3d->dimensionI() - 1);
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid3d->dimensionJ());
			ui->kSpinBox->setEnabled(true);
			ui->kSpinBox->setMaximum(sGrid3d->dimensionK() - 1);
			disableSpinBox(ui->indexSpinBox);
		}
	} else if (type == PostStringResultArgument::Type::GridEdgeK) {
		if (sGrid3d != nullptr) {
			setupComboBoxWithNames(sGrid3d->vtkJFaceData()->data()->GetCellData(), ui->resultNameComboBox);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid3d->dimensionI() - 1);
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid3d->dimensionJ() - 1);
			ui->kSpinBox->setEnabled(true);
			ui->kSpinBox->setMaximum(sGrid3d->dimensionK());
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
