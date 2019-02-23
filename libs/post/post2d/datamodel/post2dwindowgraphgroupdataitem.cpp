#include "post2dwindowgraphdataitem.h"
#include "post2dwindowgraphgroupdataitem.h"
#include "post2dwindowgraphsettingdialog.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgraphgroupdataitem_impl.h"
#include "private/post2dwindowgraphgroupdataitem_setsettingcommand.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>

#include <QVector2D>

#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

#include <cmath>
#include <vector>

namespace {

void getDirectionParallel(Post2dWindowGraphSetting::Direction dir, int i, int j, vtkStructuredGrid* grid, double* x, double* y)
{
	int dims[3];
	double v0[3], v1[3];
	grid->GetDimensions(dims);

	*x = 0;
	*y = 0;

	if (dir == Post2dWindowGraphSetting::dirI) {
		if (i < dims[0] - 1) {
			grid->GetPoint(i, j, 0, v0);
			grid->GetPoint(i + 1, j, 0, v1);
		}	else if (i == dims[0] - 1){
			grid->GetPoint(i - 1, j, 0, v0);
			grid->GetPoint(i, j, 0, v1);
		} else {
			return;
		}
	} else {
		if (j < dims[1] - 1) {
			grid->GetPoint(i, j, 0, v0);
			grid->GetPoint(i, j + 1, 0, v1);
		}	else if (j == dims[1] - 1){
			grid->GetPoint(i, j - 1, 0, v0);
			grid->GetPoint(i, j, 0, v1);
		} else {
			return;
		}
	}

	QVector2D v(v1[0] - v0[0], v1[1] - v0[1]);
	v.normalize();
	*x = v.x();
	*y = v.y();
}

void getDirectionVertical(Post2dWindowGraphSetting::Direction dir, int i, int j, vtkStructuredGrid* grid, double* x, double* y)
{
	int dims[3];
	double v0[3], v1[3], v2[3];
	grid->GetDimensions(dims);

	*x = 0;
	*y = 0;

	QPointF v;
	if (dir == Post2dWindowGraphSetting::dirI) {
		if (j == 0) {
			grid->GetPoint(i, j, 0, v0);
			grid->GetPoint(i, j + 1, 0, v1);

			v = iRIC::normalize(QPointF(v1[0] - v0[0], v1[1] - v0[1]));
			iRIC::rotateVector270(v);
		} else if (j == dims[1] - 1) {
			grid->GetPoint(i, j - 1, 0, v0);
			grid->GetPoint(i, j, 0, v1);

			v = iRIC::normalize(QPointF(v1[0] - v0[0], v1[1] - v0[1]));
			iRIC::rotateVector270(v);
		} else {
			grid->GetPoint(i, j - 1, 0, v0);
			grid->GetPoint(i, j, 0, v1);
			grid->GetPoint(i, j + 1, 0, v2);

			QPointF tmpV1 = iRIC::normalize(QPointF(v1[0] - v0[0], v1[1] - v0[1]));
			iRIC::rotateVector270(tmpV1);

			QPointF tmpV2 = iRIC::normalize(QPointF(v2[0] - v1[0], v2[1] - v1[1]));
			iRIC::rotateVector270(tmpV2);

			v = iRIC::normalize(tmpV1 + tmpV2);
		}
	} else {
		if (i == 0) {
			grid->GetPoint(i, j, 0, v0);
			grid->GetPoint(i + 1, j, 0, v1);

			v = iRIC::normalize(QPointF(v1[0] - v0[0], v1[1] - v0[1]));
			iRIC::rotateVector90(v);
		} else if (i == dims[0] - 1) {
			grid->GetPoint(i - 1, j, 0, v0);
			grid->GetPoint(i, j, 0, v1);

			v = iRIC::normalize(QPointF(v1[0] - v0[0], v1[1] - v0[1]));
			iRIC::rotateVector90(v);
		} else {
			grid->GetPoint(i - 1, j, 0, v0);
			grid->GetPoint(i, j, 0, v1);
			grid->GetPoint(i + 1, j, 0, v2);

			QPointF tmpV1 = iRIC::normalize(QPointF(v1[0] - v0[0], v1[1] - v0[1]));
			iRIC::rotateVector90(tmpV1);

			QPointF tmpV2 = iRIC::normalize(QPointF(v2[0] - v1[0], v2[1] - v1[1]));
			iRIC::rotateVector90(tmpV2);

			v = iRIC::normalize(tmpV1 + tmpV2);
		}
	}

	*x = v.x();
	*y = v.y();
}

void getDirection(const Post2dWindowGraphSetting& setting, int i, int j, vtkStructuredGrid* grid, double* x, double* y)
{
	if (setting.offsetDirection == Post2dWindowGraphSetting::Axis) {
		return getDirectionParallel(setting.direction, i, j, grid, x, y);
	} else {
		return getDirectionVertical(setting.direction, i, j, grid, x, y);
	}
}

std::vector<int> getAllRegionIndices(int size) {
	std::vector<int> ret(size);
	for (int i = 0; i < size; ++i) {
		ret[i] = i;
	}
	return ret;
}

std::vector<int> getPartialRegionIndices(const Post2dWindowGraphSetting& setting) {
	std::vector<int> ret;
	for (int i = setting.regionStartIndex; i <= setting.regionEndIndex; i += setting.regionSkipIndex) {
		ret.push_back(i);
	}
	return ret;
}

std::vector<int> getIndices(const Post2dWindowGraphSetting& setting, int isize, int jsize)
{
	if (setting.region == Post2dWindowGraphSetting::All) {
		if (setting.direction == Post2dWindowGraphSetting::dirI) {
			return getAllRegionIndices(isize);
		} else {
			return getAllRegionIndices(jsize);
		}
	} else if (setting.region == Post2dWindowGraphSetting::Partial) {
		return getPartialRegionIndices(setting);
	} else {
		return setting.regionIndices;
	}
}

} // namespace

Post2dWindowGraphGroupDataItem::Impl::Impl() :
	m_setting {},
	m_baseLinesActor {},
	m_baseLinesPolyData {vtkSmartPointer<vtkPolyData>::New()},
	m_graphLinesActor {}
{
	m_baseLinesActor.setPolyData(m_baseLinesPolyData);

	auto prop = m_baseLinesActor.actor()->GetProperty();
	prop->SetLighting(false);
	prop->SetRepresentationToWireframe();
}

Post2dWindowGraphGroupDataItem::Post2dWindowGraphGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem(tr("Graph"), QIcon(":/libs/guibase/images/iconFolder.png"), p),
	impl {new Impl()}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData())) {
		auto item = new Post2dWindowGraphDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}

	vtkRenderer* r = renderer();
	r->AddActor(impl->m_baseLinesActor.actor());
	r->AddActor(impl->m_graphLinesActor.linesActor());

	vtkPoints* basePoints = cont->data()->GetPoints();
	impl->m_baseLinesPolyData->SetPoints(basePoints);

	setDefaultSetting();

	update();
}

Post2dWindowGraphGroupDataItem::~Post2dWindowGraphGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(impl->m_baseLinesActor.actor());
	r->RemoveActor(impl->m_graphLinesActor.linesActor());

	delete impl;
}

void Post2dWindowGraphGroupDataItem::update()
{
	auto col = actorCollection();
	col->RemoveItem(impl->m_baseLinesActor.actor());
	col->RemoveItem(impl->m_graphLinesActor.linesActor());

	impl->m_baseLinesActor.actor()->VisibilityOff();
	impl->m_graphLinesActor.linesActor()->VisibilityOff();

	if (impl->m_setting.graphTarget == "") {return;}

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*> (cont->data());
	vtkPoints* basePoints = cont->data()->GetPoints();

	int dims[3];
	grid->GetDimensions(dims);

	std::vector<std::vector<QPointF> > lines;

	vtkDataArray *da = cont->data()->GetPointData()->GetArray(iRIC::toStr(impl->m_setting.graphTarget).c_str());

	vtkSmartPointer<vtkCellArray> cellsBase = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkCellArray> cellsGraph = vtkSmartPointer<vtkCellArray>::New();

	double scale = impl->m_setting.graphScale;

	std::vector<int> indices = getIndices(impl->m_setting, dims[0], dims[1]);

	if (impl->m_setting.direction == Post2dWindowGraphSetting::dirI) {
		// dirI
		for (int i : indices) {
			// base line
			std::vector<vtkIdType> baseIndices(dims[1]);
			for (int j = 0; j < dims[1]; ++j) {
				int idx = cont->nodeIndex(i, j, 0);
				baseIndices[j] = idx;
			}
			cellsBase->InsertNextCell(dims[1], baseIndices.data());

			// graph line
			std::vector<double> vals(dims[1]);
			double offset = 0;
			for (int j = 0; j < dims[1]; ++j) {
				vals[j] = da->GetTuple1(cont->nodeIndex(i, j, 0));
			}
			if (impl->m_setting.graphValueFix == Post2dWindowGraphSetting::SubtractMax) {
				offset = - *(std::max_element(vals.begin(), vals.end()));
			} else if (impl->m_setting.graphValueFix == Post2dWindowGraphSetting::SubtractMin) {
				offset = - *(std::min_element(vals.begin(), vals.end()));
			}

			std::vector<QPointF> line(dims[1]);
			for (int j = 0; j < dims[1]; ++j) {
				double p[3];
				double dx, dy;

				int idx = cont->nodeIndex(i, j, 0);
				basePoints->GetPoint(idx, p);
				getDirection(impl->m_setting, i, j, grid, &dx, &dy);

				double val = vals[j] + offset;
				double x = p[0] + dx * val * scale;
				double y = p[1] + dy * val * scale;

				line[j] = QPointF(x, y);
			}
			lines.push_back(line);
		}
	} else {
		// dirJ
		for (int j : indices) {
			// base line
			std::vector<vtkIdType> baseIndices(dims[0]);
			for (int i = 0; i < dims[0]; ++i) {
				int idx = cont->nodeIndex(i, j, 0);
				baseIndices[i] = idx;
			}
			cellsBase->InsertNextCell(dims[0], baseIndices.data());

			// graph line
			std::vector<double> vals(dims[0]);
			double offset = 0;
			for (int i = 0; i < dims[0]; ++i) {
				vals[i] = da->GetTuple1(cont->nodeIndex(i, j, 0));
			}
			if (impl->m_setting.graphValueFix == Post2dWindowGraphSetting::SubtractMax) {
				offset = - *(std::max_element(vals.begin(), vals.end()));
			} else if (impl->m_setting.graphValueFix == Post2dWindowGraphSetting::SubtractMin) {
				offset = - *(std::min_element(vals.begin(), vals.end()));
			}

			std::vector<QPointF> line(dims[0]);
			for (int i = 0; i < dims[0]; ++i) {
				double p[3];
				double dx, dy;

				int idx = cont->nodeIndex(i, j, 0);
				basePoints->GetPoint(idx, p);
				getDirection(impl->m_setting, i, j, grid, &dx, &dy);

				double val = vals[i] + offset;
				double x = p[0] + dx * val * scale;
				double y = p[1] + dy * val * scale;

				line[i] = QPointF(x, y);
			}
			lines.push_back(line);
		}
	}
	impl->m_baseLinesPolyData->SetLines(cellsBase);
	impl->m_baseLinesPolyData->Modified();

	impl->m_graphLinesActor.setLines(lines);

	impl->m_setting.gridLineStyle.applySetting(impl->m_baseLinesActor.actor()->GetProperty());
	impl->m_setting.graphLineStyle.applySetting(impl->m_graphLinesActor.linesActor()->GetProperty());

	if (impl->m_setting.drawGridLine) {
		col->AddItem(impl->m_baseLinesActor.actor());
	}
	col->AddItem(impl->m_graphLinesActor.linesActor());

	updateVisibilityWithoutRendering();
}

std::string Post2dWindowGraphGroupDataItem::target() const
{
	return impl->m_setting.graphTarget;
}

void Post2dWindowGraphGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	impl->m_setting.graphTarget = QString(target.c_str());
	update();
}

void Post2dWindowGraphGroupDataItem::setTargetWithoutSignal(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems, true);
	impl->m_setting.graphTarget = QString(target.c_str());
	update();
}

void Post2dWindowGraphGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Graph Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowGraphGroupDataItem::setDefaultSetting()
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*> (cont->data());
	int dims[3];
	grid->GetDimensions(dims);

	auto& s = impl->m_setting;
	s.direction = Post2dWindowGraphSetting::dirI;
	s.regionStartIndex = 0;
	s.regionEndIndex = dims[0] - 1;
	s.regionSkipIndex = 1;
	s.graphTarget = "";
}

void Post2dWindowGraphGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	setTargetWithoutSignal(impl->m_setting.graphTarget);
}

void Post2dWindowGraphGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

QDialog* Post2dWindowGraphGroupDataItem::propertyDialog(QWidget* p)
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*> (cont->data());
	int dims[3];
	grid->GetDimensions(dims);

	auto dialog = new Post2dWindowGraphSettingDialog(p);
	dialog->setDimensions(dims[0], dims[1]);
	dialog->setTargets(vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->GetPointData()));

	dialog->setSetting(impl->m_setting);
	return dialog;
}

void Post2dWindowGraphGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<Post2dWindowGraphSettingDialog*> (propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, true);
}

void Post2dWindowGraphGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_baseLinesActor.actor()->SetPosition(0, 0, range.min());
	impl->m_graphLinesActor.linesActor()->SetPosition(0, 0, range.max());
}

void Post2dWindowGraphGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void Post2dWindowGraphGroupDataItem::innerUpdateZScale(double scale)
{
	impl->m_baseLinesActor.actor()->SetScale(1, scale, 1);
	impl->m_graphLinesActor.linesActor()->SetScale(1, scale, 1);
}
