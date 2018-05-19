#include "measureddata.h"
#include "measureddatacsvexporter.h"

#include <misc/errormessage.h>

#include <QFile>
#include <QTextStream>
#include <QVector>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>

void MeasuredDataCsvExporter::exportData(const QString& filename, const MeasuredData& data) const
{
	QFile file(filename);
	if (! file.open(QFile::WriteOnly | QFile::Text)) {
		throw ErrorMessage(MeasuredData::tr("Error occured while opening the file."));
	}

	QTextStream stream(&file);
	stream.setRealNumberPrecision(12);
	auto vtkData = data.pointData();
	vtkPointData* pd = vtkData->GetPointData();
	int nArrays = pd->GetNumberOfArrays();

	// output header.
	QStringList headers;
	vtkIdType dataCount;
	headers << "X" << "Y";
	for (int i = 0; i < nArrays; ++i) {
		vtkDoubleArray* da = vtkDoubleArray::SafeDownCast(pd->GetArray(i));
		if (da->GetNumberOfComponents() == 3) {
			// Vector component
			QString compName = da->GetName();
			QString compNameX = compName;
			compNameX.append("X");
			QString compNameY = compName;
			compNameY.append("Y");
			headers << compNameX << compNameY;
		} else if (da->GetNumberOfComponents() == 1) {
			QString compName = da->GetName();
			headers << compName;
		}
		if (i == 0) {
			dataCount = da->GetNumberOfTuples();
		}
	}
	stream << headers.join(",") << endl;

	// output data.
	for (vtkIdType i = 0; i < dataCount; ++i) {
		QVector<double> vals;
		// output X, Y
		double* p = vtkData->GetPoint(i);
		vals << *(p);
		vals << *(p + 1);

		// output scalar and vector data.
		for (int j = 0; j < nArrays; ++j) {
			vtkDoubleArray* da = vtkDoubleArray::SafeDownCast(pd->GetArray(j));
			if (da->GetNumberOfComponents() == 3) {
				double* tuple = da->GetTuple3(i);
				vals << *(tuple);
				vals << *(tuple + 1);
			} else if (da->GetNumberOfComponents() == 1) {
				vals << da->GetTuple1(i);
			}
		}
		for (int j = 0; j < vals.size(); ++j) {
			if (j != 0) {stream << ",";}
			stream << vals.at(j);
		}
		stream << endl;
	}
	file.close();
}
