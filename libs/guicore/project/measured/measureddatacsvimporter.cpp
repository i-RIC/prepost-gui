#include "measureddata.h"
#include "measureddatacsvimporter.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QList>
#include <QPointF>
#include <QTextCodec>
#include <QTextStream>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <unordered_set>

namespace {

struct ScalarData {
	std::string name;
	unsigned int column;
};

struct VectorData {
	std::string name;
	unsigned int columnX;
	unsigned int columnY;
};

} // namespace

MeasuredData* MeasuredDataCsvImporter::importData(const QString &filename, const QPointF& offset, ProjectDataItem* parent) const
{
	MeasuredData* ret = new MeasuredData(parent);

	QFile file(filename);
	if (! file.open(QFile::ReadOnly | QFile::Text)) {
		throw ErrorMessage(MeasuredData::tr("Error occured while opening the file."));
	}
	QTextStream stream(&file);
	QString line = stream.readLine();
	QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
	// check whether the first two are "X" and "Y".
	if (!(pieces[0] == "X" && pieces[1] == "Y")) {
		throw ErrorMessage(MeasuredData::tr("The first two columns must be \"X\" and \"Y\"."));
	}
	// check whether the header consists of ASCII charactors
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	bool ok = asciiCodec->canEncode(line);
	if (! ok) {
		throw ErrorMessage(MeasuredData::tr("The data file has to consist of only English characters."));
	}
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	QList<ScalarData> scalarDatas;
	QList<VectorData> vectorDatas;

	std::map<std::string, vtkDoubleArray*> datas;
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	std::unordered_set<unsigned int> columnsDone;
	QRegExp xname("(.*)X$");
	for (int j = 2; j < pieces.size(); ++j) {
		if (columnsDone.find(j) != columnsDone.end()) {continue;}
		bool isVector = false;
		if (xname.indexIn(pieces[j]) != -1) {
			// check whether it is vector.
			std::string vdataName = iRIC::toStr(xname.cap(1));
			std::string yname = vdataName;
			yname.append("Y");
			for (int k = 2; k < pieces.size(); ++k) {
				if (columnsDone.find(k) != columnsDone.end()) {continue;}
				if (pieces[k] == yname.c_str()) {
					// X component, Y compoment both are found.
					isVector = true;
					VectorData vData;
					vData.name = vdataName;
					vData.columnX = j;
					vData.columnY = k;
					vectorDatas.append(vData);
					vtkDoubleArray* da = vtkDoubleArray::New();
					da->SetName(vdataName.c_str());
					da->SetNumberOfComponents(3);
					datas.insert({vData.name, da});
					columnsDone.insert(j);
					columnsDone.insert(k);
					ret->vectorNames().push_back(vData.name);
				}
			}
		}
		if (! isVector) {
			ScalarData sData;
			sData.name = iRIC::toStr(pieces[j]);
			sData.column = j;
			scalarDatas.append(sData);
			vtkDoubleArray* da = vtkDoubleArray::New();
			da->SetName(sData.name.c_str());
			datas.insert({sData.name, da});
			columnsDone.insert(j);
			ret->scalarNames().push_back(sData.name);
		}
	}
	try {
		vtkIdType cellId = 0;
		do {
			line = stream.readLine();
			if (! line.isEmpty()) {
				QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
				bool ok;
				double x = pieces[0].toDouble(&ok);
				if (! ok) {throw pieces[0];}
				double y = pieces[1].toDouble(&ok);
				if (! ok) {throw pieces[1];}

				points->InsertNextPoint(x, y, 0);
				for (int i = 0; i < scalarDatas.size(); ++i) {
					ScalarData& sData = scalarDatas[i];
					vtkDoubleArray* da = datas[sData.name];
					double val = pieces[sData.column].toDouble(&ok);
					if (! ok) {throw pieces[sData.column];}
					da->InsertNextValue(val);
				}
				for (int i = 0; i < vectorDatas.size(); ++i) {
					VectorData& vData = vectorDatas[i];
					vtkDoubleArray* da = datas[vData.name];
					double valX = pieces[vData.columnX].toDouble(&ok);
					if (! ok) {throw pieces[vData.columnX];}
					double valY = pieces[vData.columnY].toDouble(&ok);
					if (! ok) {throw pieces[vData.columnY];}
					double valZ = 0;
					da->InsertNextTuple3(valX, valY, valZ);
				}
				cells->InsertNextCell(1, &cellId);
			}
			++ cellId;
		} while (! line.isEmpty());
	} catch (QString& pointData) {
		throw ErrorMessage(MeasuredData::tr("Wrong data found: %1").arg(pointData));
	}
	ret->pointData()->SetPoints(points);
	ret->pointData()->SetVerts(cells);
	for (auto pair : datas) {
		vtkDoubleArray* da = pair.second;
		ret->pointData()->GetPointData()->AddArray(da);
		da->Delete();
	}
	ret->setName(QDir::toNativeSeparators(filename));
	ret->setupPolyData();
	ret->applyOffset(offset.x(), offset.y());

	return ret;
}
