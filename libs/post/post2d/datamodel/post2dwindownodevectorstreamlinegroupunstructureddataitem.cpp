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
#include <QUndoCommand>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkVertex.h>

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(Post2dWindowDataItem* parent)
	: Post2dWindowNodeVectorStreamlineGroupDataItem(parent)
{
	setupDefaultValues();
	setupTmpSource();
	m_dialog = nullptr;
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

	dialog->setSolution(m_currentSolution);
	dialog->setSettings(m_settings);
	dialog->setRegionMode(m_regionMode);

	return dialog;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setupDefaultValues()
{
	m_settings.clear();

	Post2dWindowUnstructuredStreamlineSetSetting s;

	QSettings setting;
	s.color = setting.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();
	s.width = 1;
	s.pointsSet = false;
	s.numberOfPoints = 10;
	m_settings.append(s);
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
	for (int i = 0; i < m_sourcePoints.count(); ++i) {
		m_sourcePoints[i]->Delete();
	}
	m_sourcePoints.clear();

	for (int i = 0; i < m_settings.count(); ++i) {
		Post2dWindowUnstructuredStreamlineSetSetting& setting = m_settings[i];

		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		QVector2D diffVec = setting.point2 - setting.point1;
		QVector2D v;
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
		m_sourcePoints.append(grid);

		vtkActor* actor = vtkActor::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(setting.color.redF(), setting.color.greenF(), setting.color.blueF());
		prop->SetLineWidth(setting.width);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkDataSetMapper* mapper = vtkDataSetMapper::New();
		actor->SetMapper(mapper);

		vtkStreamTracer* tracer = vtkStreamTracer::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(getSource(i));
		tracer->SetInputData(getRegion());
		tracer->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(m_currentSolution).c_str());

		mapper->SetInputConnection(tracer->GetOutputPort());

		m_streamlineActors.append(actor);
		m_streamlineMappers.append(mapper);
		m_streamTracers.append(tracer);
	}
}


vtkPointSet* Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::getSource(int i)
{
	return m_sourcePoints[i];
}


void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);

	m_settings.clear();
	QDomNode streamlinesNode = iRIC::getChildNode(node, "Streamlines");
	if (! streamlinesNode.isNull()) {
		QDomNodeList streamlines = streamlinesNode.childNodes();
		for (int i = 0; i < streamlines.length(); ++i) {
			QDomElement elem = streamlines.at(i).toElement();
			Post2dWindowUnstructuredStreamlineSetSetting s;
			s.point1.setX(static_cast<qreal>(elem.attribute("point1X").toDouble()));
			s.point1.setY(static_cast<qreal>(elem.attribute("point1Y").toDouble()));
			s.point2.setX(static_cast<qreal>(elem.attribute("point2X").toDouble()));
			s.point2.setY(static_cast<qreal>(elem.attribute("point2Y").toDouble()));
			s.pointsSet = static_cast<bool>(elem.attribute("pointsSet").toInt());
			s.numberOfPoints = elem.attribute("numberOfPoints").toInt();
			s.color = loadColorAttribute("color",elem, s.color);
			s.width = elem.attribute("width").toInt();
			m_settings.append(s);
		}
	}
	updateActorSettings();

}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("Streamlines");
	for (int i = 0; i < m_settings.count(); ++i) {
		Post2dWindowUnstructuredStreamlineSetSetting& setting = m_settings[i];
		writer.writeStartElement("Streamline");
		writer.writeAttribute("point1X", QString::number(setting.point1.x()));
		writer.writeAttribute("point1Y", QString::number(setting.point1.y()));
		writer.writeAttribute("point2X", QString::number(setting.point2.x()));
		writer.writeAttribute("point2Y", QString::number(setting.point2.y()));
		writer.writeAttribute("pointsSet", QString::number(static_cast<int>(setting.pointsSet)));
		writer.writeAttribute("numberOfPoints", QString::number(setting.numberOfPoints));
		writeColorAttribute("color", setting.color, writer);
		writer.writeAttribute("width", QString::number(setting.width));
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

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setSetting(const QVector2D& v1, const QVector2D& v2, int num)
{
	m_point1 = v1;
	m_point2 = v2;
	m_numberOfPoints = num;
	m_previewActor->GetProperty()->SetPointSize(3);

	m_previewPoints->Reset();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	QVector2D diffVec = m_point2 - m_point1;
	QVector2D v;
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

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::clearSetting()
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
	QVector2D p(x, y);
	if (m_dialog != nullptr) {
		m_dialog->informButtonDown(p);
	}
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	double x = event->x();
	double y = event->y();
	dataModel()->graphicsView()->viewportToWorld(x, y);
	QVector2D p(x, y);
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
