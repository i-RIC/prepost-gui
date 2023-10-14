#include "copycamerasettingdialog.h"
#include "ui_copycamerasettingdialog.h"

#include <guicore/datamodel/windowwithvtkgraphicsviewi.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/datamodel/vtk3dgraphicsview.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>

CopyCameraSettingDialog::CopyCameraSettingDialog(QMdiArea* area, QWidget *parent) :
	QDialog(parent),
	m_mdiArea {area},
	ui(new Ui::CopyCameraSettingDialog)
{
	ui->setupUi(this);
	setupSources();

	if (m_sources.size() > 0) {
		auto s = m_sources.at(0);
		setupTargets(s);
	}
	connect<void (QComboBox::*)(int)>(ui->sourceComboBox, &QComboBox::currentIndexChanged, this, &CopyCameraSettingDialog::updateTargets);
	connect(ui->checkAllButton, &QPushButton::clicked, this, &CopyCameraSettingDialog::checkAll);
	connect(ui->uncheckAllButton, &QPushButton::clicked, this, &CopyCameraSettingDialog::uncheckAll);
}

CopyCameraSettingDialog::~CopyCameraSettingDialog()
{
	delete ui;
}

void CopyCameraSettingDialog::accept()
{
	auto source = m_sources.at(ui->sourceComboBox->currentIndex());
	auto sourceCamera = source.window->getVtkGraphicsView()->mainRenderer()->GetActiveCamera();

	for (int i = 0; i < ui->listWidget->count(); ++i) {
		auto item = ui->listWidget->item(i);
		if (! item->isSelected()) {continue;}

		auto target = m_targets.at(i);
		auto targetCamera = target.window->getVtkGraphicsView()->mainRenderer()->GetActiveCamera();
		targetCamera->DeepCopy(sourceCamera);
		targetCamera->Modified();
		target.window->getVtkGraphicsView()->applyCameraSetting();
	}

	QDialog::accept();
}

void CopyCameraSettingDialog::checkAll()
{
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		ui->listWidget->item(i)->setSelected(true);
	}
}

void CopyCameraSettingDialog::uncheckAll()
{
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		ui->listWidget->item(i)->setSelected(false);
	}
}

void CopyCameraSettingDialog::updateTargets()
{
	auto s = m_sources.at(ui->sourceComboBox->currentIndex());
	setupTargets(s);
}

void CopyCameraSettingDialog::setupSources()
{
	std::vector<Setting> twoDimensionalSettings;
	std::vector<Setting> threeDimensionalSettings;

	for (auto w : m_mdiArea->subWindowList()) {
		auto widget =w->widget();
		auto w2 = dynamic_cast<WindowWithVtkGraphicsViewI*> (widget);
		if (w2 == nullptr) {continue;}

		auto v2 = dynamic_cast<VTK2DGraphicsView*> (w2->getVtkGraphicsView());
		auto v3 = dynamic_cast<VTK3DGraphicsView*> (w2->getVtkGraphicsView());
		Setting s;
		s.name = widget->windowTitle();
		s.window = w2;
		if (v2 != nullptr) {
			s.type = Setting::Type::TwoDimensional;
			twoDimensionalSettings.push_back(s);
		}
		if (v3 != nullptr) {
			s.type = Setting::Type::ThreeDimensional;
			threeDimensionalSettings.push_back(s);
		}
	}
	if (twoDimensionalSettings.size() > 1) {
		for (const auto& s : twoDimensionalSettings) {
			m_sources.push_back(s);
		}
	}
	if (threeDimensionalSettings.size() > 1) {
		for (const auto& s : threeDimensionalSettings) {
			m_sources.push_back(s);
		}
	}
	if (m_sources.size() == 0) {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
	} else {
		for (const auto& s : m_sources) {
			ui->sourceComboBox->addItem(s.name);
		}
	}
}

void CopyCameraSettingDialog::setupTargets(const Setting& setting)
{
	m_targets.clear();
	ui->listWidget->clear();

	for (const auto& s : m_sources) {
		if (setting == s) {continue;}
		if (s.type == setting.type) {
			m_targets.push_back(s);
			ui->listWidget->addItem(s.name);
		}
	}
}
