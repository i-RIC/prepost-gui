#include "../post2dwindowgraphicsview.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem.h"
#include "post2dwindowparticleunstructuredsettingdialog.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QMouseEvent>
#include <QSettings>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkVertex.h>

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::Setting() :
	CompositeContainer ({&point1, &point2, &pointsSet, &numberOfPoints, &color, &size}),
	point1 {"point1"},
	point2 {"point2"},
	pointsSet {"pointsSet", false},
	numberOfPoints {"numberOfPoints", 10},
	color {"color"},
	size {"size", DEFAULT_SIZE}
{
	QSettings settings;
	color = settings.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting& Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::~Post2dWindowNodeVectorParticleGroupUnstructuredDataItem()
{
	renderer()->RemoveActor(m_previewActor);
}

QDialog* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowParticleUnstructuredSettingDialog* dialog = new Post2dWindowParticleUnstructuredSettingDialog(p);
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

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setDefaultValues()
{
	m_unstSettings.clear();

	Setting s;
	m_unstSettings.append(s);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupTmpSource()
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
	m_previewActor->SetScale(1, m_zScale, 1);

	m_previewActor->VisibilityOff();
	renderer()->AddActor(m_previewActor);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupActors()
{
	for (int i = 0; i < m_unstSettings.count(); ++i) {
		const Setting& s = m_unstSettings[i];
		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(s.color);
		prop->SetPointSize(s.size);
		actor->SetScale(1, m_zScale, 1);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		actor->SetMapper(mapper);

		m_particleActors.push_back(actor);
		m_particleMappers.push_back(mapper);
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupParticleSources()
{
	for (int i = 0; i < m_sourcePoints.count(); ++i) {
		m_sourcePoints[i]->Delete();
	}
	m_sourcePoints.clear();
	for (int i = 0; i < m_unstSettings.count(); ++i) {
		const Setting& s = m_unstSettings[i];
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		QPointF diffVec = s.point2 - s.point1;
		QPointF v;
		for (int j = 0; j < s.numberOfPoints; ++j) {
			double param = j / (double)(s.numberOfPoints - 1);
			v = s.point1 + param * diffVec;
			points->InsertNextPoint(v.x(), v.y(), 0);
		}
		vtkUnstructuredGrid* grid = vtkUnstructuredGrid::New();
		grid->SetPoints(points);
		for (int j = 0; j < s.numberOfPoints; ++j) {
			vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
			vertex->GetPointIds()->SetId(0, j);
			grid->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
		}
		m_sourcePoints.append(grid);
	}
}

vtkPointSet* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::newParticles(int i)
{
	return m_sourcePoints[i];
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::showPropertyDialog()
{
	dataModel()->objectBrowserView()->select(m_standardItem->index());
	Post2dWindowParticleUnstructuredSettingDialog* propDialog = dynamic_cast<Post2dWindowParticleUnstructuredSettingDialog*>(propertyDialog(mainWindow()));
	iricMainWindow()->enterModelessDialogMode();
	connect(dataModel()->graphicsView(), SIGNAL(worldPositionChanged(QVector2D)), propDialog, SLOT(updateMousePosition(QVector2D)));
	connect(propDialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(propDialog, SIGNAL(destroyed()), this, SLOT(exitDialogMode()));
	m_dialog = propDialog;
	propDialog->show();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setSetting(const QPointF &v1, const QPointF &v2, int num, int pointSize)
{
	m_point1 = v1;
	m_point2 = v2;
	m_numberOfPoints = num;
	m_previewActor->GetProperty()->SetPointSize(pointSize);

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

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::hidePreviewSetting()
{
	m_previewActor->VisibilityOff();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::exitDialogMode()
{
	m_dialog = nullptr;
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	double x = event->x();
	double y = event->y();
	dataModel()->graphicsView()->viewportToWorld(x, y);
	QPointF p(x, y);
	if (m_dialog != nullptr) {
		m_dialog->informButtonDown(p);
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	double x = event->x();
	double y = event->y();
	dataModel()->graphicsView()->viewportToWorld(x, y);
	QPointF p(x, y);
	if (m_dialog != nullptr) {
		m_dialog->informButtonUp(p);
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	if (m_dialog != nullptr) {
		dataModel()->graphicsView()->emitWorldPosition(event->x(), event->y());
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_previewActor->SetPosition(0, 0, range.max());
	Post2dWindowNodeVectorParticleGroupDataItem::assignActorZValues(range);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);

	m_unstSettings.clear();
	QDomNode particlesNode = iRIC::getChildNode(node, "Particles");
	if (! particlesNode.isNull()) {
		QDomNodeList particles = particlesNode.childNodes();
		for (int i = 0; i < particles.length(); ++i) {
			Setting s;
			s.load(particles.at(i));
			m_unstSettings.append(s);
		}
	}
	updateActorSettings();
}


void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("Particles");
	for (int i = 0; i < m_unstSettings.count(); ++i) {
		const Setting& s = m_unstSettings[i];
		writer.writeStartElement("Particle");
		s.save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
