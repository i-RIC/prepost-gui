#include "../graph2dhybridwindow.h"
#include "../graph2dhybridwindowcontrolwidget.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowgridijkresultdataitem.h"
#include "graph2dhybridwindowresultcopydataitem.h"
#include "graph2dhybridwindowresultdataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/stringtool.h>
#include <misc/stringtool.h>

#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include <cmath>
#include <qwt_plot_curve.h>

namespace {

struct Point3D {
	Point3D():
		Point3D {0, 0, 0}
	{}
	Point3D(double xx, double yy, double zz):
		x {xx}, y {yy}, z {zz}
	{}
	Point3D(double* p) :
		x {*(p)}, y {*(p + 1)}, z {*(p + 2)}
	{}

	double x;
	double y;
	double z;
};

Point3D getCenter(vtkCell* cell)
{
	double v[3], p[3];
	for (int i = 0; i < 3; ++i) {
		v[i] = 0;
	}

	auto points = cell->GetPoints();
	int num_p = cell->GetNumberOfPoints();
	for (int i = 0; i < num_p; ++i) {
		points->GetPoint(i, p);
		for (int j = 0; j < 3; ++j) {
			v[j] += p[j];
		}
	}
	return Point3D(v[0] / num_p, v[1] / num_p, v[2] / num_p);
}

} // namespace

Graph2dHybridWindowGridIJKResultDataItem::Graph2dHybridWindowGridIJKResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent) :
	Graph2dHybridWindowResultDataItem(setting.caption(), index, setting, parent)
{
	m_physVal = setting.name().c_str();
}

Graph2dHybridWindowGridIJKResultDataItem::~Graph2dHybridWindowGridIJKResultDataItem()
{}

void Graph2dHybridWindowGridIJKResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void Graph2dHybridWindowGridIJKResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void Graph2dHybridWindowGridIJKResultDataItem::updateValues()
{
	m_xValues.clear();
	m_yValues.clear();

	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();

	auto postInfo = dataModel()->postSolutionInfo();
	auto cont = postInfo->v4ZoneContainer(info->dimension, info->zoneName);
	if (cont == nullptr) {return;}

	auto grid = cont->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (grid);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (grid);

	vtkDataSetAttributes* atts = nullptr;
	std::vector<Point3D> points;
	std::vector<vtkIdType> ids;
	if (info->gridLocation == iRICLib::H5CgnsZone::SolutionPosition::Node) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			if (sGrid2d != nullptr) {
				for (vtkIdType i = 0; i < sGrid2d->dimensionI(); ++i) {
					ids.push_back(sGrid2d->pointIndex(i, s.gridJ()));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType i = 0; i < sGrid3d->dimensionI(); ++i) {
					ids.push_back(sGrid3d->pointIndex(i, s.gridJ(), s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			if (sGrid2d != nullptr) {
				for (vtkIdType j = 0; j < sGrid2d->dimensionJ(); ++j) {
					ids.push_back(sGrid2d->pointIndex(s.gridI(), j));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType j = 0; j < sGrid3d->dimensionJ(); ++j) {
					ids.push_back(sGrid3d->pointIndex(s.gridI(), j, s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			for (vtkIdType k = 0; k < sGrid3d->dimensionK(); ++k) {
				ids.push_back(sGrid3d->pointIndex(s.gridI(), s.gridJ(), k));
			}
		}
		for (auto id : ids) {
			points.push_back(Point3D(grid->vtkData()->data()->GetPoint(id)));
		}
		atts = grid->vtkData()->data()->GetPointData();
	} else if (info->gridLocation == iRICLib::H5CgnsZone::SolutionPosition::Cell) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			if (sGrid2d != nullptr) {
				for (vtkIdType i = 0; i < sGrid2d->dimensionI() - 1; ++i) {
					ids.push_back(sGrid2d->cellIndex(i, s.gridJ()));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType i = 0; i < sGrid3d->dimensionI() - 1; ++i) {
					ids.push_back(sGrid3d->cellIndex(i, s.gridJ(), s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			if (sGrid2d != nullptr) {
				for (vtkIdType j = 0; j < sGrid2d->dimensionJ() - 1; ++j) {
					ids.push_back(sGrid2d->cellIndex(s.gridI(), j));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType j = 0; j < sGrid3d->dimensionJ() - 1; ++j) {
					ids.push_back(sGrid3d->cellIndex(s.gridI(), j, s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			for (vtkIdType k = 0; k < sGrid3d->dimensionK() - 1; ++k) {
				ids.push_back(sGrid3d->cellIndex(s.gridI(), s.gridJ(), k));
			}
		}
		for (auto id : ids) {
			points.push_back(getCenter(grid->vtkData()->data()->GetCell(id)));
		}
		atts = grid->vtkData()->data()->GetCellData();
	} else if (info->gridLocation == iRICLib::H5CgnsZone::SolutionPosition::IFace) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			if (sGrid2d != nullptr) {
				for (vtkIdType i = 0; i < sGrid2d->dimensionI(); ++i) {
					ids.push_back(sGrid2d->iEdgeIndex(i, s.gridJ()));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType i = 0; i < sGrid3d->dimensionI(); ++i) {
					ids.push_back(sGrid3d->iFaceIndex(i, s.gridJ(), s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			if (sGrid2d != nullptr) {
				for (vtkIdType j = 0; j < sGrid2d->dimensionJ() - 1; ++j) {
					ids.push_back(sGrid2d->iEdgeIndex(s.gridI(), j));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType j = 0; j < sGrid3d->dimensionJ() - 1; ++j) {
					ids.push_back(sGrid3d->iFaceIndex(s.gridI(), j, s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			for (vtkIdType k = 0; k < sGrid3d->dimensionK() - 1; ++k) {
				ids.push_back(sGrid3d->iFaceIndex(s.gridI(), s.gridJ(), k));
			}
		}

		if (sGrid2d != nullptr) {
			for (auto id : ids) {
				points.push_back(getCenter(sGrid2d->vtkIEdgeData()->data()->GetCell(id)));
			}
			atts = sGrid2d->vtkIEdgeData()->data()->GetCellData();
		} else if (sGrid3d != nullptr) {
			for (auto id : ids) {
				points.push_back(getCenter(sGrid3d->vtkIFaceData()->data()->GetCell(id)));
			}
			atts = sGrid3d->vtkIFaceData()->data()->GetCellData();
		}
	} else if (info->gridLocation == iRICLib::H5CgnsZone::SolutionPosition::JFace) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			if (sGrid2d != nullptr) {
				for (vtkIdType i = 0; i < sGrid2d->dimensionI() - 1; ++i) {
					ids.push_back(sGrid2d->jEdgeIndex(i, s.gridJ()));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType i = 0; i < sGrid3d->dimensionI() - 1; ++i) {
					ids.push_back(sGrid3d->jFaceIndex(i, s.gridJ(), s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			if (sGrid2d != nullptr) {
				for (vtkIdType j = 0; j < sGrid2d->dimensionJ(); ++j) {
					ids.push_back(sGrid2d->jEdgeIndex(s.gridI(), j));
				}
			} else if (sGrid3d != nullptr) {
				for (vtkIdType j = 0; j < sGrid3d->dimensionJ(); ++j) {
					ids.push_back(sGrid3d->jFaceIndex(s.gridI(), j, s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			for (vtkIdType k = 0; k < sGrid3d->dimensionK() - 1; ++k) {
				ids.push_back(sGrid3d->jFaceIndex(s.gridI(), s.gridJ(), k));
			}
		}

		if (sGrid2d != nullptr) {
			for (auto id : ids) {
				points.push_back(getCenter(sGrid2d->vtkJEdgeData()->data()->GetCell(id)));
			}
			atts = sGrid2d->vtkJEdgeData()->data()->GetCellData();
		} else if (sGrid3d != nullptr) {
			for (auto id : ids) {
				points.push_back(getCenter(sGrid3d->vtkJFaceData()->data()->GetCell(id)));
			}
			atts = sGrid3d->vtkJFaceData()->data()->GetCellData();
		}
	} else if (info->gridLocation == iRICLib::H5CgnsZone::SolutionPosition::KFace) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			if (sGrid3d != nullptr) {
				for (vtkIdType i = 0; i < sGrid3d->dimensionI() - 1; ++i) {
					ids.push_back(sGrid3d->kFaceIndex(i, s.gridJ(), s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			if (sGrid3d != nullptr) {
				for (vtkIdType j = 0; j < sGrid3d->dimensionJ() - 1; ++j) {
					ids.push_back(sGrid3d->kFaceIndex(s.gridI(), j, s.gridK()));
				}
			}
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			for (vtkIdType k = 0; k < sGrid3d->dimensionK(); ++k) {
				ids.push_back(sGrid3d->kFaceIndex(s.gridI(), s.gridJ(), k));
			}
		}

		if (sGrid3d != nullptr) {
			for (auto id : ids) {
				points.push_back(getCenter(sGrid3d->vtkKFaceData()->data()->GetCell(id)));
			}
			atts = sGrid3d->vtkKFaceData()->data()->GetCellData();
		}
	}

	m_xValues.assign(ids.size(), 0);
	m_yValues.assign(ids.size(), 0);

	vtkDataArray* data = atts->GetArray(iRIC::toStr(m_physVal).c_str());
	if (data == nullptr) {
		return;
	}

	double distance = 0;
	Point3D oldP;
	for (int i = 0; i < static_cast<int>(ids.size()); ++i) {
		vtkIdType id = ids.at(i);
		Point3D p = points.at(i);
		if (i == 0) {
			oldP = p;
		}
		auto dx = p.x - oldP.x;
		auto dy = p.y - oldP.y;
		auto dz = p.z - oldP.z;
		distance = distance + std::sqrt(dx * dx + dy * dy + dz * dz);

		m_xValues[i] = distance;
		double value = 0;
		if (data->GetNumberOfComponents() == 1) {
			value = data->GetTuple1(id);
		} else if (data->GetNumberOfComponents() == 3) {
			double* v = data->GetTuple3(id);
			value = std::sqrt(*v * *v + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
		}
		m_yValues[i] = value;

		oldP = p;
	}
}

Graph2dHybridWindowResultCopyDataItem* Graph2dHybridWindowGridIJKResultDataItem::copy(Graph2dHybridWindowResultCopyGroupDataItem* p)
{
	Graph2dHybridWindowResultCopyDataItem* ret = Graph2dHybridWindowResultDataItem::copy(p);

	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();

	PostSolutionInfo* postInfo = dataModel()->postSolutionInfo();
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(dataModel()->mainWindow());
	Graph2dHybridWindowControlWidget* c = w->controlWidget();

	int currentStep = postInfo->currentStep();
	const QList<double>& timesteps = postInfo->timeSteps()->timesteps();
	double time;
	if (timesteps.count() == 0) {
		time = 0;
	} else {
		if (currentStep < timesteps.count()) {
			time = timesteps.at(currentStep);
		} else {
			time = 0;
		}
	}

	QStringList args;
	args.append(m_standardItem->text());
	args.append(QString::number(time));
	if (info->dataType == Graph2dHybridWindowResultSetting::dtDim1DStructured) {
		ret->setId(Graph2dHybridWindowResultCopyDataItem::idt1D, args);
	} else if (info->dataType == Graph2dHybridWindowResultSetting::dtDim2DStructured) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			args.append(QString::number(c->jValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtI2D, args);
		} else {
			args.append(QString::number(c->iValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtJ2D, args);
		}
	} else if (info->dataType == Graph2dHybridWindowResultSetting::dtDim3DStructured) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			args.append(QString::number(c->jValue() + 1));
			args.append(QString::number(c->kValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtI3D, args);
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			args.append(QString::number(c->iValue() + 1));
			args.append(QString::number(c->kValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtJ3D, args);
		} else {
			args.append(QString::number(c->iValue() + 1));
			args.append(QString::number(c->jValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtK3D, args);
		}
	}
	return ret;
}
