#include "postzonedatacsvexporter.h"
#include "../postzonedatacontainer.h"

#include <QFile>
#include <QTextStream>

#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>

QString PostZoneDataCsvExporter::filename(const QString& prefix, int index) const
{
	QString fname = prefix;
	fname.append(QString("%1.csv").arg(index));
	return fname;
}

bool PostZoneDataCsvExporter::exportToFile(PostZoneDataContainer* c, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax) const
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

	vtkPointSet* ps = c->data();
	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);
	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(ps);
	if (sgrid != 0){
		exportStructuredGrid(c, stream, sgrid, imin, imax, jmin, jmax, kmin, kmax);
	} else if (ugrid != 0){
		exportUnstructuredGrid(stream, ugrid);
	}
	f.close();
	return true;
}

void PostZoneDataCsvExporter::exportStructuredGrid(PostZoneDataContainer* c, QTextStream& stream, vtkStructuredGrid* sgrid, int imin, int imax, int jmin, int jmax, int kmin, int kmax)
{
	int dim[3];
	sgrid->GetDimensions(dim);
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
	vtkPointData* pData = sgrid->GetPointData();
	for (int i = 0; i < pData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = pData->GetArray(i);
		int comps = array->GetNumberOfComponents();
		QString name = pData->GetArrayName(i);
		if (comps == 1){
			stream << "," << name;
		} else if (comps == 2){
			stream << "," << name << "X," << name << "Y";
		} else if (comps == 3){
			stream << "," << name << "X," << name << "Y";
			if (dim[2] != 1){
				stream << "," << name << "Z";
			}
		}
	}
	stream << "\r\n";
	// data
	for (int k = kmin; k <= kmax; ++k){
		for (int j = jmin; j <= jmax; ++j){
			for (int i = imin; i <= imax; ++i){
				stream << (i + 1);
				if (dim[1] != 1){stream << "," << (j + 1);}
				if (dim[2] != 1){stream << "," << (k + 1);}
				double pos[3];
				sgrid->GetPoint(c->nodeIndex(i, j, k), pos);
				stream << "," << pos[0] << "," << pos[1];
				if (dim[2] != 1){stream << "," << pos[2];}
				for (int l = 0; l < pData->GetNumberOfArrays(); ++l){
					vtkDataArray* array = pData->GetArray(l);
					int comps = array->GetNumberOfComponents();
					if (comps == 1){
						stream << "," << array->GetTuple1(c->nodeIndex(i, j, k));
					} else if (comps == 2){
						double* tuples = array->GetTuple2(c->nodeIndex(i, j, k));
						stream << "," << *tuples << "," << *(tuples + 1);
					} else if (comps == 3){
						double* tuples = array->GetTuple3(c->nodeIndex(i, j, k));
						stream << "," << *tuples << "," << *(tuples + 1);
						if (dim[2] != 1){
							stream << "," << *(tuples + 2);
						}
					}
				}
				stream << "\r\n";
			}
		}
	}
}

void PostZoneDataCsvExporter::exportUnstructuredGrid(QTextStream& stream, vtkUnstructuredGrid* ugrid)
{
	stream << ugrid->GetNumberOfPoints();
	stream << "\r\n";
	// header
	stream << "I";
	stream << ",X";
	stream << ",Y";
	vtkPointData* pData = ugrid->GetPointData();
	for (int i = 0; i < pData->GetNumberOfArrays(); ++i){
		vtkDataArray* array = pData->GetArray(i);
		if (array == 0){continue;}
		int comps = array->GetNumberOfComponents();
		QString name = pData->GetArrayName(i);
		if (comps == 1){
			stream << "," << name;
		} else if (comps == 2){
			stream << "," << name << "X," << name << "Y";
		} else if (comps == 3){
			stream << "," << name << "X," << name << "Y";
		}
	}
	stream << "\r\n";
	// data
	for (int i = 0; i < ugrid->GetNumberOfPoints(); ++i){
		stream << i;
		double pos[3];
		ugrid->GetPoint(i, pos);
		// unstructured grid is limited to two-dimensional.
		stream << "," << pos[0] << "," << pos[1];
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
