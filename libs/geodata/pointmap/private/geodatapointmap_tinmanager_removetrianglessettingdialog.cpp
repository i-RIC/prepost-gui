#include "geodatapointmap_tinmanager_impl.h"
#include "geodatapointmap_tinmanager_rebuildtinfrompointscommand.h"
#include "geodatapointmap_tinmanager_removetrianglessetting.h"
#include "geodatapointmap_tinmanager_removetrianglessettingdialog.h"
#include "geodatapointmap_tinmanager_triangleswithlongedgeremover.h"
#include "ui_geodatapointmap_tinmanager_removetrianglessettingdialog.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/pre/geodata/private/geodata_updateactorsettingcommand.h>
#include <misc/valuemodifycommandt.h>
#include <misc/valuemodifycommandt.h>

#include <vtkActor.h>

GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog::RemoveTrianglesSettingDialog(TINManager* tinManager, QWidget *parent) :
	QDialog(parent),
	m_tin {nullptr},
	m_tinActor {vtkActor::New()},
	m_tinManager {tinManager},
	ui(new Ui::GeoDataPointmap_TINManager_RemoveTrianglesSettingDialog)
{
	ui->setupUi(this);
	connect(ui->previewButton, &QPushButton::clicked, this, &RemoveTrianglesSettingDialog::preview);

	m_tin = tinManager->buildTinFromPoints();

	setupTinActor();

	auto& setting = tinManager->impl->m_removeTrianglesSetting;
	if (setting.thresholdLength == 0) {
		setting.thresholdLength = TrianglesWithLongEdgeRemover::thresholdLength(m_tin, 0.999);
	}

	setSetting(setting);
}

GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog::~RemoveTrianglesSettingDialog()
{
	auto geoData = m_tinManager->impl->m_parent;
	geoData->renderer()->RemoveActor(m_tinActor);

	if (m_tin != nullptr) {
		m_tin->Delete();
	}
	delete ui;
}

void GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog::accept()
{
	hide();

	auto com1 = new ValueModifyCommmand<RemoveTrianglesSetting> ("RemoveTriangles", setting(), &m_tinManager->impl->m_removeTrianglesSetting);
	auto com2 = new RebuildTinFromPointsCommand(com1, m_tinManager);
	auto com3 = new UpdateActorSettingCommand(com2, m_tinManager->impl->m_parent);

	m_tinManager->impl->m_parent->pushRenderCommand(com3);
	QDialog::accept();
}

void GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog::preview()
{
	auto s = setting();
	vtkPolyData* data = nullptr;
	if (s.enabled) {
		auto cells = TrianglesWithLongEdgeRemover::buildCellArray(m_tin, s.thresholdLength, false, true);
		data = vtkPolyData::New();
		data->SetPoints(m_tin->GetPoints());
		data->SetPolys(cells);
	} else {
		data = m_tin;
		data->Register(nullptr);
	}

	auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
	mapper->SetInputData(data);
	m_tinActor->SetMapper(mapper);
	m_tinActor->VisibilityOn();

	auto geoData = m_tinManager->impl->m_parent;
	geoData->renderGraphicsView();
}

GeoDataPointmap::TINManager::RemoveTrianglesSetting GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog::setting() const
{
	RemoveTrianglesSetting s;

	s.enabled = ui->enableCheckBox->isChecked();
	s.thresholdLength = ui->thresholdSpinBox->value();

	return s;
}

void GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog::setSetting(const RemoveTrianglesSetting& setting)
{
	ui->enableCheckBox->setChecked(setting.enabled);
	ui->thresholdSpinBox->setValue(setting.thresholdLength);
}

void GeoDataPointmap::TINManager::RemoveTrianglesSettingDialog::setupTinActor()
{
	auto prop = m_tinActor->GetProperty();
	prop->SetColor(0.3, 0.3, 0.3);
	prop->SetOpacity(0.75);

	m_tinActor->VisibilityOff();

	double position[3];
	m_tinManager->tinActor()->GetPosition(position);
	position[2] += 0.1;
	m_tinActor->SetPosition(position);

	auto geoData = m_tinManager->impl->m_parent;
	geoData->renderer()->AddActor(m_tinActor);
}
