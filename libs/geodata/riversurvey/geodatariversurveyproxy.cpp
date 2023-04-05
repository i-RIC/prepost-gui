#include "geodatariversurvey.h"
#include "geodatariversurveydisplaysettingdialog.h"
#include "geodatariversurveyproxy.h"
#include "private/geodatariversurvey_impl.h"
#include "private/geodatariversurveyproxy_impl.h"
#include "private/geodatariversurveyproxy_displaysettingwidget.h"

#include <guicore/post/post2d/base/post2dwindowgridtypedataiteminterface.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <misc/mathsupport.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

#include <QMainWindow>
#include <QVector2D>

#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkDataSetMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

GeoDataRiverSurveyProxy::GeoDataRiverSurveyProxy(GeoDataRiverSurvey* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataRiverSurveyProxy::~GeoDataRiverSurveyProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_centerAndBankLinesActor);
	r->RemoveActor(impl->m_crossectionsActor);
	r->RemoveActor(impl->m_backgroundActor);
	r->RemoveActor(impl->m_crosssectionLinesActor);

	r->RemoveActor2D(impl->m_labelActor);
}

void GeoDataRiverSurveyProxy::setupActors()
{
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(geoData());
	vtkRenderer* r = renderer();
	vtkActorCollection* col = actorCollection();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(rs->centerAndBankLines());
	impl->m_centerAndBankLinesActor->SetMapper(mapper);
	impl->m_centerAndBankLinesActor->SetProperty(rs->impl->m_centerAndBankLinesActor->GetProperty());
	r->AddActor(impl->m_centerAndBankLinesActor);
	col->AddItem(impl->m_centerAndBankLinesActor);

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(rs->crossSectionLines());
	impl->m_crossectionsActor->SetMapper(mapper);
	impl->m_crossectionsActor->SetProperty(rs->impl->m_crossSectionLinesActor->GetProperty());
	r->AddActor(impl->m_crossectionsActor);
	col->AddItem(impl->m_crossectionsActor);

	r->AddActor(impl->m_backgroundActor);

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(impl->m_crosssectionLines);
	impl->m_crosssectionLinesActor->SetMapper(mapper);
	impl->m_crosssectionLinesActor->GetProperty()->SetLineWidth(1);
	r->AddActor(impl->m_crosssectionLinesActor);

	impl->m_labelMapper->SetInputData(rs->rightBankPointSet());
	impl->m_labelMapper->SetFieldDataName(rs->labelArray()->GetName());
	GeoDataRiverSurvey::setupLabelMapper(impl->m_labelMapper);
	impl->m_labelActor->SetMapper(impl->m_labelMapper);
	r->AddActor2D(impl->m_labelActor);

	updateActorSetting();
}

void GeoDataRiverSurveyProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataRiverSurveyProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}

void GeoDataRiverSurveyProxy::updateActorSetting()
{
	auto rs = dynamic_cast<GeoDataRiverSurvey*>(geoData());
	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = rs->impl->m_displaySetting;
	}

	impl->m_crosssectionLines->Initialize();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto verticalLines = vtkSmartPointer<vtkCellArray>::New();

	GeoDataRiverPathPoint* p = rs->m_headPoint->nextPoint();
	while (p != 0) {
		double maxHeight = 0;
		const auto& alist = p->crosssection().AltitudeInfo();
		for (int i = 0; i < alist.size(); ++i) {
			GeoDataRiverCrosssection::Altitude alt = alist[i];
			if (i == 0 || maxHeight < alt.height()) {maxHeight = alt.height();}
		}
		QPointF offsetDir = p->crosssectionDirection();
		offsetDir = iRIC::rotateVector270(offsetDir);

		vtkIdType firstId = points->GetNumberOfPoints();
		std::vector<vtkIdType> ids;
		ids.reserve(alist.size());

		for (int i = 0; i < alist.size(); ++i) {
			const auto& alt = alist[i];
			double offset = (maxHeight - alt.height()) * ds.crosssectionLinesScale;
			QPointF tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
			points->InsertNextPoint(tmpp.x(), tmpp.y(), 0);
			ids.push_back(firstId + i);
		}
		verticalLines->InsertNextCell(ids.size(), ids.data());
		p = p->nextPoint();
	}
	impl->m_crosssectionLines->SetPoints(points);
	impl->m_crosssectionLines->SetLines(verticalLines);

	auto col = actorCollection();
	auto col2 = actor2DCollection();

	impl->m_crosssectionLinesActor->VisibilityOff();
	col->RemoveItem(impl->m_crosssectionLinesActor);

	if (ds.showLines) {
		col->AddItem(impl->m_crosssectionLinesActor);
		impl->m_crosssectionLinesActor->GetProperty()->SetColor(ds.crosssectionLinesColor);
	}

	impl->m_labelActor->VisibilityOff();
	col2->RemoveItem(impl->m_labelActor);

	if (ds.showNames) {
		col2->AddItem(impl->m_labelActor);
		ds.namesTextSetting.applySetting(impl->m_labelMapper->GetLabelTextProperty());
	}

	impl->m_backgroundActor->VisibilityOff();
	col->RemoveItem(impl->m_backgroundActor);
	if (ds.showBackground) {
		col->AddItem(impl->m_backgroundActor);
		impl->m_backgroundActor->GetProperty()->SetOpacity(ds.opacity);

		auto cs = colorMapSettingContainer();
		auto mapper = cs->buildPointDataMapper(rs->impl->m_backgroundGrid);
		impl->m_backgroundActor->SetMapper(mapper);
		mapper->Delete();
	}

	updateVisibilityWithoutRendering();
}

void GeoDataRiverSurveyProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GeoDataRiverSurveyProxy::assignActorZValues(const ZDepthRange& range)
{
	double background = range.min();
	double backlines = range.min() * 0.5 + range.max() * 0.5;
	double lines = range.max();

	impl->m_crossectionsActor->SetPosition(0, 0, lines);
	impl->m_centerAndBankLinesActor->SetPosition(0, 0, lines);
	impl->m_crosssectionLinesActor->SetPosition(0, 0, backlines);
	impl->m_backgroundActor->SetPosition(0, 0, background);
}

void GeoDataRiverSurveyProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataRiverSurveyProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Cross-Section Data Display Setting"));
	dialog->resize(850, 700);

	return dialog;
}
