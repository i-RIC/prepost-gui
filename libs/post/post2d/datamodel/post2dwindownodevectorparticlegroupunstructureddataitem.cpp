#include "../post2dwindowgraphicsview.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem.h"
#include "post2dwindowparticleunstructuredsettingdialog.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QMouseEvent>
#include <QSettings>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkVertex.h>

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::~Post2dWindowNodeVectorParticleGroupUnstructuredDataItem()
{
	renderer()->RemoveActor(m_previewActor);
}

QDialog* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowParticleUnstructuredSettingDialog* dialog = new Post2dWindowParticleUnstructuredSettingDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr){
		delete dialog;
		return nullptr;
	}
	dialog->setDataItem(this);
	dialog->setZoneData(cont);
	dialog->setActiveAvailable(cont->IBCExists());

	dialog->setSolution(m_currentSolution);
	dialog->setTimeMode(m_timeMode);
	dialog->setTimeSamplingRate(m_timeSamplingRate);
	dialog->setTimeDivision(m_timeDivision);
	dialog->setSettings(m_settings);
	dialog->setRegionMode(m_regionMode);
	return dialog;
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setDefaultValues()
{
	m_settings.clear();

	Post2dWindowUnstructuredParticleSetSetting s;

	QSettings setting;
	s.color = setting.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();
	s.size = 2;
	s.pointsSet = false;
	s.numberOfPoints = 10;
	m_settings.append(s);
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

	m_previewActor->VisibilityOff();
	renderer()->AddActor(m_previewActor);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupActors()
{
	for (int i = 0; i < m_settings.count(); ++i){
		Post2dWindowUnstructuredParticleSetSetting& setting = m_settings[i];
		vtkActor* actor = vtkActor::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(setting.color.redF(), setting.color.greenF(), setting.color.blueF());
		prop->SetPointSize(setting.size);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkDataSetMapper* mapper = vtkDataSetMapper::New();
		actor->SetMapper(mapper);

		m_particleActors.append(actor);
		m_particleMappers.append(mapper);
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupParticleSources()
{
	for (int i = 0; i < m_sourcePoints.count(); ++i){
		m_sourcePoints[i]->Delete();
	}
	m_sourcePoints.clear();
	for (int i = 0; i < m_settings.count(); ++i){
		Post2dWindowUnstructuredParticleSetSetting& setting = m_settings[i];
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		QVector2D diffVec = setting.point2 - setting.point1;
		QVector2D v;
		for (int j = 0; j < setting.numberOfPoints; ++j){
			double param = j / (double)(setting.numberOfPoints - 1);
			v = setting.point1 + param * diffVec;
			points->InsertNextPoint(v.x(), v.y(), 0);
		}
		vtkUnstructuredGrid* grid = vtkUnstructuredGrid::New();
		grid->SetPoints(points);
		for (int j = 0; j <setting.numberOfPoints; ++j){
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

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setSetting(const QVector2D& v1, const QVector2D& v2, int num, int pointSize)
{
	m_point1 = v1;
	m_point2 = v2;
	m_numberOfPoints = num;
	m_previewActor->GetProperty()->SetPointSize(pointSize);

	m_previewPoints->Reset();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	QVector2D diffVec = m_point2 - m_point1;
	QVector2D v;
	for (int i = 0; i < m_numberOfPoints; ++i){
		double param = i / (double)(m_numberOfPoints - 1);
		v = m_point1 + param * diffVec;
		points->InsertNextPoint(v.x(), v.y(), 0);
	}
	m_previewPoints->SetPoints(points);
	for (int i = 0; i < m_numberOfPoints; ++i){
		vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
		vertex->GetPointIds()->SetId(0, i);
		m_previewPoints->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
	}
	m_previewActor->VisibilityOn();
	renderGraphicsView();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::clearSetting()
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
	QVector2D p(x, y);
	if (m_dialog != nullptr){
		m_dialog->informButtonDown(p);
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	double x = event->x();
	double y = event->y();
	dataModel()->graphicsView()->viewportToWorld(x, y);
	QVector2D p(x, y);
	if (m_dialog != nullptr){
		m_dialog->informButtonUp(p);
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mouseMoveEvent(QMouseEvent *event, VTKGraphicsView * /*v*/)
{
	if (m_dialog != nullptr){
		dataModel()->graphicsView()->emitWorldPosition(event->x(), event->y());
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::assignActionZValues(const ZDepthRange &range)
{
	m_previewActor->SetPosition(0, 0, range.max());
	Post2dWindowNodeVectorParticleGroupDataItem::assignActionZValues(range);
}



void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);

	m_settings.clear();
	QDomNode particlesNode = iRIC::getChildNode(node, "Particles");
	if (! particlesNode.isNull()){
		QDomNodeList particles = particlesNode.childNodes();
		for (int i = 0; i < particles.length(); ++i){
			QDomElement elem = particles.at(i).toElement();
			Post2dWindowUnstructuredParticleSetSetting s;
			s.point1.setX(static_cast<qreal>(elem.attribute("point1X").toDouble()));
			s.point1.setY(static_cast<qreal>(elem.attribute("point1Y").toDouble()));
			s.point2.setX(static_cast<qreal>(elem.attribute("point2X").toDouble()));
			s.point2.setY(static_cast<qreal>(elem.attribute("point2Y").toDouble()));
			s.pointsSet = static_cast<bool>(elem.attribute("pointsSet").toInt());
			s.numberOfPoints = elem.attribute("numberOfPoints").toInt();
			s.color = loadColorAttribute("color",elem, s.color);
			s.size = elem.attribute("size").toInt();
			m_settings.append(s);
		}
	}
	updateActorSettings();
}


void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("Particles");
	for (int i = 0; i < m_settings.count(); ++i){
		Post2dWindowUnstructuredParticleSetSetting& setting = m_settings[i];
		writer.writeStartElement("Particle");
		writer.writeAttribute("point1X", QString::number(setting.point1.x()));
		writer.writeAttribute("point1Y", QString::number(setting.point1.y()));
		writer.writeAttribute("point2X", QString::number(setting.point2.x()));
		writer.writeAttribute("point2Y", QString::number(setting.point2.y()));
		writer.writeAttribute("pointsSet", QString::number(static_cast<int>(setting.pointsSet)));
		writer.writeAttribute("numberOfPoints", QString::number(setting.numberOfPoints));
		writeColorAttribute("color", setting.color, writer);
		writer.writeAttribute("size", QString::number(setting.size));
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
