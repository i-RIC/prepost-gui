#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindowpointscalargrouptopdataitemi.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodescalargroupdataitem_propertydialog.h"
#include "private/post2dwindownodescalargroupdataitem_shapeexporter.h"

#include <cs/coordinatesystem.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>

Post2dWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &opacity, &colorMapSetting},
	colorMapSetting {},
	regionSetting {},
	opacity {"opacity", 50}
{}

Post2dWindowNodeScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowNodeScalarGroupDataItem::Setting& Post2dWindowNodeScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}

Post2dWindowNodeScalarGroupDataItem::Post2dWindowNodeScalarGroupDataItem(const std::string& target, iRICLib::H5CgnsZone::SolutionPosition position, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	m_target {target},
	m_solutionPosition {position},
	m_setting {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_shapeExporter {new ShapeExporter(this)}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	auto gType = topDataItem()->zoneDataItem()->dataContainer()->gridType();
	standardItem()->setText(gType->output(target)->caption());

	setupActors();
}

Post2dWindowNodeScalarGroupDataItem::~Post2dWindowNodeScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();

	delete m_shapeExporter;
}

const std::string& Post2dWindowNodeScalarGroupDataItem::target() const
{
	return m_target;
}

const ColorMapSettingContainer& Post2dWindowNodeScalarGroupDataItem::colorMapSetting() const
{
	return m_setting.colorMapSetting;
}

void Post2dWindowNodeScalarGroupDataItem::updateActorSettings()
{
	auto cont = topDataItem()->zoneDataItem()->dataContainer();
	if (cont == nullptr || cont->data(m_solutionPosition) == nullptr) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOffWithEmptyPolyData();
		m_actor->SetMapper(mapper);
		mapper->Delete();
		return;
	}

	auto range = topDataItem()->zoneDataItem()->gridTypeDataItem()->nodeValueRange(m_target);
	m_setting.colorMapSetting.setAutoValueRange(range);

	auto filtered = m_setting.regionSetting.buildNodeFilteredData(cont->data(m_solutionPosition)->data());
	filtered->GetPointData()->SetActiveScalars(m_target.c_str());
	auto mapper = m_setting.colorMapSetting.buildPointDataMapper(filtered);
	m_actor->SetMapper(mapper);

	filtered->Delete();
	mapper->Delete();

	m_setting.colorMapSetting.legend.imageSetting.apply(dataModel()->graphicsView());
	m_actor->GetProperty()->SetOpacity(m_setting.opacity);
}

void Post2dWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSettings();
}

void Post2dWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowNodeScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	m_actorCollection->AddItem(m_actor);

	m_setting.colorMapSetting.legend.imageSetting.setActor(m_legendActor);
	m_setting.colorMapSetting.legend.imageSetting.controller()->setItem(this);
	m_setting.colorMapSetting.legend.title = topDataItem()->zoneDataItem()->dataContainer()->gridType()->output(target())->caption();

	updateActorSettings();
}

void Post2dWindowNodeScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowNodeScalarGroupDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, scale, 1);
}

Post2dWindowPointScalarGroupTopDataItemI *Post2dWindowNodeScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowPointScalarGroupTopDataItemI*> (parent());
}

void Post2dWindowNodeScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new PropertyDialog(this, p);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	return dialog;
}

bool Post2dWindowNodeScalarGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (m_setting.opacity == 100) {return false;}

	return true;
}

void Post2dWindowNodeScalarGroupDataItem::informSelection(VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleSelection(v);
	topDataItem()->zoneDataItem()->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeScalarGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleDeselection(v);
	topDataItem()->zoneDataItem()->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);

	auto zItem = topDataItem()->zoneDataItem();
	switch (m_solutionPosition) {
	case iRICLib::H5CgnsZone::SolutionPosition::Node:
		zItem->updateNodeResultAttributeBrowser(event->pos(), v);
		break;
	case iRICLib::H5CgnsZone::SolutionPosition::IFace:
		zItem->updateEdgeIResultAttributeBrowser(event->pos(), v);
		break;
	case iRICLib::H5CgnsZone::SolutionPosition::JFace:
		zItem->updateEdgeJResultAttributeBrowser(event->pos(), v);
		break;
	}
}

void Post2dWindowNodeScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post2dWindowNodeScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	if (event->button() == Qt::LeftButton) {
		auto zItem = topDataItem()->zoneDataItem();
		switch (m_solutionPosition) {
		case iRICLib::H5CgnsZone::SolutionPosition::Node:
			zItem->fixNodeResultAttributeBrowser(event->pos(), v);
			break;
		case iRICLib::H5CgnsZone::SolutionPosition::IFace:
			zItem->fixEdgeIResultAttributeBrowser(event->pos(), v);
			break;
		case iRICLib::H5CgnsZone::SolutionPosition::JFace:
			zItem->fixEdgeJResultAttributeBrowser(event->pos(), v);
			break;
		}
	}
}


bool Post2dWindowNodeScalarGroupDataItem::checkKmlExportCondition()
{
	// check the condition.
	if (m_setting.colorMapSetting.transitionMode != ColorMapSettingContainer::TransitionMode::Discrete) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, Colormode needs to be \"Discrete Mode\""));
		return false;
	}
	CoordinateSystem* cs = projectData()->mainfile()->coordinateSystem();
	if (cs == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, coordinate system should be specified."));
		return false;
	}

	return true;
}

bool Post2dWindowNodeScalarGroupDataItem::exportKMLHeader(QXmlStreamWriter& writer)
{
	writer.writeStartElement("Document");
	writer.writeTextElement("name", "iRIC Calculation Result");
	writer.writeTextElement("open", "1");

	// output styles.
	const auto& cm = m_setting.colorMapSetting;
	auto colors = cm.getColors();
	for (int i = 0; i < colors.size(); ++i) {
		QColor col = colors.at(i).color;
		QString colorStr = QString("%1%2%3%4").arg("b3").arg(col.blue(), 2, 16, QChar('0')).arg(col.green(), 2, 16, QChar('0')).arg(col.red(), 2, 16, QChar('0'));

		writer.writeStartElement("Style");
		writer.writeAttribute("id", QString("Rank%1").arg(i + 1));

		writer.writeStartElement("LineStyle");
		writer.writeTextElement("color", colorStr);
		writer.writeTextElement("width", "0");
		writer.writeEndElement();

		writer.writeStartElement("PolyStyle");
		writer.writeTextElement("color", colorStr);
		writer.writeTextElement("fill", "true");
		writer.writeTextElement("outline", "false");
		writer.writeEndElement();

		writer.writeEndElement();
	}

	return true;
}

bool Post2dWindowNodeScalarGroupDataItem::exportKMLFooter(QXmlStreamWriter& writer)
{
	writer.writeEndElement();
	return true;
}

bool Post2dWindowNodeScalarGroupDataItem::exportKMLForTimestep(QXmlStreamWriter& writer, int /*index*/, double time, bool oneShot)
{
	CoordinateSystem* cs = projectData()->mainfile()->coordinateSystem();
	const auto& cm = m_setting.colorMapSetting;

	// Folder start
	writer.writeStartElement("Folder");
	if (! oneShot) {
		// TimeStamp Start
		QDateTime datetime(QDate(2011, 1, 1));
		auto zeroDateTime = projectData()->mainfile()->zeroDateTime();
		if (! zeroDateTime.isNull()) {
			datetime = zeroDateTime;
		}
		datetime = datetime.addSecs(static_cast<int>(time));
		writer.writeStartElement("TimeStamp");
		writer.writeTextElement("when", datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));
		// TimeStamp End
		writer.writeEndElement();
	}

	// name
	writer.writeTextElement("name", QString("iRIC output t = %1").arg(time));

	// output each cell data.
	PostZoneDataContainer* cont = topDataItem()->zoneDataItem()->dataContainer();
	auto ps = cont->data()->data();
	auto da = ps->GetPointData()->GetArray(m_target.c_str());

	double minValue = cm.getMinValue();
	auto colors = cm.getColors();

	auto off = offset();

	std::vector<std::vector<std::vector<QVector3D> > > polygons;
	std::vector<std::vector<QVector3D> > emptyList;
	for (int i = 0; i < colors.size(); ++i) {
		polygons.push_back(emptyList);
	}

	for (vtkIdType cellId = 0; cellId < ps->GetNumberOfCells(); ++cellId) {
		std::vector<QVector3D> points;
		vtkCell* cell = ps->GetCell(cellId);

		double sum = 0;
		for (vtkIdType idx = 0; idx < cell->GetNumberOfPoints(); ++idx) {
			vtkIdType pointId = cell->GetPointId(idx);

			double v[3];
			double lon, lat;
			ps->GetPoint(pointId, v);
			v[0] += off.x();
			v[1] += off.y();
			cs->mapGridToGeo(v[0], v[1], &lon, &lat);
			double val = da->GetTuple1(pointId);
			points.push_back(QVector3D(lon, lat, val));
			sum += val;
		}

		double averageDepth = sum / points.size();

		int colorIndex = colors.size() - 1;
		for (int i = 0; i < colors.size() - 1; ++i) {
			if (averageDepth < colors.at(i + 1).value) {
				colorIndex = i;
			}
		}

		auto& pols = polygons[colorIndex];
		pols.push_back(points);
	}
	// polygons setup.
	for (int i = 0; i < polygons.size(); ++i) {
		double min;
		if (i == 0) {
			min = minValue;
		} else {
			min = colors.at(i - 1).value;
		}
		double max = colors.at(i).value;

		writer.writeStartElement("Placemark");
		auto name = QString("%1 - %2 m").arg(min).arg(max);
		writer.writeTextElement("name", name);
		writer.writeTextElement("styleUrl", QString("#Rank%1").arg(i + 1));
		writer.writeStartElement("MultiGeometry");

		auto& pols = polygons.at(i);
		for (auto& points : pols) {
			QStringList coordinates;
			for (int i = 0; i <= points.size(); ++i) {
				const auto& p = points.at(i % points.size());
				coordinates.push_back(QString("%1,%2,%3").arg(QString::number(p.x(), 'g', 16)).arg(QString::number(p.y(), 'g', 16)).arg(QString::number(p.z(), 'g', 8)));
			}
			writer.writeStartElement("Polygon");
			writer.writeTextElement("altitudeMode", "relativeToGround");
			writer.writeStartElement("outerBoundaryIs");
			writer.writeStartElement("LinearRing");
			writer.writeTextElement("coordinates", coordinates.join("\r\n"));
			writer.writeEndElement(); // LinearRing
			writer.writeEndElement(); // outerBoundaryIs
			writer.writeEndElement(); // Polygon
		}

		writer.writeEndElement(); // MultiGeometry
		writer.writeEndElement(); // Placemark
	}

	writer.writeEndElement(); // Folder
	return true;
}

bool Post2dWindowNodeScalarGroupDataItem::exportContourFigureToShape(const QString& filename, double time)
{
	return m_shapeExporter->exportContourFigure(filename, time);
}


void Post2dWindowNodeScalarGroupDataItem::doHandleResize(VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleResize(v);
}

void Post2dWindowNodeScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = topDataItem()->zoneDataItem()->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}
