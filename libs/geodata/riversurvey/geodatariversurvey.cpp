#include "geodatarivercrosssection.h"
#include "geodatariverpathpointexpanddialog.h"
#include "geodatariverpathpointextensionadddialog.h"
#include "geodatariverpathpointinsertdialog.h"
#include "geodatariverpathpointmovedialog.h"
#include "geodatariverpathpointrenamedialog.h"
#include "geodatariverpathpointrotatedialog.h"
#include "geodatariverpathpointshiftdialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"
#include "geodatariversurveybuildbanklinesdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowprojectdataitem.h"
#include "geodatariversurveydisplaysettingdialog.h"
#include "geodatariversurveygeneratedialog.h"
#include "geodatariversurveymappointsdialog.h"
#include "geodatariversurveyproxy.h"
#include "private/geodatariversurvey_changeselectioncommand.h"
#include "private/geodatariversurvey_deleteriverpathpointcommand.h"
#include "private/geodatariversurvey_impl.h"
#include "private/geodatariversurvey_mousemoveextensioncommand.h"
#include "private/geodatariversurvey_mouserotaterivercrosssectioncommand.h"
#include "private/geodatariversurvey_mouseshiftriverpathcentercommand.h"
#include "private/geodatariversurvey_polylinecoordinateseditor.h"
#include "private/geodatariversurvey_polylinefinishdefiningcommand.h"
#include "private/geodatariversurvey_polylineupdatelabelscommand.h"
#include "private/geodatariversurvey_removeextensioncommand.h"
#include "private/geodatariversurvey_setdisplaysettingcommand.h"
#include "private/geodatariversurvey_translateriverpathpointcommand.h"

#include <geodata/pointmap/geodatapointmap.h>
#include <guibase/polyline/polylineaddvertexcommand.h>
#include <guibase/polyline/polylinecontrollerutil.h>
#include <guibase/polyline/polylinemovevertexcommand.h>
#include <guibase/polyline/polylinepushvertexcommand.h>
#include <guibase/polyline/polylineremovevertexcommand.h>
#include <guibase/polyline/polylineutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/colorsource.h>
#include <guicore/project/projectdata.h>
#include <misc/informationdialog.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QDomElement>
#include <QFile>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPolygonF>
#include <QSet>
#include <QStandardItem>
#include <QToolBar>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActorCollection.h>
#include <vtkActor2DCollection.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <iriclib_riversurvey.h>

namespace {

QPointF nearestInterSection(const QPointF& p1, const QPointF& p2, std::vector<QPointF>& polyLine)
{
	double dist2 = iRIC::lengthSquared(p2 - p1) * 2;
	QPointF ret;
	for (int i = 0; i < polyLine.size() - 1; ++i) {
		const QPointF& q1 = polyLine.at(i);
		const QPointF& q2 = polyLine.at(i + 1);
		QPointF xsec;
		double r, s;
		bool found = iRIC::intersectionPoint(p1, p2, q1, q2, &xsec, &r, &s);

		if (! found) {continue;}
		if (r < 0 || r > 1) {continue;}
		if (s < 0 || s > 1) {continue;}

		double d = iRIC::lengthSquared(xsec - p1);
		if (d < dist2) {
			dist2 = d;
			ret = xsec;
		}
	}
	return ret;
}

void addPointAt(const QPointF& pos, const QPointF& dir, double dist, std::map<double, GeoDataRiverCrosssection::Altitude>* alts, GeoDataPointmap* pointMap)
{
	QPointF p = pos + dir * dist;
	double val;

	bool ok = pointMap->getValueAt(p, &val);
	if (! ok) {return;}

	alts->insert({dist, GeoDataRiverCrosssection::Altitude(dist, val)});
}

} // namespace

const int GeoDataRiverSurvey::WSE_NAME_MAXLENGTH = 16;

GeoDataRiverSurvey::GeoDataRiverSurvey(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoData(d, creator, att),
	impl {new Impl {this}}
{
	m_headPoint = new GeoDataRiverPathPoint("Dummy", 0, 0);
	connect(impl->m_gridThread, SIGNAL(gridUpdated()), this, SLOT(updateBackgroundGrid()));

	impl->setupActions();

	RiverSplineSolver::setLinearMode(false, m_headPoint);
	impl->updateMouseCursor(graphicsView());
}

GeoDataRiverSurvey::~GeoDataRiverSurvey()
{
	delete impl;
}

void GeoDataRiverSurvey::setEditMode()
{
	impl->m_mode = Impl::Mode::EditMode;
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::Normal;
	impl->m_menuIsSetup = false;
	impl->updateMouseCursor(graphicsView());
}

void GeoDataRiverSurvey::setCaption(const QString& cap)
{
	GeoData::setCaption(cap);
	updateCrosssectionWindows();
}

void GeoDataRiverSurvey::setupActors()
{
	impl->setupVtkObjects();
}

void GeoDataRiverSurvey::setupMenu()
{
	impl->setupMenu();
}

void GeoDataRiverSurvey::setupDataItem()
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (parent()->parent());
	auto childItems = gItem->childItems();
	int rcount = 1;
	for (GraphicsWindowDataItem* child : childItems) {
		auto i = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (child);
		if (dynamic_cast<GeoDataRiverSurvey*>(i->geoData()) != nullptr) {
			++ rcount;
		}
	}
	auto cs = new ColorSource(this);
	cs->load(":/libs/geodata/riversurvey/data/colorsource_rs.xml");

	m_setting.crosssectionLinesColor = cs->getColor(rcount);
	delete cs;

	GeoData::setupDataItem();
}

bool GeoDataRiverSurvey::addToolBarButtons(QToolBar* /*tb*/)
{
	/*
		 tb->addAction(m_addVertexAction);
		 tb->addAction(m_removeVertexAction);
		 return true;
	 */
	return true;
}

void GeoDataRiverSurvey::informSelection(PreProcessorGraphicsViewInterface*)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	vtkActor2DCollection* col2 = actor2DCollection();

	col->RemoveAllItems();
	col2->RemoveAllItems();

	if (impl->m_mode == Impl::Mode::CreateMode) {
		col->AddItem(impl->m_centerLineController.pointsActor());
		col->AddItem(impl->m_centerLineSplineController.linesActor());
		col->AddItem(impl->m_leftBankLineController.pointsActor());
		col->AddItem(impl->m_leftBankLineSplineController.linesActor());
		col->AddItem(impl->m_rightBankLineController.pointsActor());
		col->AddItem(impl->m_rightBankLineSplineController.linesActor());
		col->AddItem(impl->m_upstreamLineController.linesActor());
		col->AddItem(impl->m_downstreamLineController.linesActor());
		impl->updateLabelsAndSplines();
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		col->AddItem(impl->m_riverCenterPointsActor);
		col->AddItem(impl->m_crossSectionLinesActor);
		col->AddItem(impl->m_centerAndBankLinesActor);
		col->AddItem(impl->m_selectedRiverCenterPointsActor);
		col->AddItem(impl->m_selectedLeftBankPointsActor);
		col->AddItem(impl->m_selectedRightBankPointsActor);
		col->AddItem(impl->m_selectedCrossSectionLinesActor);
		if (m_setting.showBackground) {
			col->AddItem(impl->m_backgroundActor);
		}
		if (m_setting.showLines) {
			col->AddItem(impl->m_verticalCrossSectionLinesActor);
		}
		col2->AddItem(impl->m_labelActor);
	}

	updateVisibilityWithoutRendering();
}

void GeoDataRiverSurvey::informDeselection(PreProcessorGraphicsViewInterface* /*v*/)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	vtkActor2DCollection* col2 = actor2DCollection();
	col->RemoveAllItems();
	col2->RemoveAllItems();

	if (impl->m_mode == Impl::Mode::CreateMode) {
		col->AddItem(impl->m_centerLineController.pointsActor());
		col->AddItem(impl->m_centerLineSplineController.linesActor());
		col->AddItem(impl->m_leftBankLineController.pointsActor());
		col->AddItem(impl->m_leftBankLineSplineController.linesActor());
		col->AddItem(impl->m_rightBankLineController.pointsActor());
		col->AddItem(impl->m_rightBankLineSplineController.linesActor());
		col->AddItem(impl->m_upstreamLineController.linesActor());
		col->AddItem(impl->m_downstreamLineController.linesActor());
		impl->updateLabelsAndSplines();
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		col->AddItem(impl->m_crossSectionLinesActor);
		col->AddItem(impl->m_centerAndBankLinesActor);
		if (m_setting.showBackground) {
			col->AddItem(impl->m_backgroundActor);
		}
		if (m_setting.showLines) {
			col->AddItem(impl->m_verticalCrossSectionLinesActor);
		}
		col2->AddItem(impl->m_labelActor);
	}

	updateVisibilityWithoutRendering();
}

void GeoDataRiverSurvey::allActorsOff()
{
	impl->m_centerLineController.pointsActor()->VisibilityOff();
	impl->m_centerLineSplineController.linesActor()->VisibilityOff();
	impl->m_leftBankLineController.pointsActor()->VisibilityOff();
	impl->m_leftBankLineSplineController.linesActor()->VisibilityOff();
	impl->m_rightBankLineController.pointsActor()->VisibilityOff();
	impl->m_rightBankLineSplineController.linesActor()->VisibilityOff();
	impl->m_upstreamLineController.linesActor()->VisibilityOff();
	impl->m_downstreamLineController.linesActor()->VisibilityOff();
	impl->m_upstreamActor.actor()->VisibilityOff();
	impl->m_downstreamActor.actor()->VisibilityOff();

	impl->m_riverCenterPointsActor->VisibilityOff();
	impl->m_crossSectionLinesActor->VisibilityOff();
	impl->m_centerAndBankLinesActor->VisibilityOff();
	impl->m_selectedRiverCenterPointsActor->VisibilityOff();
	impl->m_selectedLeftBankPointsActor->VisibilityOff();
	impl->m_selectedRightBankPointsActor->VisibilityOff();
	impl->m_selectedCrossSectionLinesActor->VisibilityOff();
	impl->m_verticalCrossSectionLinesActor->VisibilityOff();
	impl->m_labelActor->VisibilityOff();
	impl->m_backgroundActor->VisibilityOff();
}

void GeoDataRiverSurvey::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	impl->updateMouseCursor(v);
}

void GeoDataRiverSurvey::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::Mode::CreateMode) {
		createModeKeyPressEvent(event, v);
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		editModeKeyPressEvent(event, v);
	}
}

void GeoDataRiverSurvey::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::Mode::CreateMode) {
		createModeKeyReleaseEvent(event, v);
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		editModeKeyReleaseEvent(event, v);
	}
}

void GeoDataRiverSurvey::mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::Mode::CreateMode) {
		createModeMouseDoubleClickEvent(event, v);
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		editModeMouseDoubleClickEvent(event, v);
	}
}

void GeoDataRiverSurvey::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::Mode::CreateMode) {
		createModeMouseMoveEvent(event, v);
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		editModeMouseMoveEvent(event, v);
	}
}

void GeoDataRiverSurvey::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::Mode::CreateMode) {
		createModeMousePressEvent(event, v);
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		editModeMousePressEvent(event, v);
	}
}

void GeoDataRiverSurvey::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::Mode::CreateMode) {
		createModeMouseReleaseEvent(event, v);
	} else if (impl->m_mode == Impl::Mode::EditMode) {
		editModeMouseReleaseEvent(event, v);
	}
}

void GeoDataRiverSurvey::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
}

void GeoDataRiverSurvey::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto elem = node.toElement();
	GeoData::doLoadFromProjectMainFile(node);
	auto mode = elem.attribute("mode");
	impl->m_mode = Impl::Mode::EditMode;
	if (mode == "create") {
		impl->m_mode = Impl::Mode::CreateMode;
	}
	m_setting.load(node);

	impl->m_backgroundActor->GetProperty()->SetOpacity(m_setting.opacity);

	int linearMode = elem.attribute("interpolateLinear").toInt();
	bool lMode = (linearMode != 0);
	RiverSplineSolver::setLinearMode(lMode, m_headPoint);
	if (lMode) {
		impl->m_interpolateSplineAction->setChecked(false);
		impl->m_interpolateLinearAction->setChecked(true);
	}
	impl->m_menuIsSetup = false;
	impl->updateMouseCursor(graphicsView());
}

void GeoDataRiverSurvey::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	QString mode = "create";
	if (impl->m_mode == Impl::Mode::EditMode) {
		mode = "edit";
	}
	writer.writeAttribute("mode", mode);
	m_setting.save(writer);

	int lMode = 0;
	if (RiverSplineSolver::linearMode()) {
		lMode = 1;
	}
	writer.writeAttribute("interpolateLinear", QString::number(static_cast<int>(lMode)));
}

void GeoDataRiverSurvey::loadExternalData(const QString& filename)
{
	if (projectData()->version().build() >= 3607) {
		if (impl->m_mode == Impl::Mode::EditMode) {
			iRICLib::RiverSurvey* rs = new iRICLib::RiverSurvey();

			rs->load(iRIC::toStr(filename).c_str());
			GeoDataRiverPathPoint* before = m_headPoint;
			GeoDataRiverPathPoint* newPoint;
			for (unsigned int i = 0; i < rs->points.size(); ++i) {
				iRICLib::RiverPathPoint* libp = rs->points.at(i);
				newPoint = new GeoDataRiverPathPoint();
				newPoint->loadFromiRICLibObject(libp);
				before->addPathPoint(newPoint);
				before = newPoint;
			}
			delete rs;
		} else if (impl->m_mode == Impl::Mode::CreateMode) {
			// implement this
			QFile f(filename);
			f.open(QIODevice::ReadOnly);
			QDataStream s(&f);

			PolyLineControllerUtil::loadPolyLine(&s, &(impl->m_centerLineController));
			PolyLineControllerUtil::loadPolyLine(&s, &(impl->m_leftBankLineController));
			PolyLineControllerUtil::loadPolyLine(&s, &(impl->m_rightBankLineController));
			f.close();

			if (impl->m_centerLineController.polyLine().size() > 0) {
				impl->m_createMouseEventMode = Impl::CreateMouseEventMode::Normal;
			} else {
				impl->m_createMouseEventMode = Impl::CreateMouseEventMode::BeforeDefining;
			}
			impl->updateLabelsAndSplines();
			impl->updateActionStatus();
		}
	} else {
		QFile f(filename);
		f.open(QIODevice::ReadOnly);
		QDataStream s(&f);

		GeoDataRiverPathPoint* before = m_headPoint;
		GeoDataRiverPathPoint* newPoint;
		bool nextExist;
		s >> nextExist;
		while (nextExist) {
			newPoint = new GeoDataRiverPathPoint();
			newPoint->load(s, projectData()->version());
			before->addPathPoint(newPoint);
			before = newPoint;
			s >> nextExist;
		}
		f.close();
	}
	updateInterpolators();
	informDeselection(nullptr);
}

void GeoDataRiverSurvey::saveExternalData(const QString& filename)
{
	if (impl->m_mode == Impl::Mode::EditMode) {
		iRICLib::RiverSurvey* rs = new iRICLib::RiverSurvey();
		bool first = true;

		iRICLib::RiverPathPoint* prevP = nullptr;
		GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
		while (p != nullptr) {
			iRICLib::RiverPathPoint* libp = new iRICLib::RiverPathPoint();
			p->saveToiRICLibObject(libp);
			if (first) {
				rs->firstPoint = libp;
				first = false;
			} else {
				prevP->nextPoint = libp;
			}
			prevP = libp;

			p = p->nextPoint();
		}
		rs->save(iRIC::toStr(filename).c_str());

		delete rs;
	} else if (impl->m_mode == Impl::Mode::CreateMode) {
		QFile f(filename);
		f.open(QIODevice::WriteOnly);
		QDataStream s(&f);

		PolyLineControllerUtil::savePolyLine(&s, impl->m_centerLineController);
		PolyLineControllerUtil::savePolyLine(&s, impl->m_leftBankLineController);
		PolyLineControllerUtil::savePolyLine(&s, impl->m_rightBankLineController);
		f.close();
	}
}

bool GeoDataRiverSurvey::getValueRange(double* min, double* max)
{
	double range[2];
	vtkDataArray* data = impl->m_backgroundGrid->GetPointData()->GetArray("Data");
	if (data == nullptr) {
		return false;
	}
	data->GetRange(range);
	if (range[0] > range[1]) {
		// min > max. It means that valid range can not be obtained from data.
		return false;
	}
	*min = range[0];
	*max = range[1];
	return true;
}

QDialog* GeoDataRiverSurvey::propertyDialog(QWidget*)
{
	return nullptr;
}

void GeoDataRiverSurvey::handlePropertyDialogAccepted(QDialog*)
{}

void GeoDataRiverSurvey::updateShapeData()
{
	impl->updateVtkPointsObjects();
	impl->updateVtkCenterAndBankLinesObjects();
	impl->updateVtkSelectedObjects();
	impl->updateVtkVerticalCenterLinesObjects();
	impl->updateVtkNameLabelObjects();
	impl->updateVtkBackgroundObjects();

	emit dataUpdated();
	impl->m_gridThread->update();
}

void GeoDataRiverSurvey::updateSelectionShapeData()
{
	impl->updateVtkSelectedObjects();
}

GeoDataRiverPathPoint* GeoDataRiverSurvey::headPoint() const
{
	return m_headPoint;
}

vtkStructuredGrid* GeoDataRiverSurvey::backgroundGrid() const
{
	return impl->m_backgroundGrid;
}

void GeoDataRiverSurvey::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void GeoDataRiverSurvey::assignActorZValues(const ZDepthRange& range)
{
	double background = range.min();
	double backlines = .7 * range.min() + .3 * range.max();
	double lines = .5 * range.min() + .5 * range.max();
	double points = range.max();

	impl->m_centerLineController.pointsActor()->SetPosition(0, 0, points);
	impl->m_leftBankLineController.pointsActor()->SetPosition(0, 0, points);
	impl->m_rightBankLineController.pointsActor()->SetPosition(0, 0, points);
	impl->m_centerLineSplineController.linesActor()->SetPosition(0, 0, lines);
	impl->m_leftBankLineSplineController.linesActor()->SetPosition(0, 0, lines);
	impl->m_rightBankLineSplineController.linesActor()->SetPosition(0, 0, lines);
	impl->m_upstreamLineController.linesActor()->SetPosition(0, 0, lines);
	impl->m_downstreamLineController.linesActor()->SetPosition(0, 0, lines);

	impl->m_riverCenterPointsActor->SetPosition(0, 0, points);
	impl->m_crossSectionLinesActor->SetPosition(0, 0, lines);
	impl->m_centerAndBankLinesActor->SetPosition(0, 0, lines);
	impl->m_selectedRiverCenterPointsActor->SetPosition(0, 0, points);
	impl->m_selectedLeftBankPointsActor->SetPosition(0, 0, points);
	impl->m_selectedRightBankPointsActor->SetPosition(0, 0, points);
	impl->m_selectedCrossSectionLinesActor->SetPosition(0, 0, lines);
	impl->m_verticalCrossSectionLinesActor->SetPosition(0, 0, backlines);
	impl->m_blackCrossSectionActor->SetPosition(0, 0, backlines);
	impl->m_backgroundActor->SetPosition(0, 0, background);
}

void GeoDataRiverSurvey::generateData()
{
	double length = PolyLineUtil::length(impl->m_centerLineSplineController.polyLine());
	std::vector<GeoDataPointmap*> pointMapDataList;
	std::vector<QString> pointMapDataNameList;

	const auto& childItems = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (parent()->parent())->childItems();
	for (int i = 0; i < childItems.size(); ++i) {
		auto c = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (childItems.at(i));
		auto geoData = c->geoData();
		auto pointMapData = dynamic_cast<GeoDataPointmap*> (geoData);
		if (pointMapData == nullptr) {continue;}

		pointMapDataList.push_back(pointMapData);
		pointMapDataNameList.push_back(pointMapData->caption());
	}

	if (pointMapDataList.size() == 0) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("No points data to map is found. Please import points data."));
		return;
	}

	GeoDataRiverSurveyGenerateDialog dialog(preProcessorWindow());
	dialog.setDEMDatas(pointMapDataNameList);
	dialog.setCenterLineLength(length);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	int numPoints = dialog.numberOfCrossSections();
	double divDist = dialog.divDistance();
	double upstreamName = dialog.upstreamName();
	GeoDataPointmap* mapData = pointMapDataList.at(dialog.demData());

	double nameDistance = std::floor(length / (numPoints - 1) / 100) / 10;
	if (nameDistance == 0) {nameDistance = 0.1;}

	auto centerLine = PolyLineUtil::buildSplinePoints(impl->m_centerLineController.polyData()->GetPoints(), numPoints - 1);
	auto leftLine = impl->m_leftBankLineSplineController.polyLine();
	auto rightLine = impl->m_rightBankLineSplineController.polyLine();
	QRectF rectL = PolyLineUtil::boundingRect(impl->m_leftBankLineController.polyLine());
	QRectF rectR = PolyLineUtil::boundingRect(impl->m_rightBankLineController.polyLine());
	QRectF rect = rectL.united(rectR);
	double maxLen = std::sqrt(rect.width() * rect.width() + rect.height() * rect.height());

	GeoDataRiverPathPoint* prev = m_headPoint;

	double nextName = upstreamName;
	for (int i = 0; i < centerLine.size(); ++i) {
		auto newPoint = new GeoDataRiverPathPoint();
		newPoint->InhibitInterpolatorUpdate = true;
		newPoint->setName(QString::number(nextName, 'f', 1));
		newPoint->setPosition(centerLine.at(i));

		QPointF dir;
		double distL, distR;
		if (i == 0) {
			QPointF p1 = centerLine.at(0);
			QPointF p2 = centerLine.at(1);
			dir = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(dir);
			distL = iRIC::distance(leftLine.at(0), centerLine.at(0));
			distR = iRIC::distance(rightLine.at(0), centerLine.at(0));
		} else if (i == centerLine.size() - 1) {
			QPointF p1 = centerLine.at(centerLine.size() - 2);
			QPointF p2 = centerLine.at(centerLine.size() - 1);
			dir = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(dir);
			distL = iRIC::distance(leftLine.at(leftLine.size() - 1), centerLine.at(centerLine.size() - 1));
			distR = iRIC::distance(rightLine.at(rightLine.size() - 1), centerLine.at(centerLine.size() - 1));
		} else {
			QPointF p1 = centerLine.at(i - 1);
			QPointF p2 = centerLine.at(i);
			QPointF p3 = centerLine.at(i + 1);
			QPointF v1 = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v1);
			QPointF v2 = iRIC::normalize(p3 - p2);
			iRIC::rotateVector270(v2);
			dir = iRIC::normalize(v1 + v2);

			QPointF nearestL = nearestInterSection(p2, p2 - dir * maxLen, leftLine);
			distL = iRIC::distance(p2, nearestL);
			QPointF nearestR = nearestInterSection(p2, p2 + dir * maxLen, rightLine);
			distR = iRIC::distance(p2, nearestR);
		}

		newPoint->setCrosssectionDirection(dir);
		std::map<double, GeoDataRiverCrosssection::Altitude> alts;

		const QPointF p;
		double pos;
		// center
		addPointAt(newPoint->position(), dir, 0, &alts, mapData);
		// left
		pos = - divDist;
		while (pos > - distL) {
			addPointAt(newPoint->position(), dir, pos, &alts, mapData);
			pos -= divDist;
		}
		addPointAt(newPoint->position(), dir, - distL, &alts, mapData);
		// right
		pos = divDist;
		while (pos < distR) {
			addPointAt(newPoint->position(), dir, pos, &alts, mapData);
			pos += divDist;
		}
		addPointAt(newPoint->position(), dir, distR, &alts, mapData);

		auto& altList = newPoint->crosssection().AltitudeInfo();
		for (const auto& pair : alts) {
			altList.push_back(pair.second);
		}
		newPoint->InhibitInterpolatorUpdate = false;

		prev->addPathPoint(newPoint);

		prev = newPoint;
		nextName -= nameDistance;
	}
	updateInterpolators();
	impl->m_mode = Impl::Mode::EditMode;
	impl->m_menuIsSetup = false;
	iRICUndoStack::instance().clear();

	informSelection(graphicsView());
	renderGraphicsView();

	InformationDialog::information(preProcessorWindow(), tr("Information"), tr("River survey data is generated using Points data.\n"
																																						 "If you want to adjust cross section position and map Points data again, you can use \"Map points data\""), "riversurvey_mapping_dem");
}

void GeoDataRiverSurvey::buildBankLines()
{
	auto centerLine = impl->m_centerLineController.polyLine();
	if (centerLine.size() < 2) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Center Line is not defined yet."));
		return;
	}

	GeoDataRiverSurveyBuildBankLinesDialog dialog(preProcessorWindow());
	dialog.setLeftBankDistance(impl->m_previousLeftBankDistance);
	dialog.setRightBankDistance(impl->m_previousRightBankDistance);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	double leftDistance = dialog.leftBankDistance();
	double rightDistance = dialog.rightBankDistance();

	std::vector<QPointF> leftBankLine, rightBankLine;

	for (int i = 0; i < centerLine.size(); ++i) {
		QPointF v;
		if (i == 0) {
			QPointF p1 = centerLine.at(0);
			QPointF p2 = centerLine.at(1);
			v = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v);
		} else if (i == centerLine.size() - 1) {
			QPointF p1 = centerLine.at(centerLine.size() - 2);
			QPointF p2 = centerLine.at(centerLine.size() - 1);
			v = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v);
		} else {
			QPointF p1 = centerLine.at(i - 1);
			QPointF p2 = centerLine.at(i);
			QPointF p3 = centerLine.at(i + 1);
			QPointF v1 = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v1);
			QPointF v2 = iRIC::normalize(p3 - p2);
			iRIC::rotateVector270(v2);
			v = iRIC::normalize(v1 + v2);
		}

		QPointF lb = centerLine.at(i);
		lb.setX(lb.x() - leftDistance * v.x());
		lb.setY(lb.y() - leftDistance * v.y());
		leftBankLine.push_back(lb);

		QPointF rb = centerLine.at(i);
		rb.setX(rb.x() + rightDistance * v.x());
		rb.setY(rb.y() + rightDistance * v.y());
		rightBankLine.push_back(rb);
	}

	impl->m_leftBankLineController.setPolyLine(leftBankLine);
	impl->m_rightBankLineController.setPolyLine(rightBankLine);

	impl->m_previousLeftBankDistance = leftDistance;
	impl->m_previousRightBankDistance = rightDistance;

	impl->createModeUpdateActionStatus();
	impl->updateLabelsAndSplines();

	renderGraphicsView();
	iRICUndoStack::instance().clear();
}

void GeoDataRiverSurvey::addVertexMode(bool on)
{
	if (on) {
		impl->m_createMouseEventMode = Impl::CreateMouseEventMode::AddVertexNotPossible;
	} else {
		impl->m_createMouseEventMode = Impl::CreateMouseEventMode::Normal;
	}
	impl->createModeUpdateActionStatus();
}

void GeoDataRiverSurvey::removeVertexMode(bool on)
{
	if (on) {
		impl->m_createMouseEventMode = Impl::CreateMouseEventMode::RemoveVertexNotPossible;
	} else {
		impl->m_createMouseEventMode = Impl::CreateMouseEventMode::Normal;
	}
	impl->createModeUpdateActionStatus();
}

void GeoDataRiverSurvey::importCenterLine()
{
	impl->importLine(&impl->m_centerLineController);
}

void GeoDataRiverSurvey::exportCenterLine()
{
	impl->exportLine(&impl->m_centerLineController, tr("Center line"));
}

void GeoDataRiverSurvey::moveSelectedPoints()
{
	GeoDataRiverPathPointMoveDialog* dialog = new GeoDataRiverPathPointMoveDialog(this, preProcessorWindow());
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::TranslateDialog;
	int selectCount = m_headPoint->selectedPoints();
	bool singleSelection = (selectCount == 1);
	GeoDataRiverPathPoint* selected = nullptr;
	if (singleSelection) {
		selected = singleSelectedPoint();
	}
	dialog->setSingleSelection(singleSelection);
	if (singleSelection) {
		dialog->setCurrentCenter(selected->position());
	}
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::deleteSelectedPoints()
{
	int num = 0;
	int selectedNum = 0;
	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	while (p != nullptr) {
		num++;
		if (p->IsSelected) {
			selectedNum++;
		}
		p = p->nextPoint();
	}

	if (num - selectedNum < 2) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("River survey data need at least 2 center points."), QMessageBox::Ok, QMessageBox::Ok);
	} else {
		iRICUndoStack::instance().push(new DeleteRiverPathPointCommand(this));
	}
}

void GeoDataRiverSurvey::shiftSelectedPoints()
{
	GeoDataRiverPathPointShiftDialog* dialog = new GeoDataRiverPathPointShiftDialog(this, preProcessorWindow());
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::ShiftDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::expandSelectedPoints()
{
	GeoDataRiverPathPointExpandDialog* dialog = new GeoDataRiverPathPointExpandDialog(this, preProcessorWindow());
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::ExpansionDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::rotateSelectedPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	QPointF dir = selected->previousPoint()->position() - selected->position();
	if (selected->previousPoint()->firstPoint() && selected->nextPoint() != nullptr) {
		dir = selected->position() - selected->nextPoint()->position();
	}
	double angle = iRIC::angle(dir, selected->crosssectionDirection());

	GeoDataRiverPathPointRotateDialog* dialog = new GeoDataRiverPathPointRotateDialog(this, preProcessorWindow());
	dialog->setCurrentRelativeAngle(angle);
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotateDialog;
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::renameSelectedPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointRenameDialog dialog(selected, this, preProcessorWindow());
	dialog.exec();
}

void GeoDataRiverSurvey::addLeftExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointExtensionAddDialog* dialog = new GeoDataRiverPathPointExtensionAddDialog(selected, this, preProcessorWindow());
	dialog->setLineMode(GeoDataRiverPathPointExtensionAddDialog::Left);
	dialog->setPoint(selected->crosssectionPosition(selected->crosssection().leftBank(true).position()));
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::AddingExtension;
	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::addRightExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointExtensionAddDialog* dialog = new GeoDataRiverPathPointExtensionAddDialog(selected, this, preProcessorWindow());
	dialog->setLineMode(GeoDataRiverPathPointExtensionAddDialog::Right);
	dialog->setPoint(selected->crosssectionPosition(selected->crosssection().rightBank(true).position()));
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::AddingExtension;
	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::removeLeftExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	iRICUndoStack::instance().push(new RemoveExtensionCommand(true, selected->leftBank()->interpolate(0), selected, this));
}

void GeoDataRiverSurvey::removeRightExtensionPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	iRICUndoStack::instance().push(new RemoveExtensionCommand(false, selected->rightBank()->interpolate(0), selected, this));
}

void GeoDataRiverSurvey::restoreMouseEventMode()
{
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::Normal;
}

void GeoDataRiverSurvey::insertNewPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointInsertDialog* dialog = new GeoDataRiverPathPointInsertDialog(selected, true, this, preProcessorWindow());
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::Inserting;

	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::addNewPoint()
{
	GeoDataRiverPathPoint* selected = singleSelectedPoint();
	GeoDataRiverPathPointInsertDialog* dialog = new GeoDataRiverPathPointInsertDialog(selected, false, this, preProcessorWindow());
	dataModel()->iricMainWindow()->enterModelessDialogMode();
	impl->m_editMouseEventMode = Impl::EditMouseEventMode::Inserting;

	connect(graphicsView(), SIGNAL(worldPositionChanged(QPointF)), dialog, SLOT(setPoint(QPointF)));
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GeoDataRiverSurvey::createModeKeyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (! iRIC::isEnterKey(event->key())) {return;}
	if (impl->m_createMouseEventMode != Impl::CreateMouseEventMode::Defining) {return;}

	finishDefiningLine();
}

void GeoDataRiverSurvey::createModeKeyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{}

void GeoDataRiverSurvey::createModeMouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_createMouseEventMode == Impl::CreateMouseEventMode::Defining) {
		finishDefiningLine();
	}
}

void GeoDataRiverSurvey::createModeMouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_createMouseEventMode) {
	case Impl::CreateMouseEventMode::Normal:
	case Impl::CreateMouseEventMode::MoveVertexPrepare:
	case Impl::CreateMouseEventMode::AddVertexPrepare:
	case Impl::CreateMouseEventMode::AddVertexNotPossible:
	case Impl::CreateMouseEventMode::RemoveVertexPrepare:
	case Impl::CreateMouseEventMode::RemoveVertexNotPossible:
		impl->createModeUpdateMouseEventMode(event->pos());
		impl->createModeUpdateMouseCursor(v);
		break;
	case Impl::CreateMouseEventMode::Defining:
		pushUpdateLabelsCommand(new PolyLinePushVertexCommand(false, v->viewportToWorld(event->pos()), impl->m_activePoints));
		break;
	case Impl::CreateMouseEventMode::MoveVertex:
		pushUpdateLabelsCommand(new PolyLineMoveVertexCommand(false, impl->m_selectedVertexId, v->viewportToWorld(event->pos()), impl->m_activePoints));
		break;
	case Impl::CreateMouseEventMode::AddVertex:
		 pushUpdateLabelsCommand(new PolyLineAddVertexCommand(false, impl->m_selectedEdgeId, v->viewportToWorld(event->pos()), impl->m_activePoints));
		break;

	default:
		break;
	}
	impl->m_currentPoint = event->pos();
}

void GeoDataRiverSurvey::createModeMousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_createMouseEventMode) {
		case Impl::CreateMouseEventMode::BeforeDefining:
			impl->m_createMouseEventMode = Impl::CreateMouseEventMode::Defining;
			pushUpdateLabelsCommand(new PolyLinePushVertexCommand(true, v->viewportToWorld(event->pos()), impl->m_activePoints));
			break;
		case Impl::CreateMouseEventMode::Defining:
			pushUpdateLabelsCommand(new PolyLinePushVertexCommand(true, v->viewportToWorld(event->pos()), impl->m_activePoints));
			break;
		case Impl::CreateMouseEventMode::MoveVertexPrepare:
			impl->m_createMouseEventMode = Impl::CreateMouseEventMode::MoveVertex;
			pushUpdateLabelsCommand(new PolyLineMoveVertexCommand(true, impl->m_selectedVertexId, v->viewportToWorld(event->pos()), impl->m_activePoints));
			break;
		case Impl::CreateMouseEventMode::AddVertexPrepare:
			impl->m_createMouseEventMode = Impl::CreateMouseEventMode::AddVertex;
			pushUpdateLabelsCommand(new PolyLineAddVertexCommand(true, impl->m_selectedEdgeId, v->viewportToWorld(event->pos()), impl->m_activePoints));
			break;
		case Impl::CreateMouseEventMode::RemoveVertexPrepare:
			impl->m_createMouseEventMode = Impl::CreateMouseEventMode::Normal;
			pushUpdateLabelsCommand(new PolyLineRemoveVertexCommand(impl->m_selectedVertexId, impl->m_activePoints));
			break;
		default:
			break;
		}
		impl->createModeUpdateMouseCursor(v);
		impl->updateActionStatus();
	}

	impl->m_currentPoint = event->pos();
}

void GeoDataRiverSurvey::createModeMouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		impl->createModeUpdateMouseEventMode(event->pos());
		impl->createModeUpdateMouseCursor(v);
		impl->updateActionStatus();
	} else if (event->button() == Qt::RightButton) {
		if (impl->m_createMouseEventMode == Impl::CreateMouseEventMode::EditCoodinatesDialog) {return;}

		if (iRIC::isNear(impl->m_currentPoint, event->pos())) {
			impl->setupMenu();
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GeoDataRiverSurvey::editModeKeyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_editMouseEventMode) {
	case Impl::EditMouseEventMode::Normal:
	case Impl::EditMouseEventMode::TranslatePrepare:
	case Impl::EditMouseEventMode::RotatePrepareRight:
	case Impl::EditMouseEventMode::RotatePrepareLeft:
	case Impl::EditMouseEventMode::ShiftPrepare:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareLeft:
		impl->m_keyboardModifiers = event->modifiers();
		impl->editModeUpdateMouseEventMode();
		impl->editModeUpdateMouseCursor(v);
		break;

	default:
		break;
	}
}

void GeoDataRiverSurvey::editModeKeyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_editMouseEventMode) {
	case Impl::EditMouseEventMode::Normal:
	case Impl::EditMouseEventMode::TranslatePrepare:
	case Impl::EditMouseEventMode::RotatePrepareRight:
	case Impl::EditMouseEventMode::RotatePrepareLeft:
	case Impl::EditMouseEventMode::ShiftPrepare:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
	case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareRight:
	case Impl::EditMouseEventMode::ExpansionPrepareLeft:
		impl->m_keyboardModifiers = event->modifiers();
		impl->editModeUpdateMouseEventMode();
		impl->editModeUpdateMouseCursor(v);
		break;

	default:
		break;
	}
}

void GeoDataRiverSurvey::editModeMouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	GeoDataRiverPathPoint* selP = singleSelectedPoint();
	if (selP == nullptr) {
		// no point is selected.
		return;
	}
	openCrossSectionWindow();
}

void GeoDataRiverSurvey::editModeMouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if ((impl->m_editMouseEventMode == Impl::EditMouseEventMode::AddingExtension || impl->m_editMouseEventMode == Impl::EditMouseEventMode::Inserting) && impl->m_leftButtonDown) {
		graphicsView()->emitWorldPosition(event->x(), event->y());
	} else {
		if (impl->m_definingBoundingBox) {
			// drawing bounding box using mouse dragging.
			MouseBoundingBox* box = dataModel()->mouseBoundingBox();
			box->setEndPoint(event->x(), event->y());
			renderGraphicsView();
		} else {
			switch (impl->m_editMouseEventMode) {
			case Impl::EditMouseEventMode::Normal:
			case Impl::EditMouseEventMode::TranslatePrepare:
			case Impl::EditMouseEventMode::RotatePrepareRight:
			case Impl::EditMouseEventMode::RotatePrepareLeft:
			case Impl::EditMouseEventMode::ShiftPrepare:
			case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
			case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
			case Impl::EditMouseEventMode::ExpansionPrepareRight:
			case Impl::EditMouseEventMode::ExpansionPrepareLeft:
				impl->m_currentPoint = event->pos();
				impl->editModeUpdateMouseEventMode();
				impl->editModeUpdateMouseCursor(v);
				break;
			case Impl::EditMouseEventMode::Translate:
				// execute translation.
				iRICUndoStack::instance().push(new TranslateRiverPathPointCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::RotateRight:
				iRICUndoStack::instance().push(new MouseRotateRiverCrosssectionCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::RotateLeft:
				iRICUndoStack::instance().push(new MouseRotateRiverCrosssectionCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::Shift:
				iRICUndoStack::instance().push(new MouseShiftRiverPathCenterCommand(impl->m_currentPoint, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
				iRICUndoStack::instance().push(new MouseMoveExtensionCommand(true, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
				iRICUndoStack::instance().push(new MouseMoveExtensionCommand(false, event->pos(), this));
				impl->m_currentPoint = event->pos();
				break;
			case Impl::EditMouseEventMode::ExpansionRight:
			case Impl::EditMouseEventMode::ExpansionLeft:
				// not used.
				break;
			case Impl::EditMouseEventMode::AddingExtension:
			case Impl::EditMouseEventMode::Inserting:
				if (impl->m_leftButtonDown) {
					graphicsView()->emitWorldPosition(event->x(), event->y());
				}
				break;

			default:
				break;
			}
		}
	}
}

void GeoDataRiverSurvey::editModeMousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_editMouseEventMode) {
		case Impl::EditMouseEventMode::Normal: {
				// start drawing the mouse bounding box.
				impl->m_definingBoundingBox = true;
				MouseBoundingBox* box = dataModel()->mouseBoundingBox();
				box->setStartPoint(event->x(), event->y());
				box->enable();
				v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
				renderGraphicsView();
			}
			break;
		case Impl::EditMouseEventMode::TranslatePrepare:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::Translate;
			break;
		case Impl::EditMouseEventMode::RotatePrepareRight:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotateRight;
			break;
		case Impl::EditMouseEventMode::RotatePrepareLeft:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::RotateLeft;
			break;
		case Impl::EditMouseEventMode::ShiftPrepare:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::Shift;
			break;
		case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareLeft:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::MoveExtentionEndPointLeft;
			break;
		case Impl::EditMouseEventMode::MoveExtensionEndPointPrepareRight:
			impl->m_editMouseEventMode = Impl::EditMouseEventMode::MoveExtentionEndPointRight;
			break;
		case Impl::EditMouseEventMode::ExpansionPrepareRight:
		case Impl::EditMouseEventMode::ExpansionPrepareLeft:
			// do nothing. mouse expansion is not used.
			break;
		case Impl::EditMouseEventMode::Translate:
		case Impl::EditMouseEventMode::RotateRight:
		case Impl::EditMouseEventMode::RotateLeft:
		case Impl::EditMouseEventMode::Shift:
		case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
		case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
		case Impl::EditMouseEventMode::ExpansionRight:
		case Impl::EditMouseEventMode::ExpansionLeft:
			// these does not happen.
			break;
		case Impl::EditMouseEventMode::AddingExtension:
		case Impl::EditMouseEventMode::Inserting:
			impl->m_leftButtonDown = true;
			graphicsView()->emitWorldPosition(event->x(), event->y());
			break;

		default:
			break;
		}
	}
	impl->m_dragStartPoint = event->pos();
}

void GeoDataRiverSurvey::editModeMouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_editMouseEventMode) {
		case Impl::EditMouseEventMode::Normal:
			if (impl->m_definingBoundingBox) {
				// bounding box selecting ended.
				MouseBoundingBox* box = dataModel()->mouseBoundingBox();
				box->setEndPoint(event->x(), event->y());
				box->disable();
				if (iRIC::isNear(box->startPoint(), box->endPoint())) {
					int x = (box->startPoint().x() + box->endPoint().x()) / 2;
					int y = (box->startPoint().y() + box->endPoint().y()) / 2;
					box->setStartPoint(x - 8, y - 8);
					box->setEndPoint(x + 8, y + 8);
				}

				v->restoreUpdateRate();

				// selection change is made not undo-able
				ChangeSelectionCommand com(this, box);
				com.redo();
			}
			impl->m_definingBoundingBox = false;
			impl->m_currentPoint = event->pos();
			impl->editModeUpdateMouseEventMode();
			impl->editModeUpdateMouseCursor(v);
			break;
		case Impl::EditMouseEventMode::Translate:
		case Impl::EditMouseEventMode::RotateRight:
		case Impl::EditMouseEventMode::RotateLeft:
		case Impl::EditMouseEventMode::Shift:
		case Impl::EditMouseEventMode::MoveExtentionEndPointLeft:
		case Impl::EditMouseEventMode::MoveExtentionEndPointRight:
		case Impl::EditMouseEventMode::ExpansionRight:
		case Impl::EditMouseEventMode::ExpansionLeft:
			// operation ended.
			impl->m_currentPoint = event->pos();
			impl->editModeUpdateMouseEventMode();
			impl->editModeUpdateMouseCursor(v);
			break;
		case Impl::EditMouseEventMode::AddingExtension:
		case Impl::EditMouseEventMode::Inserting:
			impl->m_leftButtonDown = false;
			break;

		default:
			break;
		}
	} else if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(impl->m_dragStartPoint, event->pos())) {
			impl->setupMenu();
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GeoDataRiverSurvey::pushUpdateLabelsCommand(QUndoCommand* com, bool renderRedoOnly)
{
	if (renderRedoOnly) {
		pushRenderRedoOnlyCommand(new PolyLineUpdateLabelsCommand(com, this));
	} else {
		pushRenderCommand(new PolyLineUpdateLabelsCommand(com, this));
	}
}

void GeoDataRiverSurvey::finishDefiningLine()
{
	if (impl->m_activeLine->polyLine().size() < 2) {return;}

	iRICUndoStack::instance().undo();
	pushCommand(new PolyLineFinishDefiningCommand(this));
}

GeoDataRiverPathPoint* GeoDataRiverSurvey::singleSelectedPoint()
{
	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {return p;}
		p = p->nextPoint();
	}
	return nullptr;
}

void GeoDataRiverSurvey::updateSplineSolvers()
{
	m_CenterLineSolver.update();
	m_LeftBankSolver.update();
	m_RightBankSolver.update();
}

void GeoDataRiverSurvey::updateBackgroundGrid()
{
	impl->m_backgroundGrid = impl->m_gridThread->grid();
	impl->m_backgroundGrid->GetPointData()->SetActiveScalars("Data");

	GeoDataRiverPathPoint* p = m_headPoint->nextPoint();
	impl->m_gridThread->startBGGridCopy();
	while (p != nullptr) {
		vtkPointSet* grid = impl->m_gridThread->partialGrid(p);
		if (grid != nullptr) {
			p->areaGrid()->DeepCopy(grid);
		}
		p = p->nextPoint();
	}
	impl->m_gridThread->finishBGGridCopy();
	vtkDataSetMapper* mapper = vtkDataSetMapper::SafeDownCast(impl->m_backgroundActor->GetMapper());
	mapper->SetInputData(impl->m_backgroundGrid);
	if (isVisible() && m_setting.showBackground) {
		impl->m_backgroundActor->VisibilityOn();
	}
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->informValueRangeChange();
}

void GeoDataRiverSurvey::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GeoDataRiverSurvey::tr("Information"), GeoDataRiverSurvey::tr("Please define center line by mouse-clicking. Finish definining by double clicking, or pressing return key."), "geodatariversurveyinit");
}

void GeoDataRiverSurvey::updateInterpolators()
{
	// update interpolators
	m_CenterLineSolver.setHeadPoint(m_headPoint);
	m_CenterLineSolver.update();
	m_LeftBankSolver.setHeadPoint(m_headPoint);
	m_LeftBankSolver.update();
	m_RightBankSolver.setHeadPoint(m_headPoint);
	m_RightBankSolver.update();
	m_headPoint->UpdateCtrlSections();

	// update interpolators for altitude interpolation on crosssections.
	GeoDataRiverPathPoint* tmpp = m_headPoint->nextPoint();
	while (tmpp != nullptr) {
		tmpp->updateXSecInterpolators();
		tmpp = tmpp->nextPoint();
	}
	updateShapeData();
}

void GeoDataRiverSurvey::openCrossSectionWindow()
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->openCrossSectionWindow(this, singleSelectedPoint()->name());
}

void GeoDataRiverSurvey::updateCrosssectionWindows()
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->updateCrossectionWindows();
}

void GeoDataRiverSurvey::toggleCrosssectionWindowsGridCreatingMode(bool gridMode)
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->toggleCrosssectionWindowsGridCreatingMode(gridMode, this);
}

void GeoDataRiverSurvey::informCtrlPointUpdateToCrosssectionWindows()
{
	PreProcessorGeoDataGroupDataItemInterface* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	gItem->informCtrlPointUpdateToCrosssectionWindows();
}

void GeoDataRiverSurvey::displaySetting()
{
	GeoDataRiverSurveyDisplaySettingDialog dialog(preProcessorWindow());
	dialog.setSetting(m_setting);

	int ret = dialog.exec();
	if (ret != dialog.Accepted) {return;}

	pushRenderCommand(new SetDisplaySettingCommand(dialog.setting(), this));
}

void GeoDataRiverSurvey::switchInterpolateModeToLinear()
{
	RiverSplineSolver::setLinearMode(true, m_headPoint);
	impl->m_interpolateLinearAction->setChecked(true);
	impl->m_interpolateSplineAction->setChecked(false);
	updateInterpolators();
}

void GeoDataRiverSurvey::switchInterpolateModeToSpline()
{
	RiverSplineSolver::setLinearMode(false, m_headPoint);
	impl->m_interpolateLinearAction->setChecked(false);
	impl->m_interpolateSplineAction->setChecked(true);
	updateInterpolators();
}

void GeoDataRiverSurvey::mapPointsData()
{
	std::vector<GeoDataPointmap*> pointMapDataList;
	std::vector<QString> pointMapDataNameList;

	const auto& childItems = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (parent()->parent())->childItems();
	for (int i = 0; i < childItems.size(); ++i) {
		auto c = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (childItems.at(i));
		auto geoData = c->geoData();
		auto pointMapData = dynamic_cast<GeoDataPointmap*> (geoData);
		if (pointMapData == nullptr) {continue;}

		pointMapDataList.push_back(pointMapData);
		pointMapDataNameList.push_back(pointMapData->caption());
	}

	if (pointMapDataList.size() == 0) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("No points data to map is found. Please import points data."));
		return;
	}

	GeoDataRiverSurveyMapPointsDialog dialog(preProcessorWindow());
	dialog.setDEMDatas(pointMapDataNameList);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	double divDist = dialog.divDistance();
	GeoDataPointmap* mapData = pointMapDataList.at(dialog.demData());

	auto p = m_headPoint;
	p = p->nextPoint();
	while (p != nullptr) {
		double left = p->crosssection().leftBank(true).position();
		double right = p->crosssection().rightBank(true).position();

		std::map<double, GeoDataRiverCrosssection::Altitude> alts;
		double pos;

		// center
		addPointAt(p->position(), p->crosssectionDirection(), 0, &alts, mapData);
		// left
		pos = - divDist;
		while (pos > left) {
			addPointAt(p->position(), p->crosssectionDirection(), pos, &alts, mapData);
			pos -= divDist;
		}
		addPointAt(p->position(), p->crosssectionDirection(), left, &alts, mapData);
		// right
		pos = divDist;
		while (pos < right) {
			addPointAt(p->position(), p->crosssectionDirection(), pos, &alts, mapData);
			pos += divDist;
		}
		addPointAt(p->position(), p->crosssectionDirection(), right, &alts, mapData);

		auto& altList = p->crosssection().AltitudeInfo();
		altList.clear();
		for (const auto& pair : alts) {
			altList.push_back(pair.second);
		}
		p = p->nextPoint();
	}
	updateInterpolators();

	iRICUndoStack::instance().clear();
}

void GeoDataRiverSurvey::setColoredPoints(GeoDataRiverPathPoint* black)
{
	if (black == nullptr) {
		impl->m_blackCrossSectionActor->VisibilityOff();
	} else {
		setupLine(impl->m_blackCrossSection, black);
		impl->m_blackCrossSectionActor->VisibilityOn();
	}
	renderGraphicsView();
}

void GeoDataRiverSurvey::setGridCreatingCondition(GridCreatingConditionRiverSurveyInterface* cond)
{
	impl->m_gridCreatingCondition = cond;
}

GridCreatingConditionRiverSurveyInterface* GeoDataRiverSurvey::gridCreatingCondition() const
{
	return impl->m_gridCreatingCondition;
}

void GeoDataRiverSurvey::setupLine(vtkPolyData* polyData, GeoDataRiverPathPoint* p)
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto cells = vtkSmartPointer<vtkCellArray>::New();

	// left bank
	auto point = p->crosssectionPosition(p->crosssection().leftBank(true).position());
	points->InsertNextPoint(point.x(), point.y(), 0);

	// left fixed point
	if (p->crosssection().fixedPointLSet()) {
		point = p->crosssectionPosition(p->crosssection().fixedPointL().position());
	} else {
		// use left bank.
		point = p->crosssectionPosition(p->crosssection().leftBank(true).position());
	}
	points->InsertNextPoint(point.x(), point.y(), 0);

	// river center
	point = p->position();
	points->InsertNextPoint(point.x(), point.y(), 0);

	// right fixed point
	if (p->crosssection().fixedPointRSet()) {
		point = p->crosssectionPosition(p->crosssection().fixedPointR().position());
	} else {
		// use right bank.
		point = p->crosssectionPosition(p->crosssection().rightBank(true).position());
	}
	points->InsertNextPoint(point.x(), point.y(), 0);

	// right bank
	point = p->crosssectionPosition(p->crosssection().rightBank(true).position());
	points->InsertNextPoint(point.x(), point.y(), 0);

	vtkIdType ids[5];
	for (vtkIdType i = 0; i < 5; ++i) {
		ids[i] = i;
	}
	cells->InsertNextCell(5, ids);

	polyData->Initialize();
	polyData->SetPoints(points);
	polyData->SetLines(cells);
}

void GeoDataRiverSurvey::useDivisionPointsForBackgroundGrid(bool use)
{
	impl->m_gridThread->setUseDivisionPoints(use);
	impl->m_gridThread->update();
}

void GeoDataRiverSurvey::refreshBackgroundGrid()
{
	impl->m_gridThread->update();
}

void GeoDataRiverSurvey::cancelBackgroundGridUpdate()
{
	impl->m_gridThread->cancel();
}

GeoDataProxy* GeoDataRiverSurvey::getProxy()
{
	return new GeoDataRiverSurveyProxy(this);
}

void GeoDataRiverSurvey::updateFilename()
{
	setFilename(name().append(".dat"));
}

int GeoDataRiverSurvey::iRICLibType() const
{
	return IRIC_GEO_RIVERSURVEY;
}

void GeoDataRiverSurvey::doApplyOffset(double x, double y)
{
	PolyLineControllerUtil::applyOffset(&(impl->m_centerLineController), x, y);
	PolyLineControllerUtil::applyOffset(&(impl->m_leftBankLineController), x, y);
	PolyLineControllerUtil::applyOffset(&(impl->m_rightBankLineController), x, y);

	GeoDataRiverPathPoint* p = this->m_headPoint->nextPoint();
	while (p != nullptr) {
		QPointF v = p->position();
		v.setX(v.x() - x);
		v.setY(v.y() - y);
		p->setPosition(v);
		p = p->nextPoint();
	}
	impl->updateLabelsAndSplines();

	headPoint()->updateRiverShapeInterpolators();
	updateShapeData();
}
