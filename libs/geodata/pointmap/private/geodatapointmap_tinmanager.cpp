#include "geodatapointmap_impl.h"
#include "geodatapointmap_pointsmanager.h"
#include "geodatapointmap_displaysetting.h"
#include "geodatapointmap_tinmanager.h"
#include "geodatapointmap_tinmanager_actions.h"
#include "geodatapointmap_tinmanager_breakline.h"
#include "geodatapointmap_tinmanager_addbreaklinecontroller.h"
#include "geodatapointmap_tinmanager_breakline.h"
#include "geodatapointmap_tinmanager_deletebreaklinecontroller.h"
#include "geodatapointmap_tinmanager_tinbuilder.h"

#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>
#include <misc/mouseeventcontroller.h>

#include <geos/geom/Envelope.h>
#include <geos/index/quadtree/Quadtree.h>

#include <vtkLODActor.h>

GeoDataPointmap::TINManager::TINManager(PointsManager* points, GeoDataPointmap* pointmap) :
	m_tin {vtkPolyData::New()},
	m_tinActor {vtkLODActor::New()},
	m_qTree {nullptr},
	m_needRebuild {false},
	m_pointsManager {points},
	m_parent {pointmap},
	m_normalController {new MouseEventController {}},
	m_addBreakLineController {new AddBreakLineController {this}},
	m_deleteBreakLineController {new DeleteBreakLineController {this}},
	m_activeController {m_normalController},
	m_actions {new Actions {this}}
{
	m_controllers.push_back(m_normalController);
	m_controllers.push_back(m_addBreakLineController);
	m_controllers.push_back(m_deleteBreakLineController);

	setupActors();
}

GeoDataPointmap::TINManager::~TINManager()
{
	m_tin->Delete();
	m_tinActor->Delete();
	delete m_qTree;

	delete m_normalController;
	delete m_addBreakLineController;
	delete m_deleteBreakLineController;
	delete m_actions;
}

void GeoDataPointmap::TINManager::removeBreakLine(BreakLine* line)
{
	line->removeActorsFromRenderer(m_parent->renderer());

	for (auto it = m_breakLines.begin(); it != m_breakLines.end(); ++it) {
		if (*it == line) {
			m_breakLines.erase(it);
			break;
		}
	}
	delete line;

	iRICUndoStack::instance().clear();
	m_needRebuild = true;
	m_parent->setMapped(false);

	m_parent->renderGraphicsView();
}

std::vector<GeoDataPointmap::TINManager::BreakLine*>& GeoDataPointmap::TINManager::breakLines()
{
	return m_breakLines;
}

const std::vector<GeoDataPointmap::TINManager::BreakLine*>& GeoDataPointmap::TINManager::breakLines() const
{
	return m_breakLines;
}

vtkPolyData* GeoDataPointmap::TINManager::tin() const
{
	return m_tin;
}

vtkDoubleArray* GeoDataPointmap::TINManager::values() const
{
	return vtkDoubleArray::SafeDownCast(m_tin->GetPointData()->GetArray(VALUES));
}

void GeoDataPointmap::TINManager::setTinData(vtkPolyData* data, vtkDoubleArray* values)
{
	m_tin->Initialize();
	m_tin->GetPointData()->Initialize();

	auto origPoints = data->GetPoints();
	auto newPoints = vtkSmartPointer<vtkPoints>::New();
	newPoints->SetDataTypeToDouble();

	double v[3];
	for (vtkIdType i = 0; i < origPoints->GetNumberOfPoints(); ++i) {
		origPoints->GetPoint(i, v);
		v[2] = 0;
		newPoints->InsertNextPoint(v);
	}
	m_tin->SetPoints(newPoints);
	m_tin->SetPolys(data->GetPolys());

	values->SetName(VALUES);
	m_tin->GetPointData()->AddArray(values);
	m_tin->GetPointData()->SetActiveScalars(VALUES);
	m_tin->BuildCells();

	rebuildQTree();
}

void GeoDataPointmap::TINManager::rebuildTinFromPointsIfNeeded()
{
	if (! m_needRebuild) {return;}

	rebuildTinFromPoints(false);
}

bool GeoDataPointmap::TINManager::rebuildTinFromPoints(bool allowCancel)
{
	TINBuilder builder(this);
	bool succeeded = builder.build(allowCancel);
	if (! succeeded) {return false;}

	rebuildQTree();

	return succeeded;
}

void GeoDataPointmap::TINManager::updateBreakLinesActorSettings() const
{
	const auto& setting = m_parent->impl->m_displaySetting;
	if (! setting.breakLineVisible) {
		for (auto line : m_breakLines) {
			line->controller().linesActor()->VisibilityOff();
		}
	} else {
		auto v = m_parent->dataModel()->graphicsView();

		const auto& color = setting.breakLineColor;
		int lineWidth = setting.breakLineWidth;
		auto col = m_parent->actorCollection();
		for (auto line : m_breakLines) {
			auto lineActor = line->controller().linesActor();
			lineActor->GetProperty()->SetLineWidth(lineWidth * v->devicePixelRatioF());
			lineActor->GetProperty()->SetColor(color);
			col->AddItem(lineActor);

			auto pointActor = line->controller().pointsActor();
			pointActor->GetProperty()->SetPointSize(lineWidth * 3 * v->devicePixelRatioF());
		}
	}
}

bool GeoDataPointmap::TINManager::needRebuild() const
{
	return m_needRebuild;
}

void GeoDataPointmap::TINManager::setNeedRebuild(bool needed)
{
	m_needRebuild = needed;
}

vtkCell* GeoDataPointmap::TINManager::findCell(double x, double y, double* weights)
{
	if (m_qTree == nullptr) {return nullptr;}

	auto env = new geos::geom::Envelope(x, x, y, y);
	std::vector<void*> ret;
	m_qTree->query(env, ret);
	delete env;

	double point[3], closestPoint[3], pcoords[3], dist;
	double bounds[6];
	int subId;

	point[0] = x;
	point[1] = y;
	point[2] = 0;

	for (void* vptr : ret) {
		vtkIdType cellId = reinterpret_cast<vtkIdType> (vptr);
		m_tin->GetCellBounds(cellId, bounds);
		if (point[0] < bounds[0]) {continue;}
		if (point[0] > bounds[1]) {continue;}
		if (point[1] < bounds[2]) {continue;}
		if (point[1] > bounds[3]) {continue;}
		vtkCell* cell = m_tin->GetCell(cellId);
		if (1 == cell->EvaluatePosition(point, closestPoint, subId, pcoords, dist, weights)) {
			return cell;
		}
	}

	return nullptr;
}

bool GeoDataPointmap::TINManager::checkIfBreakLinesHasIntersections() const
{
	for (unsigned int i = 0; i < m_breakLines.size(); ++i) {
		auto l1 = m_breakLines.at(i);
		for (unsigned int j = i + 1; j < m_breakLines.size(); ++j) {
			auto l2 = m_breakLines.at(j);
			if (l1->intersects(*l2)) {
				auto w = m_parent->preProcessorWindow();
				QMessageBox::warning(w, GeoDataPointmap::tr("Warning"), GeoDataPointmap::tr("Break line have to have no intersection with other break lines."));
				return true;
			}
		}
	}
	return false;
}

void GeoDataPointmap::TINManager::setZDepthToBreakLines(double z)
{
	for (auto line : m_breakLines) {
		line->controller().linesActor()->SetPosition(0, 0, z);
		line->controller().pointsActor()->SetPosition(0, 0, z);
	}
}

bool GeoDataPointmap::TINManager::pointsUsedForBreakLines(const std::unordered_set<vtkIdType>& indices)
{
	for (auto line : m_breakLines) {
		for (auto index : line->vertexIndices()) {
			auto it = indices.find(index);
			if (it != indices.end()) {
				return true;
			}
		}
	}
	return false;
}

void GeoDataPointmap::TINManager::updateBreakLinesOnPointsDelete(const std::unordered_set<vtkIdType>& indicesSet)
{
	std::vector<vtkIdType> indices;
	for (vtkIdType idx : indicesSet) {
		indices.push_back(idx);
	}
	std::sort(indices.begin(), indices.end());

	std::vector<vtkIdType> keyIds;
	std::vector<int> offsets;

	// build lists first.
	vtkIdType prevDeletedId = -1;
	vtkIdType keyIdToAddCandidate = 0;
	vtkIdType offsetToAddCandidate = 0;

	for (unsigned int i = 0; i < indices.size(); ++i) {
		vtkIdType deletedId = indices[i];
		if (deletedId != prevDeletedId + 1) {
			// Continuous
			keyIds.push_back(keyIdToAddCandidate);
			offsets.push_back(offsetToAddCandidate);
		} else {
			// Not continuous, so not used.
		}
		keyIdToAddCandidate = deletedId + 1;
		++ offsetToAddCandidate;

		prevDeletedId = deletedId;
	}
	keyIds.push_back(keyIdToAddCandidate);
	offsets.push_back(offsetToAddCandidate);

	for (auto line : m_breakLines) {
		auto oldindices = line->vertexIndices();
		std::vector<vtkIdType> newindices;
		for (unsigned int i = 0; i < oldindices.size(); ++i) {
			vtkIdType oldindex = oldindices[i];
			auto lb = std::lower_bound(keyIds.begin(), keyIds.end(), oldindex);
			vtkIdType tmpId = *lb;
			std::vector<vtkIdType>::iterator keyIt;
			if (lb == keyIds.end()) {
				-- lb;
				tmpId = *lb;
				keyIt = std::lower_bound(keyIds.begin(), keyIds.end(), tmpId);
			} else {
				keyIt = std::lower_bound(keyIds.begin(), keyIds.end(), tmpId);
				if (tmpId != oldindex) {
					--keyIt;
				}
			}
			auto offsetIndex = keyIt - keyIds.begin();
			int offset = offsets.at(offsetIndex);
			newindices.push_back(oldindex - offset);
		}
		line->setVertexIndices(newindices);
	}
}

void GeoDataPointmap::TINManager::updateBreakLinesOnPointsUndoDelete(const std::unordered_set<vtkIdType>& indicesSet)
{
	std::vector<vtkIdType> indices;
	for (vtkIdType idx : indicesSet) {
		indices.push_back(idx);
	}
	std::sort(indices.begin(), indices.end());

	std::vector<vtkIdType> keyIds;
	std::vector<int> offsets;

	// build lists first.
	vtkIdType prevDeletedId = -1;
	vtkIdType keyIdToAddCandidate = 0;
	vtkIdType offsetToAddCandidate = 0;

	for (unsigned int i = 0; i < indices.size(); ++i) {
		vtkIdType deletedId = indices[i];
		if (deletedId != prevDeletedId + 1) {
			// Continuous
			keyIds.push_back(keyIdToAddCandidate);
			offsets.push_back(offsetToAddCandidate);
		} else {
			// Not continuous, so not used.
		}
		keyIdToAddCandidate = deletedId - offsetToAddCandidate;
		++ offsetToAddCandidate;

		prevDeletedId = deletedId;
	}
	keyIds.push_back(keyIdToAddCandidate);
	offsets.push_back(offsetToAddCandidate);

	for (auto line : m_breakLines) {
		auto  oldindices = line->vertexIndices();
		std::vector<vtkIdType> newindices;
		for (unsigned int i = 0; i < oldindices.size(); ++i) {
			vtkIdType oldindex = oldindices[i];
			auto lb = std::lower_bound(keyIds.begin(), keyIds.end(), oldindex);
			vtkIdType tmpId = *lb;
			std::vector<vtkIdType>::iterator keyIt;
			if (lb == keyIds.end()) {
				-- lb;
				tmpId = *lb;
				keyIt = std::lower_bound(keyIds.begin(), keyIds.end(), tmpId);
			} else {
				keyIt = std::lower_bound(keyIds.begin(), keyIds.end(), tmpId);
				if (tmpId != oldindex) {
					--keyIt;
				}
			}
			int offsetIndex = keyIt - keyIds.begin();
			int offset = offsets.at(offsetIndex);
			newindices.push_back(oldindex + offset);
		}
		line->setVertexIndices(newindices);
	}
}

void GeoDataPointmap::TINManager::addActorsToRenderer(vtkRenderer* renderer)
{
	renderer->AddActor(m_tinActor);
	for (auto l : m_breakLines) {
		l->addActorsToRenderer(renderer);
	}
	for (auto c : m_controllers) {
		c->addActorsToRenderer(renderer);
	}
}

void GeoDataPointmap::TINManager::removeActorsFromRenderer(vtkRenderer* renderer)
{
	renderer->RemoveActor(m_tinActor);
	for (auto l : m_breakLines) {
		l->removeActorsFromRenderer(renderer);
	}
	for (auto c : m_controllers) {
		c->removeActorsFromRenderer(renderer);
	}
}

void GeoDataPointmap::TINManager::addActionsToMenu(QMenu* menu)
{
	menu->addAction(m_actions->addBreakLineAction);
	menu->addAction(m_actions->removeBreakLineAction);
	menu->addAction(m_actions->removeAllBreakLinesAction);

	menu->addSeparator();

	menu->addAction(m_actions->remeshTinAction);
}

void GeoDataPointmap::TINManager::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleKeyPressEvent(event, v);
}

void GeoDataPointmap::TINManager::handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleKeyReleaseEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMouseDoubleClickEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMouseMoveEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMousePressEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMouseReleaseEvent(event, v);
}

vtkLODActor* GeoDataPointmap::TINManager::tinActor() const
{
	return m_tinActor;
}

void GeoDataPointmap::TINManager::toggleAddBreakLineMode(bool on)
{
	if (on) {
		m_actions->removeBreakLineAction->setChecked(false);
	}
	auto v = m_parent->dataModel()->graphicsView();
	m_activeController->deactivate(v);
	if (on) {
		iRICUndoStack::instance().clear();

		auto breakLine = new BreakLine(m_pointsManager->points());
		breakLine->addActorsToRenderer(m_parent->renderer());
		breakLine->controller().pointsActor()->VisibilityOn();
		m_breakLines.push_back(breakLine);
		m_addBreakLineController->setBreakLine(breakLine);
		m_activeController = m_addBreakLineController;

		m_parent->setNeedRebuildTin(true);
		m_parent->setMapped(false);

		m_parent->assignActorZValues(m_parent->m_zDepthRange);
		m_parent->updateActorSetting();
	} else {
		m_activeController = m_normalController;
	}
	m_activeController->activate(v);
}

void GeoDataPointmap::TINManager::toggleRemoveBreakLineMode(bool on)
{
	if (on) {
		m_actions->addBreakLineAction->setChecked(false);
	}
	auto v = m_parent->dataModel()->graphicsView();
	m_activeController->deactivate(v);
	if (on) {
		m_activeController = m_deleteBreakLineController;
	} else {
		m_activeController = m_normalController;
	}
	m_activeController->activate(v);
}

void GeoDataPointmap::TINManager::removeAllBreakLines()
{
	auto w = m_parent->preProcessorWindow();

	int result = QMessageBox::warning(w, GeoDataPointmap::tr("Warning"), GeoDataPointmap::tr("Are you sure you want to remove ALL break lines?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}

	auto renderer = m_parent->renderer();
	for (auto line : m_breakLines) {
		line->removeActorsFromRenderer(renderer);
		delete line;
	}
	m_breakLines.clear();

	// This operation is not undo-able.
	iRICUndoStack::instance().clear();
	m_parent->renderGraphicsView();
}

void GeoDataPointmap::TINManager::setupActors()
{
	auto prop = m_tinActor->GetProperty();
	prop->SetRepresentationToPoints();
	prop->SetLighting(false);
}

void GeoDataPointmap::TINManager::rebuildQTree()
{
	delete m_qTree;
	m_qTree = new geos::index::quadtree::Quadtree();

	double bounds[6];
	for (vtkIdType i = 0; i < m_tin->GetNumberOfCells(); ++i) {
		m_tin->GetCellBounds(i, bounds);

		auto env = new geos::geom::Envelope(bounds[0], bounds[1], bounds[2], bounds[3]);
		m_qTree->insert(env, reinterpret_cast<void*>(i));
	}
}
