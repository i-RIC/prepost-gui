#include "../../base/iricmainwindowi.h"
#include "../../grid/v4grid.h"
#include "../../grid/v4structured2dgrid.h"
#include "../../grid/v4structured3dgrid.h"
#include "../../grid/v4unstructured2dgrid.h"
#include "../../project/projectdata.h"
#include "../../solverdef/solverdefinitiongridoutput.h"
#include "../../solverdef/solverdefinitiongridtype.h"
#include "../postsolutioninfo.h"
#include "../v4solutiongrid.h"
#include "../v4postzonedatacontainer.h"
#include "postzonedatacsvexporter.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>
#include <misc/informationdialog.h>

#include <QFile>
#include <QTextStream>
#include <QVector2D>

#include <vtkCellData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>

namespace {

void outputHeaders(SolverDefinitionGridOutput::Position pos, const QString& name, int comps, int *dim, QTextStream& stream)
{
	QString prefix = "";
	if (pos == SolverDefinitionGridOutput::Position::Node) {
		prefix = "N_";
	} else if (pos == SolverDefinitionGridOutput::Position::CellCenter) {
		prefix = "C_";
	} else if (pos == SolverDefinitionGridOutput::Position::EdgeI) {
		prefix = "I_";
	} else if (pos == SolverDefinitionGridOutput::Position::EdgeJ) {
		prefix = "J_";
	} else if (pos == SolverDefinitionGridOutput::Position::EdgeK) {
		prefix = "K_";
	}

	if (comps == 1){
		stream << "," << prefix << name;
	} else if (comps == 2){
		stream << "," << prefix << name << "X," << prefix << name << "Y";
	} else if (comps == 3){
		stream << "," << prefix << name << "X," << prefix << name << "Y";
		if (*(dim + 2) != 1){
			stream << "," << prefix << name << "Z";
		}
	}
}

void exportStructured2dGrid(v4Structured2dGrid* grid, QTextStream& stream, int imin, int imax, int jmin, int jmax, int kmin, int kmax, const QPointF& offset)
{
	int dim[3];
	auto vtkGrid = grid->vtkConcreteData()->concreteData();
	vtkGrid->GetDimensions(dim);
	stream << dim[0];
	if (dim[1] != 1){stream << "," << dim[1];}
	if (dim[2] != 1){stream << "," << dim[2];}
	stream << "\r\n";

	// header
	stream << "I";
	if (dim[1] != 1){stream << ",J";}
	if (dim[2] != 1){stream << ",K";}
	stream << ",X";
	stream << ",Y";
	if (dim[2] != 1){stream << ",Z";}

	vtkPointData* pData = vtkGrid->GetPointData();
	for (int i = 0; i < pData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = pData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::Node, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}
	vtkCellData* cData = vtkGrid->GetCellData();
	for (int i = 0; i < cData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = cData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::CellCenter, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}
	vtkCellData* ifData = grid->vtkIEdgeData()->data()->GetCellData();
	for (int i = 0; i < ifData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = ifData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::EdgeI, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}
	vtkCellData* jfData = grid->vtkJEdgeData()->data()->GetCellData();
	for (int i = 0; i < jfData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = jfData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::EdgeJ, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}

	stream << "\r\n";

	// data
	for (int k = kmin; k <= kmax; ++k){
		for (int j = jmin; j <= jmax; ++j){
			for (int i = imin; i <= imax; ++i){
				// grid shape
				stream << (i + 1);
				if (dim[1] != 1){stream << "," << (j + 1);}
				if (dim[2] != 1){stream << "," << (k + 1);}
				double pos[3];
				vtkGrid->GetPoint(grid->pointIndex(i, j), pos);
				stream << "," << pos[0] + offset.x() << "," << pos[1] + offset.y();
				if (dim[2] != 1){stream << "," << pos[2];}

				// grid node values
				for (int l = 0; l < pData->GetNumberOfArrays(); ++l){
					vtkDataArray* array = pData->GetArray(l);
					int comps = array->GetNumberOfComponents();
					if (comps == 1){
						stream << "," << array->GetTuple1(grid->pointIndex(i, j));
					} else if (comps == 2){
						double* tuples = array->GetTuple2(grid->pointIndex(i, j));
						stream << "," << *tuples << "," << *(tuples + 1);
					} else if (comps == 3){
						double* tuples = array->GetTuple3(grid->pointIndex(i, j));
						stream << "," << *tuples << "," << *(tuples + 1);
						if (dim[2] != 1){
							stream << "," << *(tuples + 2);
						}
					}
				}

				// grid cell values
				if (i == imax || j == jmax || (dim[2] != 1 && k == kmax)) {
					for (int l = 0; l < cData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = cData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << ",";
						} else if (comps == 2){
							stream << ",,";
						} else if (comps == 3){
							stream << ",,";
							if (dim[2] != 1){
								stream << ",";
							}
						}
					}
				} else {
					for (int l = 0; l < cData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = cData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << "," << array->GetTuple1(grid->cellIndex(i, j));
						} else if (comps == 2){
							double* tuples = array->GetTuple2(grid->cellIndex(i, j));
							stream << "," << *tuples << "," << *(tuples + 1);
						} else if (comps == 3){
							double* tuples = array->GetTuple3(grid->cellIndex(i, j));
							stream << "," << *tuples << "," << *(tuples + 1);
							if (dim[2] != 1){
								stream << "," << *(tuples + 2);
							}
						}
					}
				}

				// grid iedge values
				if (j == jmax) {
					for (int l = 0; l < ifData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = ifData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << ",";
						} else if (comps == 2){
							stream << ",,";
						} else if (comps == 3){
							stream << ",,";
							if (dim[2] != 1){
								stream << ",";
							}
						}
					}
				} else {
					for (int l = 0; l < ifData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = ifData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << "," << array->GetTuple1(grid->iEdgeIndex(i, j));
						} else if (comps == 2){
							double* tuples = array->GetTuple2(grid->iEdgeIndex(i, j));
							stream << "," << *tuples << "," << *(tuples + 1);
						} else if (comps == 3){
							double* tuples = array->GetTuple3(grid->iEdgeIndex(i, j));
							stream << "," << *tuples << "," << *(tuples + 1);
							if (dim[2] != 1){
								stream << "," << *(tuples + 2);
							}
						}
					}
				}

				// grid jedge values
				if (i == imax) {
					for (int l = 0; l < jfData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = jfData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << ",";
						} else if (comps == 2){
							stream << ",,";
						} else if (comps == 3){
							stream << ",,";
							if (dim[2] != 1){
								stream << ",";
							}
						}
					}
				} else {
					for (int l = 0; l < jfData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = jfData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << "," << array->GetTuple1(grid->jEdgeIndex(i, j));
						} else if (comps == 2){
							double* tuples = array->GetTuple2(grid->jEdgeIndex(i, j));
							stream << "," << *tuples << "," << *(tuples + 1);
						} else if (comps == 3){
							double* tuples = array->GetTuple3(grid->jEdgeIndex(i, j));
							stream << "," << *tuples << "," << *(tuples + 1);
							if (dim[2] != 1){
								stream << "," << *(tuples + 2);
							}
						}
					}
				}

				stream << "\r\n";
			}
		}
	}
}

void exportStructured3dGrid(v4Structured3dGrid* grid, QTextStream& stream, int imin, int imax, int jmin, int jmax, int kmin, int kmax, const QPointF& offset)
{
	int dim[3];
	auto vtkGrid = grid->vtkConcreteData()->concreteData();
	vtkGrid->GetDimensions(dim);
	stream << dim[0];
	if (dim[1] != 1){stream << "," << dim[1];}
	if (dim[2] != 1){stream << "," << dim[2];}
	stream << "\r\n";

	// header
	stream << "I";
	if (dim[1] != 1){stream << ",J";}
	if (dim[2] != 1){stream << ",K";}
	stream << ",X";
	stream << ",Y";
	if (dim[2] != 1){stream << ",Z";}

	vtkPointData* pData = vtkGrid->GetPointData();
	for (int i = 0; i < pData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = pData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::Node, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}
	vtkCellData* cData = vtkGrid->GetCellData();
	for (int i = 0; i < cData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = cData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::CellCenter, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}
	vtkCellData* ifData = grid->vtkIFaceData()-> data()->GetCellData();
	for (int i = 0; i < ifData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = ifData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::EdgeI, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}
	vtkCellData* jfData = grid->vtkJFaceData()->data()->GetCellData();
	for (int i = 0; i < jfData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = jfData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::EdgeJ, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}
	vtkCellData* kfData = grid->vtkKFaceData()->data()->GetCellData();
	for (int i = 0; i < kfData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = kfData->GetArray(i);
		outputHeaders(SolverDefinitionGridOutput::EdgeK, array->GetName(), array->GetNumberOfComponents(), &(dim[0]), stream);
	}

	stream << "\r\n";

	// data
	for (int k = kmin; k <= kmax; ++k){
		for (int j = jmin; j <= jmax; ++j){
			for (int i = imin; i <= imax; ++i){
				// grid shape
				stream << (i + 1);
				if (dim[1] != 1){stream << "," << (j + 1);}
				if (dim[2] != 1){stream << "," << (k + 1);}
				double pos[3];
				vtkGrid->GetPoint(grid->pointIndex(i, j, k), pos);
				stream << "," << pos[0] + offset.x() << "," << pos[1] + offset.y();
				if (dim[2] != 1){stream << "," << pos[2];}

				// grid node values
				for (int l = 0; l < pData->GetNumberOfArrays(); ++l){
					vtkDataArray* array = pData->GetArray(l);
					int comps = array->GetNumberOfComponents();
					if (comps == 1){
						stream << "," << array->GetTuple1(grid->pointIndex(i, j, k));
					} else if (comps == 2){
						double* tuples = array->GetTuple2(grid->pointIndex(i, j, k));
						stream << "," << *tuples << "," << *(tuples + 1);
					} else if (comps == 3){
						double* tuples = array->GetTuple3(grid->pointIndex(i, j, k));
						stream << "," << *tuples << "," << *(tuples + 1);
						if (dim[2] != 1){
							stream << "," << *(tuples + 2);
						}
					}
				}

				// grid cell values
				if (i == imax || j == jmax || (dim[2] != 1 && k == kmax)) {
					for (int l = 0; l < cData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = cData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << ",";
						} else if (comps == 2){
							stream << ",,";
						} else if (comps == 3){
							stream << ",,";
							if (dim[2] != 1){
								stream << ",";
							}
						}
					}
				} else {
					for (int l = 0; l < cData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = cData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << "," << array->GetTuple1(grid->cellIndex(i, j, k));
						} else if (comps == 2){
							double* tuples = array->GetTuple2(grid->cellIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
						} else if (comps == 3){
							double* tuples = array->GetTuple3(grid->cellIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
							if (dim[2] != 1){
								stream << "," << *(tuples + 2);
							}
						}
					}
				}

				// grid iedge values
				if (j == jmax) {
					for (int l = 0; l < ifData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = ifData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << ",";
						} else if (comps == 2){
							stream << ",,";
						} else if (comps == 3){
							stream << ",,";
							if (dim[2] != 1){
								stream << ",";
							}
						}
					}
				} else {
					for (int l = 0; l < ifData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = ifData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << "," << array->GetTuple1(grid->iFaceIndex(i, j, k));
						} else if (comps == 2){
							double* tuples = array->GetTuple2(grid->iFaceIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
						} else if (comps == 3){
							double* tuples = array->GetTuple3(grid->iFaceIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
							if (dim[2] != 1){
								stream << "," << *(tuples + 2);
							}
						}
					}
				}

				// grid jedge values
				if (i == imax) {
					for (int l = 0; l < jfData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = jfData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << ",";
						} else if (comps == 2){
							stream << ",,";
						} else if (comps == 3){
							stream << ",,";
							if (dim[2] != 1){
								stream << ",";
							}
						}
					}
				} else {
					for (int l = 0; l < jfData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = jfData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << "," << array->GetTuple1(grid->jFaceIndex(i, j, k));
						} else if (comps == 2){
							double* tuples = array->GetTuple2(grid->jFaceIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
						} else if (comps == 3){
							double* tuples = array->GetTuple3(grid->jFaceIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
							if (dim[2] != 1){
								stream << "," << *(tuples + 2);
							}
						}
					}
				}

				// grid kedge values
				if (i == imax) {
					for (int l = 0; l < kfData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = kfData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << ",";
						} else if (comps == 2){
							stream << ",,";
						} else if (comps == 3){
							stream << ",,";
							if (dim[2] != 1){
								stream << ",";
							}
						}
					}
				} else {
					for (int l = 0; l < kfData->GetNumberOfArrays(); ++l) {
						vtkDataArray* array = kfData->GetArray(l);
						int comps = array->GetNumberOfComponents();
						if (comps == 1){
							stream << "," << array->GetTuple1(grid->kFaceIndex(i, j, k));
						} else if (comps == 2){
							double* tuples = array->GetTuple2(grid->kFaceIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
						} else if (comps == 3){
							double* tuples = array->GetTuple3(grid->kFaceIndex(i, j, k));
							stream << "," << *tuples << "," << *(tuples + 1);
							if (dim[2] != 1){
								stream << "," << *(tuples + 2);
							}
						}
					}
				}

				stream << "\r\n";
			}
		}
	}
}

void exportUnstructuredGrid(v4Unstructured2dGrid* grid, QTextStream& stream, const QPointF& offset)
{
	stream << grid->nodeCount();
	stream << "\r\n";
	// header
	stream << "I";
	stream << ",X";
	stream << ",Y";
	vtkPointData* pData = grid->vtkConcreteData()->data()->GetPointData();
	for (int i = 0; i < pData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = pData->GetArray(i);
		if (array == nullptr) {continue;}
		int comps = array->GetNumberOfComponents();
		if (comps == 1){
			stream << "," << "N_" << array->GetName();
		} else if (comps == 2){
			stream << "," << "N_" << array->GetName() << "X," << "N_" << array->GetName() << "Y";
		} else if (comps == 3){
			stream << "," << "N_" << array->GetName() << "X," << "N_" << array->GetName() << "Y," << "N_" << array->GetName() << "Z";
		}
	}
	stream << "\r\n";
	// data
	for (int i = 0; i < grid->nodeCount(); ++i){
		stream << i;
		double pos[3];
		grid->vtkData()->data()->GetPoint(i, pos);
		// unstructured grid is limited to two-dimensional.
		stream << "," << pos[0] + offset.x() << "," << pos[1] + offset.y();
		for (int l = 0; l < pData->GetNumberOfArrays(); ++l){
			vtkDataArray* array = pData->GetArray(l);
			if (array == 0){continue;}
			int comps = array->GetNumberOfComponents();
			if (comps == 1){
				stream << "," << array->GetTuple1(i);
			} else if (comps == 2){
				double* tuples = array->GetTuple2(i);
				stream << "," << *tuples << "," << *(tuples + 1);
			} else if (comps == 3){
				double* tuples = array->GetTuple3(i);
				stream << "," << *tuples << "," << *(tuples + 1);
			}
		}
		stream << "\r\n";
	}
}

} // namespace


PostZoneDataCsvExporter::PostZoneDataCsvExporter()
{}

PostZoneDataCsvExporter::~PostZoneDataCsvExporter()
{}

QString PostZoneDataCsvExporter::filename(const QString& prefix, int index) const
{
	QString fname = prefix;
	fname.append(QString("%1.csv").arg(index));
	return fname;
}

bool PostZoneDataCsvExporter::exportToFile(v4PostZoneDataContainer* c, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* projectData, const QPointF& offset) const
{
	if (QFile::exists(filename)){
		bool ok = QFile::remove(filename);
		if (! ok){
			return false;
		}
	}

	QFile f(filename);
	bool ok = f.open(QIODevice::WriteOnly);
	if (! ok){return false;}
	QTextStream stream(&f);
	stream.setRealNumberPrecision(12);
	stream << "iRIC output t = " << time << "\r\n";

	auto grid = c->gridData()->grid();
	auto sgrid2d = dynamic_cast<v4Structured2dGrid*> (grid);
	auto sgrid3d = dynamic_cast<v4Structured3dGrid*> (grid);
	auto ugrid = dynamic_cast<v4Unstructured2dGrid*> (grid);

	if (sgrid2d != nullptr) {
		exportStructured2dGrid(sgrid2d, stream, imin, imax, jmin, jmax, kmin, kmax, offset);
	} else if (sgrid3d != nullptr) {
		exportStructured3dGrid(sgrid3d, stream, imin, imax, jmin, jmax, kmin, kmax, offset);
	} else if (ugrid != nullptr) {
		exportUnstructuredGrid(ugrid, stream, offset);
	}
	f.close();

	if (m_isFirstStep) {
		InformationDialog::warning(projectData->mainWindow(), PostSolutionInfo::tr("Warning"),
															 PostSolutionInfo::tr("CSV files export calculation result defined at grid nodes, cells, and edges."),
															 "postzonedatacsvexporter_warning");
	}

	return true;
}
