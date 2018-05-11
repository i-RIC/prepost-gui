#include "../post2dwindowgraphicsview.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"
#include "post2dwindowstreamlineunstructuredsettingdialog.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QMainWindow>
#include <QMouseEvent>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkVertex.h>

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::Setting() :
	CompositeContainer ({&point1, &point2, &pointsSet, &numberOfPoints, &color, &width}),
	point1 {"point1"},
	point2 {"point2"},
	pointsSet {"pointsSet", false},
	numberOfPoints {"numberOfPoints"},
	color {"color"},
	width {"width", 1}
{}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting& Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowNodeVectorStreamlineGroupDataItem(parent),
	m_dialog {nullptr}
{
	setupDefaultValues();
	setupTmpSource();
}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::~Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem()
{
	renderer()->RemoveActor(m_previewActor);
}

QDialog* Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowStreamlineUnstructuredSettingDialog* dialog = new Post2dWindowStreamlineUnstructuredSettingDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setDataItem(this);
	dialog->setZoneData(cont);
	dialog->setActiveAvailable(cont->IBCExists());

	dialog->setSettings(m_setting, m_unstSettings);

	return dialog;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setupDefaultValues()
{
	m_unstSettings.clear();

	Setting s;

	QSettings setting;
	s.color = setting.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();
	s.width = 1;
	s.pointsSet = false;
	s.numberOfPoints = 10;
	m_unstSettings.push_back(s);
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setupTmpSource()
{
	m_previewPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_previewMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_previewMapper ->SetInputData(m_previewPoints);
	m_previewActor = vtkSmartPointer<vtkActor>::New();
	m_previewActor->SetMapper(m_previewMapper);
	vtkProperty* prop = m_previewActor->GetProperty();
	prop->SetRepresentationToPoints();
	prop->SetPointSize(3);
	prop->SetLighting(false);
	prop->SetColor(0, 0, 0);

	m_previewActor->VisibilityOff();
	renderer()->AddActor(m_previewActor);
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setupActors()
{
	for (int i = 0; i < m_sourcePoints.size(); ++i) {
		m_sourcePoints[i]->Delete();
	}
	m_sourcePoints.clear();

	m_streamlineActors.reserve(m_unstSettings.size());

	for (int i = 0; i < m_unstSettings.size(); ++i) {
		Setting& setting = m_unstSettings[i];

		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		QPointF diffVec = setting.point2 - setting.point1;
		QPointF v;
		for (int j = 0; j < setting.numberOfPoints; ++j) {
			double param = j / (double)(setting.numberOfPoints - 1);
			v = setting.point1 + param * diffVec;
			points->InsertNextPoint(v.x(), v.y(), 0);
		}
		vtkUnstructuredGrid* grid = vtkUnstructuredGrid::New();
		grid->SetPoints(points);
		for (int j = 0; j <setting.numberOfPoints; ++j) {
			vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
			vertex->GetPointIds()->SetId(0, j);
			grid->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
		}
		m_sourcePoints.push_back(grid);

		vtkActor* actor = vtkActor::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(setting.color);
		prop->SetLineWidth(setting.width);
		actor->SetScale(1, m_zScale, 1);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkDataSetMapper* mapper = vtkDataSetMapper::New();
		actor->SetMapper(mapper);
		mapper->Delete();

		vtkStreamTracer* tracer = vtkStreamTracer::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(getSource(i));
		tracer->SetInputData(getRegion());
		tracer->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(m_setting.target).c_str());

		mapper->SetInputConnection(tracer->GetOutputPort());
		tracer->Delete();

		m_streamlineActors.push_back(actor);
	}
}

vtkPointSet* Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::getSource(int i)
{
	return m_sourcePoints[i];
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);

	m_unstSettings.clear();
	QDomNode streamlinesNode = iRIC::getChildNode(node, "Streamlines");
	if (! streamlinesNode.isNull()) {
		QDomNodeList streamlines = streamlinesNode.childNodes();
		for (int i = 0; i < streamlines.length(); ++i) {
			Setting s;
			s.load(streamlines.at(i));
			m_unstSettings.push_back(s);
		}
	}
	updateActorSettings();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("Streamlines");
	for (int i = 0; i < m_unstSettings.size(); ++i) {
		Setting& setting = m_unstSettings[i];
		writer.writeStartElement("Streamline");
		setting.save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::showPropertyDialog()
{
	dataModel()->objectBrowserView()->select(m_standardItem->index());
	Post2dWindowStreamlineUnstructuredSettingDialog* propDialog = dynamic_cast<Post2dWindowStreamlineUnstructuredSettingDialog*>(propertyDialog(mainWindow()));
	iricMainWindow()->enterModelessDialogMode();
	connect(dataModel()->graphicsView(), SIGNAL(worldPositionChanged(QVector2D)), propDialog, SLOT(updateMousePosition(QVector2D)));
	connect(propDialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(propDialog, SIGNAL(destroyed()), this, SLOT(exitDialogMode()));
	m_dialog = propDialog;
	propDialog->show();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setSetting(const QPointF &v1, const QPointF &v2, int num)
{
	m_point1 = v1;
	m_point2 = v2;
	m_numberOfPoints = num;
	m_previewActor->GetProperty()->SetPointSize(3);

	m_previewPoints->Reset();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	QPointF diffVec = m_point2 - m_point1;
	QPointF v;
	for (int i = 0; i < m_numberOfPoints; ++i) {
		double param = i / (double)(m_numberOfPoints - 1);
		v = m_point1 + param * diffVec;
		points->InsertNextPoint(v.x(), v.y(), 0);
	}
	m_previewPoints->SetPoints(points);
	for (int i = 0; i < m_numberOfPoints; ++i) {
		vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
		vertex->GetPointIds()->SetId(0, i);
		m_previewPoints->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
	}
	m_previewActor->VisibilityOn();
	renderGraphicsView();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::hidePreviewSetting()
{
	m_previewActor->VisibilityOff();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::exitDialogMode()
{
	m_dialog = nullptr;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	double x = event->x();
	double y = event->y();
	dataModel()->graphicsView()->viewportToWorld(x, y);
	QPointF p(x, y);
	if (m_dialog != nullptr) {
		m_dialog->informButtonDown(p);
	}
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	double x = event->x();
	double y = event->y();
	dataModel()->graphicsView()->viewportToWorld(x, y);
	QPointF p(x, y);
	if (m_dialog != nullptr) {
		m_dialog->informButtonUp(p);
	}
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	if (m_dialog != nullptr) {
		dataModel()->graphicsView()->emitWorldPosition(event->x(), event->y());
	}
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_previewActor->SetPosition(0, 0, range.max());
	Post2dWindowNodeVectorStreamlineGroupDataItem::assignActorZValues(range);
}
