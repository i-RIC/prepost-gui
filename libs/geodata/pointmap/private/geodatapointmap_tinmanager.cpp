#include "geodatapointmap_impl.h"
#include "geodatapointmap_pointsmanager.h"
#include "geodatapointmap_displaysetting.h"
#include "geodatapointmap_tinmanager.h"
#include "geodatapointmap_tinmanager_actions.h"
#include "geodatapointmap_tinmanager_breakline.h"
#include "geodatapointmap_tinmanager_addbreaklinecontroller.h"
#include "geodatapointmap_tinmanager_breakline.h"
#include "geodatapointmap_tinmanager_deletebreaklinecontroller.h"
#include "geodatapointmap_tinmanager_impl.h"
#include "geodatapointmap_tinmanager_removetrianglessettingdialog.h"
#include "geodatapointmap_tinmanager_triangleswithlongedgeremover.h"
#include "geodatapointmap_tinmanager_tinbuilder.h"

#include <guibase/vtktool/vtkpointsetgeos2dindex.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>
#include <misc/mouseeventcontroller.h>

#include <geos/geom/Envelope.h>

#include <vtkLODActor.h>

GeoDataPointmap::TINManager::TINManager(PointsManager* points, GeoDataPointmap* pointmap) :
	m_actions {new Actions {this}},
	impl {new Impl {this, pointmap}}
{
	impl->m_pointsManager = points;

	setupActors();
}

GeoDataPointmap::TINManager::~TINManager()
{
	delete m_actions;
	delete impl;
}

void GeoDataPointmap::TINManager::load(const QDomNode& node)
{
	impl->m_removeTrianglesSetting.load(node);
}

void GeoDataPointmap::TINManager::save(QXmlStreamWriter& writer)
{
	impl->m_removeTrianglesSetting.save(writer);
}

void GeoDataPointmap::TINManager::removeBreakLine(BreakLine* line)
{
	line->removeActorsFromRenderer(impl->m_parent->renderer());

	for (auto it = impl->m_breakLines.begin(); it != impl->m_breakLines.end(); ++it) {
		if (*it == line) {
			impl->m_breakLines.erase(it);
			break;
		}
	}
	delete line;

	iRICUndoStack::instance().clear();
	impl->m_needRebuild = true;
	impl->m_parent->setMapped(false);

	impl->m_parent->renderGraphicsView();
}

std::vector<GeoDataPointmap::TINManager::BreakLine*>& GeoDataPointmap::TINManager::breakLines()
{
	return impl->m_breakLines;
}

const std::vector<GeoDataPointmap::TINManager::BreakLine*>& GeoDataPointmap::TINManager::breakLines() const
{
	return impl->m_breakLines;
}

vtkPolyData* GeoDataPointmap::TINManager::tin() const
{
	return impl->m_tin;
}

vtkDoubleArray* GeoDataPointmap::TINManager::values() const
{
	return vtkDoubleArray::SafeDownCast(impl->m_tin->GetPointData()->GetArray(VALUES));
}

void GeoDataPointmap::TINManager::setTinData(vtkPolyData* data, vtkDoubleArray* values)
{
	impl->m_tin->Initialize();
	impl->m_tin->GetPointData()->Initialize();

	impl->m_tin->SetPoints(data->GetPoints());
	impl->m_tin->SetPolys(data->GetPolys());

	values->SetName(VALUES);
	impl->m_tin->GetPointData()->AddArray(values);
	impl->m_tin->GetPointData()->SetActiveScalars(VALUES);
	impl->m_tin->BuildCells();

	rebuildIndex();
}

void GeoDataPointmap::TINManager::rebuildTinFromPointsIfNeeded()
{
	if (! impl->m_needRebuild) {return;}

	rebuildTinFromPoints(false);
}

vtkPolyData* GeoDataPointmap::TINManager::buildTinFromPoints()
{
	auto ret = vtkPolyData::New();

	TINBuilder builder(this);
	builder.build(ret, false);

	return ret;
}

bool GeoDataPointmap::TINManager::rebuildTinFromPoints(bool allowCancel)
{
	TINBuilder builder(this);
	bool succeeded = builder.build(allowCancel);
	if (! succeeded) {return false;}

	if (impl->m_removeTrianglesSetting.enabled) {
		auto newCells = TrianglesWithLongEdgeRemover::buildCellArray(impl->m_tin, impl->m_removeTrianglesSetting.thresholdLength, false, false);
		impl->m_tin->SetPolys(newCells);
		newCells->Delete();
	}

	rebuildIndex();

	return succeeded;
}

void GeoDataPointmap::TINManager::updateBreakLinesActorSettings() const
{
	const auto& setting = impl->m_parent->impl->m_displaySetting;
	if (! setting.breakLineVisible) {
		for (auto line : impl->m_breakLines) {
			line->controller().linesActor()->VisibilityOff();
		}
	} else {
		auto v = impl->m_parent->dataModel()->graphicsView();

		const auto& color = setting.breakLineColor;
		int lineWidth = setting.breakLineWidth;
		auto col = impl->m_parent->actorCollection();
		for (auto line : impl->m_breakLines) {
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
	return impl->m_needRebuild;
}

void GeoDataPointmap::TINManager::setNeedRebuild(bool needed)
{
	impl->m_needRebuild = needed;
}

vtkCell* GeoDataPointmap::TINManager::findCell(double x, double y, double* weights)
{
	if (impl->m_index == nullptr) {return nullptr;}
	return impl->m_index->findCell(x, y, weights);
}

bool GeoDataPointmap::TINManager::checkIfBreakLinesHasIntersections() const
{
	for (unsigned int i = 0; i < impl->m_breakLines.size(); ++i) {
		auto l1 = impl->m_breakLines.at(i);
		for (unsigned int j = i + 1; j < impl->m_breakLines.size(); ++j) {
			auto l2 = impl->m_breakLines.at(j);
			if (l1->intersects(*l2)) {
				auto w = impl->m_parent->preProcessorWindow();
				QMessageBox::warning(w, GeoDataPointmap::tr("Warning"), GeoDataPointmap::tr("Break line have to have no intersection with other break lines."));
				return true;
			}
		}
	}
	return false;
}

void GeoDataPointmap::TINManager::setZDepthToBreakLines(double z)
{
	for (auto line : impl->m_breakLines) {
		line->controller().linesActor()->SetPosition(0, 0, z);
		line->controller().pointsActor()->SetPosition(0, 0, z);
	}
}

bool GeoDataPointmap::TINManager::pointsUsedForBreakLines(const std::unordered_set<vtkIdType>& indices)
{
	for (auto line : impl->m_breakLines) {
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

	for (auto line : impl->m_breakLines) {
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

	for (auto line : impl->m_breakLines) {
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
	renderer->AddActor(impl->m_tinActor);
	for (auto l : impl->m_breakLines) {
		l->addActorsToRenderer(renderer);
	}
	for (auto c : impl->m_controllers) {
		c->addActorsToRenderer(renderer);
	}
}

void GeoDataPointmap::TINManager::removeActorsFromRenderer(vtkRenderer* renderer)
{
	renderer->RemoveActor(impl->m_tinActor);
	for (auto l : impl->m_breakLines) {
		l->removeActorsFromRenderer(renderer);
	}
	for (auto c : impl->m_controllers) {
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
	menu->addAction(m_actions->removeTrianglesSettingAction);
}

void GeoDataPointmap::TINManager::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	impl->m_activeController->handleKeyPressEvent(event, v);
}

void GeoDataPointmap::TINManager::handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	impl->m_activeController->handleKeyReleaseEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	impl->m_activeController->handleMouseDoubleClickEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	impl->m_activeController->handleMouseMoveEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	impl->m_activeController->handleMousePressEvent(event, v);
}

void GeoDataPointmap::TINManager::handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	impl->m_activeController->handleMouseReleaseEvent(event, v);
}

vtkLODActor* GeoDataPointmap::TINManager::tinActor() const
{
	return impl->m_tinActor;
}

void GeoDataPointmap::TINManager::toggleAddBreakLineMode(bool on)
{
	if (on) {
		m_actions->removeBreakLineAction->setChecked(false);
	}
	auto v = impl->m_parent->dataModel()->graphicsView();
	impl->m_activeController->deactivate(v);
	if (on) {
		iRICUndoStack::instance().clear();

		auto breakLine = new BreakLine(impl->m_pointsManager->points());
		breakLine->addActorsToRenderer(impl->m_parent->renderer());
		breakLine->controller().pointsActor()->VisibilityOn();
		impl->m_breakLines.push_back(breakLine);
		impl->m_addBreakLineController->setBreakLine(breakLine);
		impl->m_activeController = impl->m_addBreakLineController;

		impl->m_parent->setNeedRebuildTin(true);
		impl->m_parent->setMapped(false);

		impl->m_parent->assignActorZValues(impl->m_parent->m_zDepthRange);
		impl->m_parent->updateActorSetting();
	} else {
		impl->m_activeController = impl->m_normalController;
	}
	impl->m_activeController->activate(v);
}

void GeoDataPointmap::TINManager::toggleRemoveBreakLineMode(bool on)
{
	if (on) {
		m_actions->addBreakLineAction->setChecked(false);
	}
	auto v = impl->m_parent->dataModel()->graphicsView();
	impl->m_activeController->deactivate(v);
	if (on) {
		impl->m_activeController = impl->m_deleteBreakLineController;
	} else {
		impl->m_activeController = impl->m_normalController;
	}
	impl->m_activeController->activate(v);
}

void GeoDataPointmap::TINManager::removeAllBreakLines()
{
	auto w = impl->m_parent->preProcessorWindow();

	int result = QMessageBox::warning(w, GeoDataPointmap::tr("Warning"), GeoDataPointmap::tr("Are you sure you want to remove ALL break lines?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}

	auto renderer = impl->m_parent->renderer();
	for (auto line : impl->m_breakLines) {
		line->removeActorsFromRenderer(renderer);
		delete line;
	}
	impl->m_breakLines.clear();

	// This operation is not undo-able.
	iRICUndoStack::instance().clear();
	impl->m_parent->renderGraphicsView();
}

void GeoDataPointmap::TINManager::showRemoveTrianglesSettingDialog()
{
	auto dialog = new RemoveTrianglesSettingDialog(this, impl->m_parent->preProcessorWindow());
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	connect(dialog, &QObject::destroyed, impl->m_parent->iricMainWindow(), &iRICMainWindowInterface::exitModelessDialogMode);

	impl->m_parent->iricMainWindow()->enterModelessDialogMode();

	dialog->show();
}

void GeoDataPointmap::TINManager::setupActors()
{
	auto prop = impl->m_tinActor->GetProperty();
	prop->SetRepresentationToPoints();
	prop->SetLighting(false);
}

void GeoDataPointmap::TINManager::rebuildIndex()
{
	delete impl->m_index;
	impl->m_index = new vtkPointSetGeos2dIndex(impl->m_tin);
}
