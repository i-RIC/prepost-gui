#include "../post2dwindowattributebrowsercontroller.h"
#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodescalargroupdataitem_impl.h"
#include "private/post2dwindownodescalargroupdataitem_settingeditwidget.h"
#include "private/post2dwindownodescalargroupdataitem_shapeexporter.h"

#include <cs/coordinatesystem.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/objectbrowserview.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/public/v4grid_attributedataprovider.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <misc/stringtool.h>

namespace {

struct Point3D {
	Point3D(double xx, double yy, double zz):
		x {xx}, y {yy}, z {zz}
	{}

	double x;
	double y;
	double z;
};

}

Post2dWindowNodeScalarGroupDataItem::Post2dWindowNodeScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, Reorderable, Deletable);

	impl->m_target = target;

	auto grid = topDataItem()->zoneDataItem()->v4DataContainer()->gridData();
	auto adProvider = grid->grid()->attributeDataProvider();
	QString caption = adProvider->caption(target);

	standardItem()->setText(caption);
	auto cs = adProvider->createColorMapSetting(target);
	cs->legendSetting()->setTitle(caption);
	impl->m_setting.colorMapSetting = cs;
	impl->m_setting.contourSetting.setColorMapSetting(cs);

	impl->m_colorMapToolBarWidget->hide();
	impl->m_colorMapToolBarWidget->setSetting(impl->m_setting.colorMapSetting);
	connect(impl->m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(impl->m_colorMapToolBarWidget->modifiedSetting(), impl->m_setting.colorMapSetting);
		pushUpdateActorSettingCommand(com, this);
	});

	impl->m_opacityToolBarWidget->hide();
	impl->m_opacityToolBarWidget->setContainer(&impl->m_setting.opacity);
	connect(impl->m_opacityToolBarWidget, &OpacityContainerWidget::updated, [=](){
		auto com = impl->m_opacityToolBarWidget->createModifyCommand();
		pushUpdateActorSettingCommand(com, this, false);
	});

	setupActors();
}

Post2dWindowNodeScalarGroupDataItem::~Post2dWindowNodeScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);
	r->RemoveActor2D(impl->m_legendActor);
}

const std::string& Post2dWindowNodeScalarGroupDataItem::target() const
{
	return impl->m_target;
}

void Post2dWindowNodeScalarGroupDataItem::updateActorSetting()
{
	impl->m_actor->VisibilityOff();

	auto z = topDataItem()->zoneDataItem();
	auto cont = z->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOffWithEmptyPolyData();
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
		return;
	}
	auto v = dataModel()->graphicsView();
	auto grid2d = dynamic_cast<v4Grid2d*> (cont->gridData()->grid());

	auto range = topDataItem()->zoneDataItem()->gridTypeDataItem()->nodeValueRange(impl->m_target);
	impl->m_setting.colorMapSetting->setAutoValueRange(range);

	vtkPointSet* filtered = nullptr;
	if (impl->m_setting.regionSetting.mode == Region2dSettingContainer::Mode::Full) {
		filtered = grid2d->vtkFilteredData();
		if (filtered != nullptr) {
			filtered->Register(nullptr);
		}
	} else {
		filtered = impl->m_setting.regionSetting.buildNodeFilteredData(cont->gridData()->grid()->vtkData()->data());
	}
	if (filtered == nullptr) {return;}

	filtered->GetPointData()->SetActiveScalars(impl->m_target.c_str());
	auto filtered2 = impl->m_setting.contourSetting.buildFilteredData(filtered);
	filtered->Delete();

	auto mapper = impl->m_setting.colorMapSetting->buildPointDataMapper(filtered2);
	filtered2->Delete();

	impl->m_actor->SetMapper(mapper);
	mapper->Delete();

	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->apply(dataModel()->graphicsView());
	impl->m_actor->GetProperty()->SetOpacity(impl->m_setting.opacity);
	impl->m_actor->GetProperty()->SetLineWidth(v->devicePixelRatioF() * impl->m_setting.contourSetting.contourLineWidth);

	updateVisibilityWithoutRendering();
}

void Post2dWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	updateActorSetting();
}

void Post2dWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void Post2dWindowNodeScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_actor);
	r->AddActor2D(impl->m_legendActor);

	m_actorCollection->AddItem(impl->m_actor);

	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->setActor(impl->m_legendActor);
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->setItem(this);

	updateActorSetting();
}

void Post2dWindowNodeScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowNodeScalarGroupDataItem::update()
{
	updateActorSetting();
}

void Post2dWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	impl->m_actor->SetScale(1, scale, 1);
}

Post2dWindowNodeScalarGroupTopDataItem *Post2dWindowNodeScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowNodeScalarGroupTopDataItem*> (parent());
}

void Post2dWindowNodeScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	dialog->resize(900, 700);

	return dialog;
}

bool Post2dWindowNodeScalarGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (impl->m_setting.opacity == 100) {return false;}

	return true;
}

void Post2dWindowNodeScalarGroupDataItem::gatherActiveColorMapLegends(std::vector<ColorMapLegendSettingContainerI*>* legends)
{
	if (! (isAncientChecked() && isChecked())) {return;}
	if (! impl->m_setting.colorMapSetting->legendSetting()->getVisible()) {return;}

	legends->push_back(impl->m_setting.colorMapSetting->legendSetting());
}

void Post2dWindowNodeScalarGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	topDataItem()->attributeBrowserController()->initialize();
}

void Post2dWindowNodeScalarGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	topDataItem()->attributeBrowserController()->clear();
}

void Post2dWindowNodeScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
	topDataItem()->attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowNodeScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void Post2dWindowNodeScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
	if (event->button() == Qt::LeftButton) {
		topDataItem()->attributeBrowserController()->fix(event->pos(), v);
	}
}

bool Post2dWindowNodeScalarGroupDataItem::checkKmlExportCondition()
{
	// check the condition.
	auto colorMapSetting = dynamic_cast<ColorMapSettingContainer*>(impl->m_setting.colorMapSetting);
	if (colorMapSetting == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, The value should be real value, This is an integer value."));
		return false;
	}

	if (colorMapSetting->transitionMode != ColorMapSettingContainer::TransitionMode::Discrete) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, Color mode needs to be \"Discrete Mode\""));
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
	auto cm = dynamic_cast<ColorMapSettingContainer*> (impl->m_setting.colorMapSetting);
	auto colors = cm->getColors();
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
	auto cm = dynamic_cast<ColorMapSettingContainer*> (impl->m_setting.colorMapSetting);

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
	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	auto ps = cont->gridData()->grid()->vtkData()->data();
	auto da = ps->GetPointData()->GetArray(impl->m_target.c_str());

	double minValue = cm->getMinValue();
	auto colors = cm->getColors();

	auto off = offset();

	std::vector<std::vector<std::vector<Point3D> > > polygons;
	std::vector<std::vector<Point3D> > emptyList;
	for (int i = 0; i < colors.size(); ++i) {
		polygons.push_back(emptyList);
	}

	for (vtkIdType cellId = 0; cellId < ps->GetNumberOfCells(); ++cellId) {
		std::vector<Point3D> points;
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
			points.push_back(Point3D(lon, lat, val));
			// points.push_back(Point3D(v[0], v[1], val));
			sum += val;
		}

		double averageDepth = sum / points.size();

		if (! cm->fillLower && averageDepth < cm->getMinValue()) {continue;}
		if (! cm->fillUpper && averageDepth > cm->getMaxValue()) {continue;}

		int colorIndex = static_cast<int> (colors.size()) - 1;
		for (int i = 0; i < colors.size() - 1; ++i) {
			if (averageDepth < colors.at(i).value) {
				colorIndex = i;
				break;
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
				coordinates.push_back(QString("%1,%2,%3").arg(QString::number(p.x, 'g', 16)).arg(QString::number(p.y, 'g', 16)).arg(QString::number(p.z, 'g', 8)));
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

bool Post2dWindowNodeScalarGroupDataItem::colorMapIsDiscreteMode()
{
	auto cs = dynamic_cast<ColorMapSettingContainer*> (impl->m_setting.colorMapSetting);
	if (cs == nullptr) {return false;}

	return (cs->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete);
}

bool Post2dWindowNodeScalarGroupDataItem::exportContourFigureToShape(const QString& filename, double time)
{
	return impl->m_shapeExporter->exportContourFigure(filename, time);
}

void Post2dWindowNodeScalarGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

void Post2dWindowNodeScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(topDataItem()->showAttributeBrowserAction());
}

bool Post2dWindowNodeScalarGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	impl->m_colorMapToolBarWidget->setParent(toolBar);
	impl->m_colorMapToolBarWidget->show();

	toolBar->addWidget(impl->m_colorMapToolBarWidget);
	toolBar->addSeparator();

	impl->m_opacityToolBarWidget->setParent(toolBar);
	impl->m_opacityToolBarWidget->show();

	toolBar->addWidget(impl->m_opacityToolBarWidget);

	return true;
}

void Post2dWindowNodeScalarGroupDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	auto parentItem = dynamic_cast<GraphicsWindowDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}
