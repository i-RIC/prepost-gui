#include "poststringresultargumentsettingeditdialog.h"
#include "ui_poststringresultargumentsettingeditdialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>
#include <guicore/grid/public/v4grid_attributedataprovider.h>
#include <guicore/grid/v4grid.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionoutput.h>

#include <QMessageBox>

namespace {

void setupComboBoxWithNames(vtkDataSetAttributes* att, v4Grid::AttributeDataProvider* provider, QComboBox* comboBox, std::vector<std::string>* names)
{
	names->clear();
	comboBox->clear();

	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(att)) {
		names->push_back(name);
		comboBox->addItem(provider->caption(name));
	}
}

void disableSpinBox(QSpinBox* box)
{
	box->setMaximum(1);
	box->setDisabled(true);
}

} // namespace

PostStringResultArgumentSettingEditDialog::PostStringResultArgumentSettingEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PostStringResultArgumentSettingEditDialog)
{
	ui->setupUi(this);

	connect<void (QComboBox::*)(int)>(ui->positionComboBox, &QComboBox::currentIndexChanged, this, &PostStringResultArgumentSettingEditDialog::handlePositionChange);
	connect<void (QComboBox::*)(int)>(ui->resultNameComboBox, &QComboBox::currentIndexChanged, this, &PostStringResultArgumentSettingEditDialog::handleResultChange);
}

PostStringResultArgumentSettingEditDialog::~PostStringResultArgumentSettingEditDialog()
{
	delete ui;
}

void PostStringResultArgumentSettingEditDialog::setZoneDataContainer(v4PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;

	auto s = container->solutionInfo();
	if (s->baseIterativeNumericalResults().size() > 0 || s->baseIterativeStringResults().size() > 0) {
		m_types.push_back(PostStringResultArgumentContainer::Type::BaseIterative);
		ui->positionComboBox->addItem(tr("Global"));
	}

	if (container->gridData()->grid()->vtkData()->pointScalarValueExists()) {
		m_types.push_back(PostStringResultArgumentContainer::Type::GridNode);
		ui->positionComboBox->addItem(tr("Node"));
	}
	if (container->gridData()->grid()->vtkData()->cellScalarValueExists()) {
		m_types.push_back(PostStringResultArgumentContainer::Type::GridCell);
		ui->positionComboBox->addItem(tr("Cell"));
	}

	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (container->gridData()->grid());
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (container->gridData()->grid());

	if (sGrid2d != nullptr) {
		if (sGrid2d->vtkIEdgeData()->cellScalarValueExists()) {
			m_types.push_back(PostStringResultArgumentContainer::Type::GridIEdge);
			ui->positionComboBox->addItem(tr("EdgeI"));
		}
		if (sGrid2d->vtkJEdgeData()->cellScalarValueExists()) {
			m_types.push_back(PostStringResultArgumentContainer::Type::GridJEdge);
			ui->positionComboBox->addItem(tr("EdgeJ"));
		}
	} else if (sGrid3d != nullptr) {
		if (sGrid3d->vtkIFaceData()->cellScalarValueExists()) {
			m_types.push_back(PostStringResultArgumentContainer::Type::GridIEdge);
			ui->positionComboBox->addItem(tr("EdgeI"));
		}
		if (sGrid3d->vtkJFaceData()->cellScalarValueExists()) {
			m_types.push_back(PostStringResultArgumentContainer::Type::GridJEdge);
			ui->positionComboBox->addItem(tr("EdgeJ"));
		}
		if (sGrid3d->vtkKFaceData()->cellScalarValueExists()) {
			m_types.push_back(PostStringResultArgumentContainer::Type::GridKEdge);
			ui->positionComboBox->addItem(tr("EdgeK"));
		}
	}

	handlePositionChange();
}

PostStringResultArgumentContainer PostStringResultArgumentSettingEditDialog::setting() const
{
	PostStringResultArgumentContainer ret;

	ret.type = m_types.at(ui->positionComboBox->currentIndex());
	ret.name = ui->resultNameComboBox->currentText(); // FIX THIS!!!
	ret.refName = ui->vNameEdit->text();
	ret.i = ui->iSpinBox->value() - 1;
	ret.j = ui->jSpinBox->value() - 1;
	ret.k = ui->kSpinBox->value() - 1;
	ret.index = ui->indexSpinBox->value() - 1;
	ret.valueForTest = ui->valueForTestEdit->text();

	return ret;
}


void PostStringResultArgumentSettingEditDialog::setSetting(const PostStringResultArgumentContainer& setting)
{
	auto it = std::find(m_types.begin(), m_types.end(), setting.type);
	if (it != m_types.end()) {
		ui->positionComboBox->setCurrentIndex(it - m_types.begin());
	}
	ui->resultNameComboBox->setCurrentText(setting.name);
	ui->vNameEdit->setText(setting.refName);
	ui->iSpinBox->setValue(setting.i + 1);
	ui->jSpinBox->setValue(setting.j + 1);
	ui->kSpinBox->setValue(setting.k + 1);
	ui->indexSpinBox->setValue(setting.index + 1);
	ui->valueForTestEdit->setText(setting.valueForTest);
}

void PostStringResultArgumentSettingEditDialog::accept()
{
	auto s = setting();
	if (s.refName.value().length() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("Variable name is empty."));
		return;
	}

	QDialog::accept();
}

void PostStringResultArgumentSettingEditDialog::handlePositionChange()
{
	int idx = ui->positionComboBox->currentIndex();
	auto type = m_types.at(idx);

	ui->resultNameComboBox->clear();

	auto grid = m_zoneDataContainer->gridData()->grid();
	auto si = m_zoneDataContainer->solutionInfo();
	auto gridType = m_zoneDataContainer->gridType();
	auto def = gridType->solverDefinition();
	auto provider = grid->attributeDataProvider();

	auto uGrid = dynamic_cast<v4Unstructured2dGrid*> (grid);
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (grid);

	m_valueNames.clear();

	if (type == PostStringResultArgumentContainer::Type::BaseIterative) {
		ui->resultNameComboBox->clear();
		for (auto& pair : si->baseIterativeNumericalResults()) {
			m_valueNames.push_back(pair.first);
			ui->resultNameComboBox->addItem(def->globalOutput(pair.first)->caption());
		}
		disableSpinBox(ui->iSpinBox);
		disableSpinBox(ui->jSpinBox);
		disableSpinBox(ui->kSpinBox);
		disableSpinBox(ui->indexSpinBox);
	} else if (type == PostStringResultArgumentContainer::Type::GridNode) {
		auto pd = grid->vtkData()->data()->GetPointData();
		setupComboBoxWithNames(pd, provider, ui->resultNameComboBox, &m_valueNames);
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
	} else if (type == PostStringResultArgumentContainer::Type::GridCell) {
		auto cd = grid->vtkData()->data()->GetCellData();
		setupComboBoxWithNames(cd, provider, ui->resultNameComboBox, &m_valueNames);
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
	} else if (type == PostStringResultArgumentContainer::Type::GridIEdge) {
		if (sGrid2d != nullptr) {
			setupComboBoxWithNames(sGrid2d->vtkIEdgeData()->data()->GetCellData(), provider, ui->resultNameComboBox, &m_valueNames);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid2d->dimensionI());
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid2d->dimensionJ() - 1);
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		} else if (sGrid3d != nullptr) {
			setupComboBoxWithNames(sGrid3d->vtkIFaceData()->data()->GetCellData(), provider, ui->resultNameComboBox, &m_valueNames);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid3d->dimensionI());
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid3d->dimensionJ() - 1);
			ui->kSpinBox->setEnabled(true);
			ui->kSpinBox->setMaximum(sGrid3d->dimensionK() - 1);
			disableSpinBox(ui->indexSpinBox);
		}
	} else if (type == PostStringResultArgumentContainer::Type::GridJEdge) {
		if (sGrid2d != nullptr) {
			setupComboBoxWithNames(sGrid2d->vtkJEdgeData()->data()->GetCellData(), provider, ui->resultNameComboBox, &m_valueNames);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid2d->dimensionI() - 1);
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid2d->dimensionJ());
			disableSpinBox(ui->kSpinBox);
			disableSpinBox(ui->indexSpinBox);
		} else if (sGrid3d != nullptr) {
			setupComboBoxWithNames(sGrid3d->vtkJFaceData()->data()->GetCellData(), provider, ui->resultNameComboBox, &m_valueNames);
			ui->iSpinBox->setEnabled(true);
			ui->iSpinBox->setMaximum(sGrid3d->dimensionI() - 1);
			ui->jSpinBox->setEnabled(true);
			ui->jSpinBox->setMaximum(sGrid3d->dimensionJ());
			ui->kSpinBox->setEnabled(true);
			ui->kSpinBox->setMaximum(sGrid3d->dimensionK() - 1);
			disableSpinBox(ui->indexSpinBox);
		}
	} else if (type == PostStringResultArgumentContainer::Type::GridKEdge) {
		if (sGrid3d != nullptr) {
			setupComboBoxWithNames(sGrid3d->vtkJFaceData()->data()->GetCellData(), provider, ui->resultNameComboBox, &m_valueNames);
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

void PostStringResultArgumentSettingEditDialog::handleResultChange()
{
	auto index = ui->resultNameComboBox->currentIndex();
	if (index < 0) {return;}

	auto name = m_valueNames.at(index);
	ui->vNameEdit->setText(name.substr(0, 1).c_str());
}
