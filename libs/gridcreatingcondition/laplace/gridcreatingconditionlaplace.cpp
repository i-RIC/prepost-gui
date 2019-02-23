#include "gridcreatingconditionlaplace.h"
#include "gridcreatingconditionlaplacectrlpointsdialog.h"
#include "gridcreatingconditionlaplacedeploysettingdialog.h"
#include "gridcreatingconditionlaplacedivisionsettingdialog.h"
#include "gridcreatingconditionlaplacewholeregiondivisionsettingdialog.h"
#include "private/gridcreatingconditionlaplace_centerlinecoordinateseditor.h"
#include "private/gridcreatingconditionlaplace_edgecoordinateseditor.h"
#include "private/gridcreatingconditionlaplace_impl.h"

#include <geodata/riversurvey/geodatariverpathpoint.h>
#include <geodata/riversurvey/geodatariversurvey.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>

#include <misc/informationdialog.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>

#include <QLocale>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>

namespace {

GeoDataRiverSurvey* findRiverSurveyData(GridCreatingCondition* cond)
{
	auto gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(cond->parent()->parent()->parent());
	auto rtItem = gtItem->geoDataTop();

	for (auto gItem : rtItem->groupDataItems()) {
		auto rItems = gItem->geoDatas();
		for (auto rItem : rItems) {
			auto riverSurvey = dynamic_cast<GeoDataRiverSurvey*>(rItem->geoData());
			if (riverSurvey != nullptr) {
				// this is a river survey data!
				return riverSurvey;
			}
		}
	}
	return nullptr;
}

void loadPoints(QDataStream* stream, std::vector<QPointF>* points, const QPointF& offset)
{
	int size;
	*stream >> size;
	for (int i = 0; i < size; ++i) {
		qreal x, y;
		*stream >> x >> y;
		points->push_back(QPointF(x - offset.x(), y - offset.y()));
	}
}

void savePoints(QDataStream* stream, const std::vector<QPointF>& points, const QPointF& offset)
{
	int size = static_cast<int>(points.size());
	*stream << size;
	for (const QPointF& p : points) {
		*stream << p.x() + offset.x() << p.y() + offset.y();
	}
}

void loadPolyLine(QDataStream* stream, PolyLineController* polyLine, const QPointF& offset)
{
	std::vector<QPointF> line;
	loadPoints(stream, &line, offset);
	polyLine->setPolyLine(line);
}

void savePolyLine(QDataStream* stream, const PolyLineController& polyLine, const QPointF& offset)
{
	savePoints(stream, polyLine.polyLine(), offset);
}

} // namespace

GridCreatingConditionLaplace::GridCreatingConditionLaplace(ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition(parent, creator),
	impl {new Impl {this}}
{

}

GridCreatingConditionLaplace::~GridCreatingConditionLaplace()
{
	delete impl;
}

bool GridCreatingConditionLaplace::init()
{
	return true;
}

void GridCreatingConditionLaplace::showInitialDialog()
{
	impl->m_editMode = Impl::EditMode::CenterLineOnly;
	auto rs = findRiverSurveyData(this);
	if (rs == nullptr) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("River Survey data not found. Please define Center Line by yourself."));
		impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::BeforeDefining;
		impl->updateMouseCursor(graphicsView());
		impl->updateActionStatus();
		return;
	}

	int numPoints = Impl::getNumPoints(rs);

	GridCreatingConditionLaplaceCtrlPointsDialog dialog(preProcessorWindow());
	dialog.setLocale(iricMainWindow()->locale());
	dialog.setMaximum(numPoints);
	int ret = dialog.exec();
	impl->copyCenterLine(rs, dialog.value());

	impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
	impl->updateMouseCursor(graphicsView());
	impl->updateActionStatus();
}

bool GridCreatingConditionLaplace::create(QWidget* parent)
{
	Grid* grid = impl->createGrid();
	emit gridCreated(grid);

	return true;
}

bool GridCreatingConditionLaplace::ready() const
{
	return true;
}

void GridCreatingConditionLaplace::clear()
{

}

void GridCreatingConditionLaplace::setupActors()
{

}

void GridCreatingConditionLaplace::setupMenu()
{
	m_menu->clear();
	impl->m_rightClickingMenu->clear();

	auto m = impl->m_rightClickingMenu;

	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		m_menu->addAction(impl->m_buildBankLinesAction);
		m_menu->addSeparator();
		m_menu->addAction(impl->m_addVertexAction);
		m_menu->addAction(impl->m_removeVertexAction);
		m_menu->addAction(impl->m_editCoordinatesAction);

		m->addAction(impl->m_buildBankLinesAction);
		m->addSeparator();
		m->addAction(impl->m_addVertexAction);
		m->addAction(impl->m_removeVertexAction);
		m->addAction(impl->m_editCoordinatesAction);
	} else if (impl->m_editMode == Impl::EditMode::RegionDefined) {
		m_menu->addAction(m_conditionDataItem->createAction());
		m_menu->addSeparator();
		m_menu->addAction(impl->m_buildBankLinesAction);
		m_menu->addSeparator();
		m_menu->addMenu(impl->m_modeMenu);
		m_menu->addSeparator();
		m_menu->addAction(impl->m_addNewEdgeAction);
		m_menu->addAction(impl->m_joinRegionsAction);
		m_menu->addSeparator();
		m_menu->addMenu(impl->m_interpolateMenu);
		m_menu->addSeparator();
		m_menu->addAction(impl->m_addVertexAction);
		m_menu->addAction(impl->m_removeVertexAction);
		m_menu->addAction(impl->m_editCoordinatesAction);

		m->addAction(m_conditionDataItem->createAction());
		m->addSeparator();
		m->addAction(impl->m_buildBankLinesAction);
		m->addSeparator();
		m->addMenu(impl->m_modeMenu);
		m->addSeparator();
		m->addAction(impl->m_addNewEdgeAction);
		m->addAction(impl->m_joinRegionsAction);
		m->addSeparator();
		m->addMenu(impl->m_interpolateMenu);
		m->addSeparator();
		m->addAction(impl->m_addVertexAction);
		m->addAction(impl->m_removeVertexAction);
		m->addAction(impl->m_editCoordinatesAction);
	} else {
		m_menu->addAction(m_conditionDataItem->createAction());
		m_menu->addSeparator();
		m_menu->addMenu(impl->m_modeMenu);
		m_menu->addSeparator();
		m_menu->addAction(impl->m_wholeRegionDivisionSettingAction);
		m_menu->addAction(impl->m_divisionSettingAction);
		m_menu->addAction(impl->m_deploySettingAction);

		m->addAction(m_conditionDataItem->createAction());
		m->addSeparator();
		m->addMenu(impl->m_modeMenu);
		m->addSeparator();
		m->addAction(impl->m_wholeRegionDivisionSettingAction);
		m->addAction(impl->m_divisionSettingAction);
		m->addAction(impl->m_deploySettingAction);
	}
	m_menu->addSeparator();
	m_menu->addAction(m_conditionDataItem->importAction());
	m_menu->addAction(m_conditionDataItem->exportAction());

	m->addSeparator();
	m->addAction(m_conditionDataItem->importAction());
	m->addAction(m_conditionDataItem->exportAction());
}

void GridCreatingConditionLaplace::informSelection(PreProcessorGraphicsViewInterface* v)
{
	impl->m_itemSelected = true;
	impl->updateActorSetting();
	renderGraphicsView();
}

void GridCreatingConditionLaplace::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	impl->m_itemSelected = false;
	impl->updateActorSetting();
	renderGraphicsView();
}

void GridCreatingConditionLaplace::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	impl->updateMouseCursor(v);
}

void GridCreatingConditionLaplace::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		if (! iRIC::isEnterKey(event->key())) {return;}
		if (impl->m_centerLineOnlyMouseEventMode != Impl::CenterLineOnlyMouseEventMode::Defining) {return;}
		impl->pushCenterLineFinishDefinitionCommand();
	}
}

void GridCreatingConditionLaplace::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{}

void GridCreatingConditionLaplace::mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		if (impl->m_centerLineOnlyMouseEventMode == Impl::CenterLineOnlyMouseEventMode::Defining) {
			impl->pushCenterLineFinishDefinitionCommand();
		}
	}
}

void GridCreatingConditionLaplace::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		switch (impl->m_centerLineOnlyMouseEventMode) {
		case Impl::CenterLineOnlyMouseEventMode::Normal:
		case Impl::CenterLineOnlyMouseEventMode::MoveVertexPrepare:
		case Impl::CenterLineOnlyMouseEventMode::AddVertexPrepare:
		case Impl::CenterLineOnlyMouseEventMode::AddVertexNotPossible:
		case Impl::CenterLineOnlyMouseEventMode::RemoveVertexPrepare:
		case Impl::CenterLineOnlyMouseEventMode::RemoveVertexNotPossible:
			impl->updateMouseEventMode(event->pos());
			impl->updateMouseCursor(v);
			break;
		case Impl::CenterLineOnlyMouseEventMode::Defining:
			impl->pushCenterLineDefineNewVertexCommand(false, event->pos());
			break;
		case Impl::CenterLineOnlyMouseEventMode::MoveVertex:
			impl->pushCenterLineMoveVertexCommand(false, impl->m_previousPos, event->pos());
			break;
		case Impl::CenterLineOnlyMouseEventMode::AddVertex:
			impl->pushCenterLineAddVertexCommand(false, event->pos());
			break;
		default:
			break;
		}
	} else if (impl->m_editMode == Impl::EditMode::RegionDefined) {
		switch (impl->m_regionDefinedMouseEventMode) {
		case Impl::RegionDefinedMouseEventMode::Normal:
		case Impl::RegionDefinedMouseEventMode::MoveVertexPrepare:
		case Impl::RegionDefinedMouseEventMode::AddVertexPrepare:
		case Impl::RegionDefinedMouseEventMode::AddVertexNotPossible:
		case Impl::RegionDefinedMouseEventMode::RemoveVertexPrepare:
		case Impl::RegionDefinedMouseEventMode::RemoveVertexNotPossible:
		case Impl::RegionDefinedMouseEventMode::AddEdgeLinePrepare:
		case Impl::RegionDefinedMouseEventMode::AddEdgeLineNotPossible:
			impl->updateMouseEventMode(event->pos());
			impl->updateMouseCursor(v);
			break;
		case Impl::RegionDefinedMouseEventMode::AddEdgeLine:
		case Impl::RegionDefinedMouseEventMode::AddEdgeLineFinishPrepare:
			if (impl->m_newEdgeLine.polyLine().size() == 2) {
				impl->pushNewEdgeStartDefinitionCommand(false, event->pos(), v);
			} else {
				impl->pushNewEdgeDefineNewVertexCommand(false, event->pos());
			}
			impl->updateMouseEventMode(event->pos());
			impl->updateMouseCursor(v);
			break;
		case Impl::RegionDefinedMouseEventMode::MoveVertex:
			impl->pushEdgeMoveCommand(false, impl->m_previousPos, event->pos());
			break;
		case Impl::RegionDefinedMouseEventMode::AddVertex:
			impl->pushEdgeAddVertexCommand(false, event->pos());
			break;
		default:
			break;
		}
	}
	impl->m_previousPos = event->pos();
}

void GridCreatingConditionLaplace::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		if (event->button() == Qt::LeftButton) {
			switch (impl->m_centerLineOnlyMouseEventMode) {
			case Impl::CenterLineOnlyMouseEventMode::BeforeDefining:
				impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Defining;
				impl->pushCenterLineStartDefinitionCommand(true, event->pos());
				break;
			case Impl::CenterLineOnlyMouseEventMode::Defining:
				impl->pushCenterLineDefineNewVertexCommand(true, event->pos());
				break;
			case Impl::CenterLineOnlyMouseEventMode::MoveVertexPrepare:
				impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::MoveVertex;
				impl->pushCenterLineMoveVertexCommand(true, event->pos(), event->pos());
				break;
			case Impl::CenterLineOnlyMouseEventMode::AddVertexPrepare:
				impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::AddVertex;
				impl->pushCenterLineAddVertexCommand(true, event->pos());
				break;
			case Impl::CenterLineOnlyMouseEventMode::RemoveVertexPrepare:
				impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
				impl->pushCenterLineRemoveVertexCommand();
				break;
			}
			impl->updateMouseCursor(v);
			impl->updateActionStatus();
		}
	} else if (impl->m_editMode == Impl::EditMode::RegionDefined){
		if (event->button() == Qt::LeftButton) {
			switch (impl->m_regionDefinedMouseEventMode) {
			case Impl::RegionDefinedMouseEventMode::AddEdgeLinePrepare:
				impl->pushNewEdgeStartDefinitionCommand(true, event->pos(), v);
				impl->updateMouseCursor(v);
				break;
			case Impl::RegionDefinedMouseEventMode::AddEdgeLine:
				impl->pushNewEdgeDefineNewVertexCommand(true, event->pos());
				break;
			case Impl::RegionDefinedMouseEventMode::AddEdgeLineFinishPrepare:
				impl->addNewEdge(event->pos(), v);
				break;
			case Impl::RegionDefinedMouseEventMode::MoveVertexPrepare:
				impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::MoveVertex;
				impl->pushEdgeMoveCommand(true, event->pos(), event->pos());
				break;
			case Impl::RegionDefinedMouseEventMode::AddVertexPrepare:
				impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::AddVertex;
				impl->pushEdgeAddVertexCommand(true, event->pos());
				break;
			case Impl::RegionDefinedMouseEventMode::RemoveVertexPrepare:
				impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::Normal;
				impl->pushEdgeRemoveVertexCommand();
				break;
			}
			impl->updateMouseCursor(v);
			impl->updateActionStatus();
		}
	}
	impl->m_previousPos = event->pos();
	impl->m_pressPos = event->pos();
}

void GridCreatingConditionLaplace::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		if (event->button() == Qt::LeftButton) {
			impl->updateMouseEventMode(event->pos());
			impl->updateMouseCursor(v);
			impl->updateActionStatus();
		} else if (event->button() == Qt::RightButton) {
			if (impl->m_centerLineOnlyMouseEventMode == Impl::CenterLineOnlyMouseEventMode::EditCoodinatesDialog) {return;}

			if (iRIC::isNear(impl->m_pressPos, event->pos())) {
				impl->m_rightClickingMenu->move(event->globalPos());
				impl->m_rightClickingMenu->show();
			}
		}
	} else {
		if (event->button() == Qt::LeftButton) {
			impl->updateMouseEventMode(event->pos(), true);
			impl->updateMouseCursor(v);
			impl->updateActionStatus();
			impl->updateActorSetting();
			renderGraphicsView();
		} else if (event->button() == Qt::RightButton) {
			if (impl->m_regionDefinedMouseEventMode == Impl::RegionDefinedMouseEventMode::EditCoodinatesDialog) {return;}

			if (iRIC::isNear(impl->m_pressPos, event->pos())) {
				impl->m_rightClickingMenu->move(event->globalPos());
				impl->m_rightClickingMenu->show();
			}
		}
	}
}

void GridCreatingConditionLaplace::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void GridCreatingConditionLaplace::assignActorZValues(const ZDepthRange& range)
{
	impl->m_zDepthRange = range;
	impl->m_centerLine.pointsActor()->SetPosition(0, 0, range.max());
	impl->m_centerLineSpline.linesActor()->SetPosition(0, 0, range.min());
}

void GridCreatingConditionLaplace::buildBankLines()
{
	impl->buildBankLines();
}

void GridCreatingConditionLaplace::switchModeToShape()
{
	impl->m_editMode = Impl::EditMode::RegionDefined;
	impl->updateActorSetting();
	impl->updateActionStatus();
	setupMenu();
	renderGraphicsView();
}

void GridCreatingConditionLaplace::switchModeToDivide()
{
	impl->m_editMode = Impl::EditMode::DivideSetting;
	impl->updateActorSetting();
	impl->updateActionStatus();
	setupMenu();
	renderGraphicsView();
}

void GridCreatingConditionLaplace::interpolateModeSprine()
{
	impl->pushEdgeSetInterpolationModeCommand(Impl::InterpolationType::Spline);
}

void GridCreatingConditionLaplace::interpolateModeLinear()
{
	impl->pushEdgeSetInterpolationModeCommand(Impl::InterpolationType::Linear);
}

void GridCreatingConditionLaplace::newEdgeMode(bool on)
{
	if (impl->m_editMode != Impl::EditMode::RegionDefined) {return;}
	if (on) {
		InformationDialog::information(preProcessorWindow(), tr("Information"), tr("To divide region, click on the edge of the region first, and click on the another edge."), "gridcreatingconditionlaplace_divideregion");
		impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::AddEdgeLineNotPossible;
	} else {
		impl->m_newEdgeLine.clear();
		impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::Normal;
	}
}

void GridCreatingConditionLaplace::joinRegions()
{
	if (impl->m_selectedSectionEdgeType == Impl::EdgeType::StreamWise) {
		int j = impl->m_selectedSectionId / (impl->m_ctrlPointCountI - 1);
		if (j == 0 || j == impl->m_ctrlPointCountJ - 1) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Please select edge line inside the region when you want to join regions."));
		} else {
			int ret = QMessageBox::information(preProcessorWindow(), tr("Warning"), tr("Are you sure you join the regions divided with the selected edge?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret == QMessageBox::No) {return;}
			impl->removeEdgeLineStreamWise(j);
		}
	} else if (impl->m_selectedSectionEdgeType == Impl::EdgeType::CrossSection) {
		int i = impl->m_selectedSectionId % impl->m_ctrlPointCountI;
		if (i == 0 || i == impl->m_ctrlPointCountI - 1) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Please select edge line inside the region when you want to join regions."));
		} else {
			int ret = QMessageBox::information(preProcessorWindow(), tr("Warning"), tr("Are you sure you join the regions divided with the selected edge?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret == QMessageBox::No) {return;}
			impl->removeEdgeLineCrossSection(i);
		}
	}
}

void GridCreatingConditionLaplace::addVertexMode(bool on)
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		if (on) {
			impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::AddVertexNotPossible;
		} else {
			impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
		}
	} else {
		if (on) {
			impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::AddVertexNotPossible;
		} else {
			impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::Normal;
		}
	}
	impl->updateActionStatus();
}

void GridCreatingConditionLaplace::removeVertexMode(bool on)
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		if (on) {
			impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::RemoveVertexNotPossible;
		} else {
			impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
		}
	} else {
		if (on) {
			impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::RemoveVertexNotPossible;
		} else {
			impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::Normal;
		}
	}
	impl->updateActionStatus();
}

void GridCreatingConditionLaplace::editCoorinates()
{
	if (impl->m_editMode == Impl::EditMode::CenterLineOnly) {
		CenterLineCoordinatesEditor::edit(&impl->m_centerLine, this);
	} else {
		if (impl->m_selectedSectionEdgeType == Impl::EdgeType::None) {return;}
		if (impl->m_selectedSectionEdgeType == Impl::EdgeType::StreamWise) {
			int i = impl->m_selectedSectionId % (impl->m_ctrlPointCountI - 1);
			int j = impl->m_selectedSectionId / (impl->m_ctrlPointCountI - 1);
			EdgeCoordinatesEditor::edit(true, i, j, this);
		} else {
			int i = impl->m_selectedSectionId % impl->m_ctrlPointCountI;
			int j = impl->m_selectedSectionId / impl->m_ctrlPointCountI;
			EdgeCoordinatesEditor::edit(false, i, j, this);
		}
	}
}

void GridCreatingConditionLaplace::wholeRegionDivisionSetting()
{
	GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog dialog(preProcessorWindow());
	int idiv = 0;
	int jdiv = 0;
	for (auto d : impl->m_divCountsStreamWise) {
		idiv += d;
	}
	for (auto d : impl->m_divCountsCrossSection) {
		jdiv += d;
	}

	double iLen = 0;
	std::vector<double> iLens;
	for (int i = 0; i < impl->m_ctrlPointCountI - 1; ++i) {
		std::vector<PolyLineController*> lines;
		for (int j = 0; j < impl->m_ctrlPointCountJ; ++j) {
			lines.push_back(&(impl->edgeLineStreamWiseForEdgeSelection(i, j)));
		}
		auto tmpLen = Impl::averageLength(lines);
		iLens.push_back(tmpLen);
		iLen += tmpLen;
	}
	double jLen = 0;
	std::vector<double> jLens;
	for (int j = 0; j < impl->m_ctrlPointCountJ - 1; ++j) {
		std::vector<PolyLineController*> lines;
		for (int i = 0; i < impl->m_ctrlPointCountI; ++i) {
			lines.push_back(&(impl->edgeLineCrossSectionForEdgeSelection(i, j)));
		}
		auto tmpLen = Impl::averageLength(lines);
		jLens.push_back(tmpLen);
		jLen += tmpLen;
	}
	dialog.setILength(iLen);
	dialog.setJLength(jLen);
	dialog.setIDivMin(impl->m_divCountsStreamWise.size());
	dialog.setJDivMin(impl->m_divCountsCrossSection.size());
	dialog.setIDiv(idiv);
	dialog.setJDiv(jdiv);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	int iDiv = dialog.iDiv();
	int jDiv = dialog.jDiv();

	int maxIdx, maxDiv, divSum;
	std::vector<int> iDivVec, jDivVec;

	divSum = 0;
	for (int i = 0; i < iLens.size(); ++i) {
		int div = static_cast<int>(iDiv * iLens.at(i) / iLen);
		if (div == 0) {div = 1;}
		if (i == 0 || div > maxDiv) {
			maxIdx = i;
			maxDiv = div;
		}
		iDivVec.push_back(div);
		divSum += div;
	}
	if (divSum < iDiv) {
		iDivVec[maxIdx] += (iDiv - divSum);
	}
	divSum = 0;
	for (int j = 0; j < jLens.size(); ++j) {
		int div = static_cast<int>(jDiv * jLens.at(j) / jLen);
		if (div == 0) {div = 1;}
		if (j == 0 || div > maxDiv) {
			maxIdx = j;
			maxDiv = div;
		}
		jDivVec.push_back(div);
		divSum += div;
	}
	if (divSum < jDiv) {
		jDivVec[maxIdx] += (jDiv - divSum);
	}
	impl->pushWholeRegionDivisionSettingCommand(iDivVec, jDivVec);
}

void GridCreatingConditionLaplace::divisionSetting()
{
	if (impl->m_selectedSectionEdgeType == Impl::EdgeType::None) {return;}

	int edgeId;
	GridCreatingConditionLaplaceDivisionSettingDialog dialog(preProcessorWindow());
	if (impl->m_selectedSectionEdgeType == Impl::EdgeType::StreamWise) {
		edgeId = impl->m_selectedSectionId % (impl->m_ctrlPointCountI - 1);
		dialog.setDivisionNumber(impl->m_divCountsStreamWise[edgeId]);
	} else {
		edgeId = impl->m_selectedSectionId / impl->m_ctrlPointCountI;
		dialog.setDivisionNumber(impl->m_divCountsCrossSection[edgeId]);
	}

	int result = dialog.exec();
	if (result == QDialog::Rejected) {return;}

	if (impl->m_selectedSectionEdgeType == Impl::EdgeType::StreamWise) {
		impl->pushDivisionSettingCommand(true, edgeId, dialog.divisionNumber());
	} else {
		impl->pushDivisionSettingCommand(false, edgeId, dialog.divisionNumber());
	}
}

void GridCreatingConditionLaplace::deploySetting()
{
	if (impl->m_selectedSectionEdgeType == Impl::EdgeType::None) {return;}

	GridCreatingConditionLaplaceDeploySettingDialog dialog(preProcessorWindow());
	if (impl->m_selectedSectionEdgeType == Impl::EdgeType::StreamWise) {
		dialog.setDivisionMode(impl->m_divModesStreamWise[impl->m_selectedSectionId]);
		dialog.setCommonRatio(impl->m_divCommonRatiosStreamWise[impl->m_selectedSectionId]);
	} else {
		dialog.setDivisionMode(impl->m_divModesCrossSection[impl->m_selectedSectionId]);
		dialog.setCommonRatio(impl->m_divCommonRatiosCrossSection[impl->m_selectedSectionId]);
	}

	int result = dialog.exec();
	if (result == QDialog::Rejected) {return;}

	if (impl->m_selectedSectionEdgeType == Impl::EdgeType::StreamWise) {
		impl->pushDeploySettingCommand(true, impl->m_selectedSectionId, dialog.divisionMode(), dialog.commonRatio());
	} else {
		impl->pushDeploySettingCommand(false, impl->m_selectedSectionId, dialog.divisionMode(), dialog.commonRatio());
	}
}

void GridCreatingConditionLaplace::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_ctrlPointCountI = iRIC::getIntAttribute(node, "ctrlPointCountI", 2);
	impl->m_ctrlPointCountJ = iRIC::getIntAttribute(node, "ctrlPointCountJ", 3);
	impl->m_previousLeftBankDistance = iRIC::getDoubleAttribute(node, "leftDist", 10);
	impl->m_previousRightBankDistance = iRIC::getDoubleAttribute(node, "rightDist", 10);
	impl->m_centerLineIndex = iRIC::getIntAttribute(node, "centerLineIndex", 1);
}

void GridCreatingConditionLaplace::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setIntAttribute(writer, "ctrlPointCountI", impl->m_ctrlPointCountI);
	iRIC::setIntAttribute(writer, "ctrlPointCountJ", impl->m_ctrlPointCountJ);
	iRIC::setDoubleAttribute(writer, "leftDist", impl->m_previousLeftBankDistance);
	iRIC::setDoubleAttribute(writer, "rightDist", impl->m_previousRightBankDistance);
	iRIC::setIntAttribute(writer, "centerLineIndex", impl->m_centerLineIndex);
}

void GridCreatingConditionLaplace::loadExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);

	loadPolyLine(&s, &(impl->m_centerLine), offset());

	loadPoints(&s, &(impl->m_ctrlPoints), offset());
	Impl::loadEnumVector(&s, &(impl->m_edgeInterpolationStreamWise));
	Impl::loadEnumVector(&s, &(impl->m_edgeInterpolationCrossSection));
	Impl::loadValueVector(&s, &(impl->m_divCountsStreamWise));
	Impl::loadValueVector(&s, &(impl->m_divCountsCrossSection));
	Impl::loadEnumVector(&s, &(impl->m_divModesStreamWise));
	Impl::loadEnumVector(&s, &(impl->m_divModesCrossSection));
	Impl::loadValueVector(&s, &(impl->m_divCommonRatiosStreamWise));
	Impl::loadValueVector(&s, &(impl->m_divCommonRatiosCrossSection));

	auto r = renderer();
	for (int j = 0; j < impl->m_ctrlPointCountJ; ++j) {
		for (int i = 0; i < impl->m_ctrlPointCountI - 1; ++i) {
			auto line = new PolyLineController();
			loadPolyLine(&s, line, offset());
			impl->insertEdgeLineStreamWise(line);
			impl->addEdgeLinesStreamWiseForSelectionAndPreview(r);
		}
	}
	for (int j = 0; j < impl->m_ctrlPointCountJ - 1; ++j) {
		for (int i = 0; i < impl->m_ctrlPointCountI; ++i) {
			auto line = new PolyLineController();
			loadPolyLine(&s, line, offset());
			impl->insertEdgeLineCrossSection(line);
			impl->addEdgeLinesCrossSectionForSelectionAndPreview(r);
		}
	}

	f.close();
	impl->updateCenterLineLabelsAndSpline();
	impl->updateEdgeLinesForSelection();
	if (impl->m_ctrlPointCountI != 0) {
		impl->m_editMode = Impl::EditMode::RegionDefined;
		impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::Normal;
	} else {
		impl->m_editMode = Impl::EditMode::CenterLineOnly;
		if (impl->m_centerLine.polyLine().size() < 2) {
			impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::BeforeDefining;
		} else {
			impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
		}
	}
	impl->updateActionStatus();
	impl->updateActorSetting();
}

void GridCreatingConditionLaplace::saveExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);

	savePolyLine(&s, impl->m_centerLine, offset());

	savePoints(&s, impl->m_ctrlPoints, offset());
	Impl::saveEnumVector(&s, impl->m_edgeInterpolationStreamWise);
	Impl::saveEnumVector(&s, impl->m_edgeInterpolationCrossSection);
	Impl::saveValueVector(&s, impl->m_divCountsStreamWise);
	Impl::saveValueVector(&s, impl->m_divCountsCrossSection);
	Impl::saveEnumVector(&s, impl->m_divModesStreamWise);
	Impl::saveEnumVector(&s, impl->m_divModesCrossSection);
	Impl::saveValueVector(&s, impl->m_divCommonRatiosStreamWise);
	Impl::saveValueVector(&s, impl->m_divCommonRatiosCrossSection);

	int idx;
	idx = 0;
	for (int j = 0; j < impl->m_ctrlPointCountJ; ++j) {
		for (int i = 0; i < impl->m_ctrlPointCountI - 1; ++i) {
			savePolyLine(&s, *(impl->m_edgeLinesStreamWise[idx]), offset());
			++idx;
		}
	}
	idx = 0;
	for (int j = 0; j < impl->m_ctrlPointCountJ - 1; ++j) {
		for (int i = 0; i < impl->m_ctrlPointCountI; ++i) {
			savePolyLine(&s, *(impl->m_edgeLinesCrossSection[idx]), offset());
			++idx;
		}
	}
}

void GridCreatingConditionLaplace::doApplyOffset(double x, double y)
{

}
