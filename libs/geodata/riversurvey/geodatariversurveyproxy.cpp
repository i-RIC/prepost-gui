#include "geodatariversurvey.h"
#include "geodatariversurveydisplaysettingdialog.h"
#include "geodatariversurveyproxy.h"
#include "private/geodatariversurvey_impl.h"
#include "private/geodatariversurveyproxy_setsettingcommand.h"

#include <misc/mathsupport.h>
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
	GeoDataProxy(geodata)
{}

GeoDataRiverSurveyProxy::~GeoDataRiverSurveyProxy()
{
	auto r = renderer();

	r->RemoveActor(m_centerAndBankLinesActor);
	r->RemoveActor(m_crossectionsActor);
	r->RemoveActor(m_backgroundActor);
	r->RemoveActor(m_crosssectionLinesActor);

	r->RemoveActor2D(m_labelActor);
}

void GeoDataRiverSurveyProxy::setupActors()
{
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(geoData());
	vtkRenderer* r = renderer();
	vtkActorCollection* col = actorCollection();

	m_centerAndBankLinesActor = vtkSmartPointer<vtkActor>::New();
	m_centerAndBankLinesActor->SetMapper(rs->impl->m_centerAndBankLinesActor->GetMapper());
	m_centerAndBankLinesActor->SetProperty(rs->impl->m_centerAndBankLinesActor->GetProperty());
	r->AddActor(m_centerAndBankLinesActor);
	col->AddItem(m_centerAndBankLinesActor);

	m_crossectionsActor = vtkSmartPointer<vtkActor>::New();
	m_crossectionsActor->SetMapper(rs->impl->m_crossSectionLinesActor->GetMapper());
	m_crossectionsActor->SetProperty(rs->impl->m_crossSectionLinesActor->GetProperty());
	r->AddActor(m_crossectionsActor);
	col->AddItem(m_crossectionsActor);

	m_backgroundActor = vtkSmartPointer<vtkActor>::New();
	m_backgroundActor->SetMapper(rs->impl->m_backgroundActor->GetMapper());
	r->AddActor(m_backgroundActor);
	col->AddItem(m_backgroundActor);

	m_crosssectionLinesActor = vtkSmartPointer<vtkActor>::New();

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_crosssectionLines = vtkSmartPointer<vtkUnstructuredGrid>::New();
	mapper->SetInputData(m_crosssectionLines);
	m_crosssectionLinesActor->SetMapper(mapper);
	m_crosssectionLinesActor->GetProperty()->SetLineWidth(1);

	r->AddActor(m_crosssectionLinesActor);
	col->AddItem(m_crosssectionLinesActor);

	m_labelActor = vtkSmartPointer<vtkActor2D>::New();
	m_labelActor->SetMapper(rs->impl->m_labelMapper);
	r->AddActor2D(m_labelActor);
	actor2DCollection()->AddItem(m_labelActor);

	updateGraphics();
}

void GeoDataRiverSurveyProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	updateGraphics();
}

void GeoDataRiverSurveyProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void GeoDataRiverSurveyProxy::updateGraphics()
{
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(geoData());

	m_crosssectionLines->Reset();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	GeoDataRiverPathPoint* p = rs->m_headPoint->nextPoint();
	vtkIdType pointNum = 0;
	while (p != 0) {
		double maxHeight = 0;
		GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		// calculate maxHeight.
		for (int i = 0; i < alist.size(); ++i) {
			GeoDataRiverCrosssection::Altitude alt = alist[i];
			if (i == 0 || maxHeight < alt.height()) {maxHeight = alt.height();}
		}
		// now draw lines.
		QPointF offsetDir = p->crosssectionDirection();
		iRIC::rotateVector270(offsetDir);

		double offset;
		GeoDataRiverCrosssection::Altitude alt = alist[0];
		offset = (maxHeight - alt.height()) * m_setting.crosssectionLinesScale;
		QPointF tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
		points->InsertNextPoint(tmpp.x(), tmpp.y(), 0);
		++ pointNum;
		for (int i = 1; i < alist.size(); ++i) {
			GeoDataRiverCrosssection::Altitude alt = alist[i];
			offset = (maxHeight - alt.height()) * m_setting.crosssectionLinesScale;
			QPointF tmpp = p->crosssectionPosition(alt.position()) + offsetDir * offset;
			points->InsertNextPoint(tmpp.x(), tmpp.y(), 0);
			++ pointNum;
			vtkSmartPointer<vtkLine> tmpline = vtkSmartPointer<vtkLine>::New();
			tmpline->GetPointIds()->SetId(0, pointNum - 2);
			tmpline->GetPointIds()->SetId(1, pointNum - 1);
			m_crosssectionLines->InsertNextCell(tmpline->GetCellType(), tmpline->GetPointIds());
		}
		p = p->nextPoint();
	}
	m_crosssectionLines->SetPoints(points);
	points->Modified();
	m_crosssectionLines->Modified();
	m_crosssectionLines->BuildLinks();

	m_backgroundActor->VisibilityOff();
	m_backgroundActor->GetProperty()->SetOpacity(m_setting.opacity);
	m_crosssectionLinesActor->VisibilityOff();
	m_crosssectionLinesActor->GetProperty()->SetColor(m_setting.crosssectionLinesColor);

	vtkActorCollection* collection = actorCollection();
	collection->RemoveItem(m_backgroundActor);
	collection->RemoveItem(m_crosssectionLinesActor);

	if (m_setting.showBackground) {
		collection->AddItem(m_backgroundActor);
	}
	if (m_setting.showLines) {
		collection->AddItem(m_crosssectionLinesActor);
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

	m_crossectionsActor->SetPosition(0, 0, lines);
	m_centerAndBankLinesActor->SetPosition(0, 0, lines);
	m_crosssectionLinesActor->SetPosition(0, 0, backlines);
	m_backgroundActor->SetPosition(0, 0, background);
}

QDialog* GeoDataRiverSurveyProxy::propertyDialog(QWidget* parent)
{
	GeoDataRiverSurveyDisplaySettingDialog* dialog = new GeoDataRiverSurveyDisplaySettingDialog(parent);
	dialog->setSetting(m_setting);

	return dialog;
}

void GeoDataRiverSurveyProxy::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GeoDataRiverSurveyDisplaySettingDialog* dialog = dynamic_cast<GeoDataRiverSurveyDisplaySettingDialog*>(propDialog);

	pushRenderCommand(new SetSettingCommand(dialog->setting(), this));
}
