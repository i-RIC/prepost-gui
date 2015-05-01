#include "postdummy3dzonedatacontainer.h"
#include "postsolutioninfo.h"
#include "../project/projectcgnsfile.h"
#include <misc/stringtool.h>
#include <cgnslib.h>
#include <iriclib.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetAttributes.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkIntArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <QRegExp>

bool PostDummy3DZoneDataContainer::loadStructuredGrid(const int fn, const int currentStep)
{
	if (! m_loadedOnce) {
		m_data = vtkSmartPointer<vtkStructuredGrid>::New();
	} else {
		m_data->Initialize();
	}
	vtkPointSet* p1 = m_data;
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(p1);
	int NVertexI, NVertexJ, NVertexK;

	NVertexI = m_sizes[0];
	NVertexJ = m_sizes[1];
	NVertexK = KMAX;

	grid->SetDimensions(NVertexI, NVertexJ, NVertexK);
	// Find zone iterative data.
	char zoneItername[32];
	int ier;
	ier = cg_ziter_read(fn, m_baseId, m_zoneId, zoneItername);
	bool iterativeCoordinates = false;
	if (ier == 0) {
		// zone iterative data exists.
		// try to read the array information of GridCoordinagesPointers.
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, zoneItername, 0, "end");
		if (ier != 0) {return false;}
		int narrays;
		ier = cg_narrays(&narrays);
		if (ier != 0) {return false;}
		for (int i = 1; i <= narrays; ++i) {
			char arrayname[ProjectCgnsFile::BUFFERLEN];
			DataType_t dataType;
			int dimension;
			cgsize_t dimVector[3];
			cg_array_info(i, arrayname, &dataType, &dimension, dimVector);
			if (QString(arrayname) == "GridCoordinatesPointers") {
				// GridCoordinatesPointers found.
				char* pointers;
				// dimension = 2, dimVector = [32, NumberOfSteps].
				pointers = new char[dimVector[0] * dimVector[1]];
				cg_array_read(i, pointers);
				char currentCoordinates[32];
				memcpy(currentCoordinates, &(pointers[32 * currentStep]), 32);
				// currentCoordinates is not null terminated.
				currentCoordinates[31] = '\0';
				QString curCoord = QString(currentCoordinates).trimmed();
				delete pointers;
				// now, goto the specified coordinates node.
				ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, iRIC::toStr(curCoord).c_str(), 0, "end");
				iterativeCoordinates = true;
			}
		}
	} else {
		strcpy(zoneItername, "");
	}
	if (! iterativeCoordinates) {
		// The grid shape used as input grid is also used as result grid.
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "GridCoordinates", 0, "end");
	}
	if (ier != 0) {
		// grid data does not exists.
		return false;
	}
	// get array info in order to know what data type is used.
	// assume that x, y and z are the same type.
	char aName[ProjectCgnsFile::BUFFERLEN];
	DataType_t dType;
	int d;
	cgsize_t dVector[3];
	cg_array_info(1, aName, &dType, &d, dVector);

	double* dataX, *dataY, *dataZ;
	size_t numPoints = NVertexI * NVertexJ * 1;
	dataX = new double[numPoints];
	dataY = new double[numPoints];
	dataZ = new double[numPoints];

	// RealSingle or RealDouble
	if (dType == RealSingle) {
		float* tmpX, *tmpY, *tmpZ;
		tmpX = new float[numPoints];
		tmpY = new float[numPoints];
		tmpZ = new float[numPoints];

		// the first one must be X.
		ier = cg_array_read(1, tmpX);
		if (m_cellDim >= 2) {
			// the second one must be Y.
			ier = cg_array_read(2, tmpY);
		} else {
			for (unsigned int i = 0; i < numPoints; ++i) {*(tmpY + i) = 0;}
		}
		if (m_cellDim == 3) {
			// the third one must be Z.
			ier = cg_array_read(3, tmpZ);
		} else {
			for (unsigned int i = 0; i < numPoints; ++i) {*(tmpZ + i) = 0;}
		}
		for (unsigned int i = 0; i < numPoints; i++) {
			*(dataX + i) = *(tmpX + i);
			*(dataY + i) = *(tmpY + i);
			*(dataZ + i) = *(tmpZ + i);
		}

		delete[] tmpX;
		delete[] tmpY;
		delete[] tmpZ;
	} else {
		// the first one must be X.
		ier = cg_array_read(1, dataX);

		if (m_cellDim >= 2) {
			// the second one must be Y.
			ier = cg_array_read(2, dataY);
		} else {
			for (unsigned int i = 0; i < numPoints; ++i) {*(dataY + i) = 0;}
		}
		if (m_cellDim == 3) {
			// the third one must be Z.
			ier = cg_array_read(3, dataZ);
		} else {
			for (unsigned int i = 0; i < numPoints; ++i) {*(dataZ + i) = 0;}
		}
	}

	vtkPoints* points = vtkPoints::New();
	for (int k = 0; k < NVertexK; ++k) {
		for (int j = 0; j < NVertexJ; ++j) {
			for (int i = 0; i < NVertexI; ++i) {
				points->InsertNextPoint(
					*(dataX + i + NVertexI * j),
					*(dataY + i + NVertexI * j),
					*(dataZ + i + NVertexI * j) + k * ZWIDTH);
			}
		}
	}
	grid->SetPoints(points);
	points->Delete();
	delete[] dataX;
	delete[] dataY;
	delete[] dataZ;
	return true;
}

bool PostDummy3DZoneDataContainer::loadScalarData(const int fn, const int solid)
{
	int ier;
	char solname[32];
	GridLocation_t location;
	ier = cg_sol_info(fn, m_baseId, m_zoneId, solid, solname, &location);
	if (ier != 0) {return false;}
	ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "FlowSolution_t", solid, "end");
	if (ier != 0) {return false;}
	vtkDataSetAttributes* data;
	if (location == Vertex) {
		// vertex.
		data = m_data->GetPointData();
	} else {
		// cell center.
		data = m_data->GetCellData();
	}
	int narrays;
	cg_narrays(&narrays);
	for (int i = 1; i <= narrays; ++i) {
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		char arrayname[30];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		QString name(arrayname);

		// skip vector values.
		QRegExp rx;
		// For example, "VelocityX"
		rx = QRegExp("(.*)X$");
		if (rx.indexIn(name) != -1) {continue;}
		// For example, "VelocityY"
		rx = QRegExp("(.*)Y$");
		if (rx.indexIn(name) != -1) {continue;}
		// For example, "VelocityZ"
		rx = QRegExp("(.*)Z$");
		if (rx.indexIn(name) != -1) {continue;}

		int datalen = 1;
		for (int j = 0; j < dimension; ++j) {
			datalen *= dimVector[j];
		}
		if (datatype == Integer) {
			int* data2 = new int[datalen];
			cg_array_read(i, data2);
			vtkIntArray* tmpArray = vtkIntArray::New();
			tmpArray->SetName(arrayname);
			tmpArray->Allocate(datalen);
			for (int k = 0; k < KMAX; ++k) {
				for (int j = 0; j < datalen; ++j) {
					tmpArray->InsertNextValue(*(data2 + j));
				}
			}
			delete data2;
			data->AddArray(tmpArray);
			tmpArray->Delete();
		} else if (datatype == RealSingle) {
			float* data2 = new float[datalen];
			cg_array_read(i, data2);
			vtkFloatArray* tmpArray = vtkFloatArray::New();
			tmpArray->SetName(arrayname);
			tmpArray->Allocate(datalen);
			for (int k = 0; k < KMAX; ++k) {
				for (int j = 0; j < datalen; ++j) {
					tmpArray->InsertNextValue(*(data2 + j));
				}
			}
			delete data2;
			data->AddArray(tmpArray);
			tmpArray->Delete();
		} else if (datatype == RealDouble) {
			double* data2 = new double[datalen];
			cg_array_read(i, data2);
			vtkDoubleArray* tmpArray = vtkDoubleArray::New();
			tmpArray->SetName(arrayname);
			tmpArray->Allocate(datalen);
			for (int k = 0; k < KMAX; ++k) {
				for (int j = 0; j < datalen; ++j) {
					tmpArray->InsertNextValue(*(data2 + j));
				}
			}
			delete data2;
			data->AddArray(tmpArray);
			tmpArray->Delete();
		}
	}
	return true;
}


bool PostDummy3DZoneDataContainer::loadVectorData(const int fn, const int solid)
{
	int ier;
	char solname[32];
	GridLocation_t location;
	ier = cg_sol_info(fn, m_baseId, m_zoneId, solid, solname, &location);
	if (ier != 0) {return false;}
	ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "FlowSolution_t", solid, "end");
	if (ier != 0) {return false;}
	vtkDataSetAttributes* data;
	if (location == Vertex) {
		// vertex.
		data = m_data->GetPointData();
	} else {
		// cell center.
		data = m_data->GetCellData();
	}
	int narrays;
	cg_narrays(&narrays);

	// try to find vector attributes.
	for (int i = 1; i <= narrays; ++i) {
		char arrayname[30];
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		QString name(arrayname);

		QRegExp rx;
		// find solution with name "VelocityX", for example.
		rx = QRegExp("(.*)X$");
		if (rx.indexIn(name) != -1) {
			int indexX, indexY, indexZ;
			// vector data found.
			QString vectorName = rx.cap(1);

			indexX = i;
			indexY = 0;
			indexZ = 0;
			// try to find Y component. i.e. "VelocityY"
			QString yname = vectorName;
			yname.append("Y");
			for (int j = 1; j <= narrays; ++j) {
				char arrayname2[30];
				DataType_t datatype2;
				int dimension2;
				cgsize_t dimVector2[3];
				cg_array_info(j, arrayname2, &datatype2, &dimension2, dimVector2);
				if (yname == arrayname2 && datatype == datatype2 && dimension == dimension2) {
					indexY = j;
				}
			}
			if (indexY != 0) {
				// try to find Z component. i.e. "VelocityZ"
				QString zname = vectorName;
				zname.append("Z");
				for (int j = 1; j <= narrays; ++j) {
					char arrayname2[30];
					DataType_t datatype2;
					int dimension2;
					cgsize_t dimVector2[3];
					cg_array_info(j, arrayname2, &datatype2, &dimension2, dimVector2);
					if (zname == arrayname2 && datatype == datatype2 && dimension == dimension2) {
						indexZ = j;
					}
				}
			}
			// OK, indexX, indexY, indexZ are set correctly.
			unsigned int datalen = 1;
			for (int j = 0; j < dimension; ++j) {
				datalen *= dimVector[j];
			}
			if (datatype == Integer) {
				int* dataX = new int[datalen];
				int* dataY = new int[datalen];
				int* dataZ = new int[datalen];
				double* dataMag = new double[datalen];
				// read x.
				cg_array_read(indexX, dataX);
				// read y.
				if (indexY != 0) {
					cg_array_read(indexY, dataY);
				} else {
					for (unsigned int j = 0; j < datalen; ++j) {*(dataY + j) = 0;}
				}
				// read z
				if (indexZ != 0) {
					cg_array_read(indexZ, dataZ);
				} else {
					for (unsigned int j = 0; j < datalen; ++j) {*(dataZ + j) = 0;}
				}
				for (unsigned int j = 0; j < datalen; ++j) {
					double sum = 0;
					sum += *(dataX + j) * (*(dataX + j));
					sum += *(dataY + j) * (*(dataY + j));
					sum += *(dataZ + j) * (*(dataZ + j));
					double v = sqrt(sum);
					*(dataMag + j) = v;
				}
				vtkIntArray* tmpArray = vtkIntArray::New();
				tmpArray->SetName(iRIC::toStr(vectorName).c_str());
				tmpArray->SetNumberOfComponents(3);
				tmpArray->Allocate(datalen);
				for (int k = 0; k < KMAX; ++k) {
					for (unsigned int j = 0; j < datalen; ++j) {
						tmpArray->InsertNextTuple3(*(dataX + j), *(dataY + j), *(dataZ + j));
					}
				}
				delete dataX;
				delete dataY;
				delete dataZ;
				data->AddArray(tmpArray);
				tmpArray->Delete();

				vtkDoubleArray* magArray = vtkDoubleArray::New();
				magArray->SetName(iRIC::toStr(vectorName.append(" (magnitude)")).c_str());
				magArray->Allocate(datalen);
				for (int k = 0; k < KMAX; ++k) {
					for (unsigned int j = 0; j < datalen; ++j) {
						magArray->InsertNextValue(*(dataMag + j));
					}
				}
				delete dataMag;
				data->AddArray(magArray);
				magArray->Delete();
			} else if (datatype == RealSingle) {
				float* dataX = new float[datalen];
				float* dataY = new float[datalen];
				float* dataZ = new float[datalen];
				double* dataMag = new double[datalen];
				// read x.
				cg_array_read(indexX, dataX);
				// read y.
				if (indexY != 0) {
					cg_array_read(indexY, dataY);
				} else {
					for (unsigned int j = 0; j < datalen; ++j) {*(dataY + j) = 0;}
				}
				// read z
				if (indexZ != 0) {
					cg_array_read(indexZ, dataZ);
				} else {
					for (unsigned int j = 0; j < datalen; ++j) {*(dataZ + j) = 0;}
				}
				for (unsigned int j = 0; j < datalen; ++j) {
					double sum = 0;
					sum += *(dataX + j) * (*(dataX + j));
					sum += *(dataY + j) * (*(dataY + j));
					sum += *(dataZ + j) * (*(dataZ + j));
					double v = sqrt(sum);
					*(dataMag + j) = v;
				}
				vtkFloatArray* tmpArray = vtkFloatArray::New();
				tmpArray->SetName(iRIC::toStr(vectorName).c_str());
				tmpArray->SetNumberOfComponents(3);
				tmpArray->Allocate(datalen);
				for (int k = 0; k < KMAX; ++k) {
					for (unsigned int j = 0; j < datalen; ++j) {
						tmpArray->InsertNextTuple3(*(dataX + j), *(dataY + j), *(dataZ + j));
					}
				}
				delete dataX;
				delete dataY;
				delete dataZ;
				data->AddArray(tmpArray);
				tmpArray->Delete();

				vtkDoubleArray* magArray = vtkDoubleArray::New();
				magArray->SetName(iRIC::toStr(vectorName.append(" (magnitude)")).c_str());
				magArray->Allocate(datalen);
				for (int k = 0; k < KMAX; ++k) {
					for (unsigned int j = 0; j < datalen; ++j) {
						magArray->InsertNextValue(*(dataMag + j));
					}
				}
				delete dataMag;
				data->AddArray(magArray);
				magArray->Delete();
			} else if (datatype == RealDouble) {
				double* dataX = new double[datalen];
				double* dataY = new double[datalen];
				double* dataZ = new double[datalen];
				double* dataMag = new double[datalen];
				// read x.
				cg_array_read(indexX, dataX);
				// read y.
				if (indexY != 0) {
					cg_array_read(indexY, dataY);
				} else {
					for (unsigned int j = 0; j < datalen; ++j) {*(dataY + j) = 0;}
				}
				// read z
				if (indexZ != 0) {
					cg_array_read(indexZ, dataZ);
				} else {
					for (unsigned int j = 0; j < datalen; ++j) {*(dataZ + j) = 0;}
				}
				for (unsigned int j = 0; j < datalen; ++j) {
					double sum = 0;
					sum += *(dataX + j) * (*(dataX + j));
					sum += *(dataY + j) * (*(dataY + j));
					sum += *(dataZ + j) * (*(dataZ + j));
					double v = sqrt(sum);
					*(dataMag + j) = v;
				}
				vtkDoubleArray* tmpArray = vtkDoubleArray::New();
				tmpArray->SetName(iRIC::toStr(vectorName).c_str());
				tmpArray->SetNumberOfComponents(3);
				tmpArray->Allocate(datalen);
				for (int k = 0; k < KMAX; ++k) {
					for (unsigned int j = 0; j < datalen; ++j) {
						tmpArray->InsertNextTuple3(*(dataX + j), *(dataY + j), *(dataZ + j));
					}
				}
				delete dataX;
				delete dataY;
				delete dataZ;
				data->AddArray(tmpArray);
				tmpArray->Delete();

				vtkDoubleArray* magArray = vtkDoubleArray::New();
				magArray->SetName(iRIC::toStr(vectorName.append(" (magnitude)")).c_str());
				magArray->Allocate(datalen);
				for (int k = 0; k < KMAX; ++k) {
					for (unsigned int j = 0; j < datalen; ++j) {
						magArray->InsertNextValue(*(dataMag + j));
					}
				}
				delete dataMag;
				data->AddArray(magArray);
				magArray->Delete();
			}
		}
	}
	return true;
}

