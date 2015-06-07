#include "../base/iricmainwindowinterface.h"
#include "measureddata.h"

#include <guibase/waitdialog.h>
#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QCoreApplication>
#include <QDir>
#include <QDomNode>
#include <QFile>
#include <QMap>
#include <QRegExp>
#include <QSet>
#include <QTextCodec>
#include <QTextStream>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>

#define REAL double
#define VOID void

#include <triangle/triangle.h>
#include <triangle/triangleexecutethread.h>

MeasuredData::MeasuredData(ProjectDataItem* parent)
	: ProjectDataItem(parent)
{
	m_pointData = vtkSmartPointer<vtkPolyData>::New();
	m_polyData = vtkSmartPointer<vtkPolyData>::New();
	m_noPolyData = false;
}

struct ScalarData {
	QString name;
	unsigned int column;
};

struct VectorData {
	QString name;
	unsigned int columnX;
	unsigned int columnY;
};

void MeasuredData::importFromFile(const QString& filename)
{
	QFile file(filename);
	if (! file.open(QFile::ReadOnly | QFile::Text)) {
		throw ErrorMessage(tr("Error occured while opening the file."));
	}
	QTextStream stream(&file);
	QString line = stream.readLine();
	QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
	// check whether the first two are "X" and "Y".
	if (!(pieces[0] == "X" && pieces[1] == "Y")) {
		throw ErrorMessage(tr("The first two columns must be \"X\" and \"Y\"."));
	}
	// check whether the header consists of ASCII charactors
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	bool ok = asciiCodec->canEncode(line);
	if (! ok) {
		throw ErrorMessage(tr("The data file has to consist of only English characters."));
	}
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	QList<ScalarData> scalarDatas;
	QList<VectorData> vectorDatas;

	QMap<QString, vtkDoubleArray*> datas;
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	QSet<unsigned int> columnsDone;
	QRegExp xname("(.*)X$");
	for (int j = 2; j < pieces.size(); ++j) {
		if (columnsDone.contains(j)) {continue;}
		bool isVector = false;
		if (xname.indexIn(pieces[j]) != -1) {
			// check whether it is vector.
			QString vdataName = xname.cap(1);
			QString yname = vdataName;
			yname.append("Y");
			for (int k = 2; k < pieces.size(); ++k) {
				if (columnsDone.contains(k)) {continue;}
				if (pieces[k] == yname) {
					// X component, Y compoment both are found.
					isVector = true;
					VectorData vData;
					vData.name = vdataName;
					vData.columnX = j;
					vData.columnY = k;
					vectorDatas.append(vData);
					vtkDoubleArray* da = vtkDoubleArray::New();
					da->SetName(iRIC::toStr(vdataName).c_str());
					da->SetNumberOfComponents(3);
					datas.insert(vData.name, da);
					columnsDone.insert(j);
					columnsDone.insert(k);
					m_vectorNames.append(vData.name);
				}
			}
		}
		if (! isVector) {
			ScalarData sData;
			sData.name = pieces[j];
			sData.column = j;
			scalarDatas.append(sData);
			vtkDoubleArray* da = vtkDoubleArray::New();
			da->SetName(iRIC::toStr(sData.name).c_str());
			datas.insert(sData.name, da);
			columnsDone.insert(j);
			m_pointNames.append(sData.name);
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
					vtkDoubleArray* da = datas.value(sData.name);
					double val = pieces[sData.column].toDouble(&ok);
					if (! ok) {throw pieces[sData.column];}
					da->InsertNextValue(val);
				}
				for (int i = 0; i < vectorDatas.size(); ++i) {
					VectorData& vData = vectorDatas[i];
					vtkDoubleArray* da = datas.value(vData.name);
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
		throw ErrorMessage(tr("Wrong data found: %1").arg(pointData));
	}
	m_pointData->SetPoints(points);
	m_pointData->SetVerts(cells);
	QList<vtkDoubleArray*> vals = datas.values();
	for (int i = 0; i < vals.size(); ++i) {
		vtkDoubleArray* da = vals[i];
		m_pointData->GetPointData()->AddArray(da);
		da->Delete();
	}

	m_name = QDir::toNativeSeparators(filename);
	setupPolyData();
}

void MeasuredData::exportToFile(const QString& filename)
{
	QFile file(filename);
	if (! file.open(QFile::WriteOnly | QFile::Text)) {
		throw ErrorMessage(tr("Error occured while opening the file."));
	}
	QTextStream stream(&file);
	vtkPointData* pd = m_pointData->GetPointData();
	int nArrays = pd->GetNumberOfArrays();
	vtkIdType dataCount = 0;

	// output header.
	QStringList headers;
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
		double* p = m_pointData->GetPoint(i);
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

void MeasuredData::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_name = elem.attribute("name");
	int index = elem.attribute("index").toInt();
	setIndex(index);
}

void MeasuredData::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_name);
	writer.writeAttribute("index", QString::number(m_index));
}

void MeasuredData::loadExternalData(const QString& filename)
{
	vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
	reader->SetFileName(iRIC::toStr(filename).c_str());
	reader->Update();
	m_pointData->DeepCopy(reader->GetOutput());
	m_pointNames.clear();
	m_vectorNames.clear();
	vtkPointData* pd = m_pointData->GetPointData();
	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		vtkDataArray* da = pd->GetArray(i);
		if (da->GetNumberOfComponents() == 1) {
			m_pointNames.append(da->GetName());
		} else if (da->GetNumberOfComponents() == 3) {
			m_vectorNames.append(da->GetName());
		}
	}
	setupPolyData();
}

void MeasuredData::saveExternalData(const QString& filename)
{
	vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	writer->SetFileName(iRIC::toStr(filename).c_str());
	writer->SetInputData(m_pointData);
	writer->SetFileTypeToBinary();
	writer->Write();
}

void MeasuredData::setupPolyData()
{
	triangulateio in, out;
	in.pointlist = new double[m_pointData->GetPoints()->GetNumberOfPoints() * 2];
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	in.numberofpoints = m_pointData->GetPoints()->GetNumberOfPoints();
	in.numberofpointattributes = 0;

	in.trianglelist = NULL;
	in.triangleattributelist = NULL;
	in.trianglearealist = NULL;
	in.neighborlist = NULL;
	in.numberoftriangles = 0;
	in.numberofcorners = 0;
	in.numberoftriangleattributes = 0;

	in.segmentlist = NULL;
	in.segmentmarkerlist = NULL;
	in.numberofsegments = 0;

	in.holelist = NULL;
	in.numberofholes = 0;

	in.regionlist = NULL;
	in.numberofregions = 0;

	in.edgelist = NULL;
	in.edgemarkerlist = NULL;
	in.normlist = NULL;
	in.numberofedges = 0;

	for (vtkIdType i = 0; i < m_pointData->GetPoints()->GetNumberOfPoints(); ++i) {
		double v[3];
		m_pointData->GetPoints()->GetPoint(i, v);
		*(in.pointlist + i * 2)     = v[0];
		*(in.pointlist + i * 2 + 1) = v[1];
	}

	out.pointlist = in.pointlist;
	out.pointattributelist = NULL;
	out.pointmarkerlist = NULL;
	out.trianglelist = NULL;
	out.triangleattributelist = NULL;
	out.trianglearealist = NULL;
	out.neighborlist = NULL;
	out.segmentlist = NULL;
	out.segmentmarkerlist = NULL;
	out.holelist = NULL;
	out.regionlist = NULL;
	out.edgelist = NULL;
	out.edgemarkerlist = NULL;
	out.normlist = NULL;

	QString argstr;
	argstr.append("pcj");
	char* arg = new char[argstr.length() + 1];
	strcpy(arg, iRIC::toStr(argstr).c_str());
	bool finished = false;

	TriangleExecuteThread* thread = new TriangleExecuteThread(this);

	thread->setArgs(arg);
	thread->setIOs(&in, &out);

	if (m_pointData->GetPoints()->GetNumberOfPoints() < 3) {
		out.numberoftriangles = 0;
		goto finalization;
	}
	// start execution!
	thread->start();

	// wait for 1 second
	finished = thread->wait(1000);

	if (! finished) {
		int prog = 10;
		// Not finished yet. Show wait dialog.
		WaitDialog* waitDialog = new WaitDialog(iricMainWindow());
		waitDialog->showProgressBar();
		waitDialog->setRange(0, 100);
		waitDialog->setUnknownLimitMode(300);
		waitDialog->setProgress(prog);
		waitDialog->disableCancelButton();
		waitDialog->setMessage(tr("Importing Measured Data..."));
		waitDialog->show();
		while (! finished) {
			qApp->processEvents();
			finished = thread->wait(200);
			waitDialog->setProgress(prog);
			++prog;
		}
		waitDialog->hide();
		delete waitDialog;
	}
finalization:
	delete in.pointlist;
	delete arg;

	m_polyData->Reset();
	m_polyData->Allocate(out.numberoftriangles);
	m_polyData->SetPoints(m_pointData->GetPoints());
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < out.numberoftriangles; ++i) {
		vtkIdType nodeIds[3];
		nodeIds[0] = *(out.trianglelist + i * 3) - 1;
		nodeIds[1] = *(out.trianglelist + i * 3 + 1) - 1;
		nodeIds[2] = *(out.trianglelist + i * 3 + 2) - 1;
		cells->InsertNextCell(3, nodeIds);
	}
	m_polyData->SetPolys(cells);
	m_polyData->GetPointData()->Initialize();
	for (int i = 0; i < m_pointData->GetPointData()->GetNumberOfArrays(); ++i) {
		vtkDataArray* da = m_pointData->GetPointData()->GetArray(i);
		m_polyData->GetPointData()->AddArray(da);
	}
	m_polyData->BuildCells();
	m_polyData->BuildLinks();
	m_polyData->Modified();
	m_polyData->GetPointData()->Modified();

	trifree(out.pointmarkerlist);
	trifree(out.trianglelist);
	trifree(out.segmentlist);
	trifree(out.segmentmarkerlist);

	if (out.numberoftriangles == 0) {
		m_noPolyData = true;
	}
}

void MeasuredData::doApplyOffset(double x, double y)
{
	vtkPoints* points = this->m_polyData->GetPoints();
	vtkIdType numPoints = points->GetNumberOfPoints();
	double v[3];
	for (vtkIdType id = 0; id < numPoints; ++id) {
		points->GetPoint(id, v);
		v[0] -= x;
		v[1] -= y;
		points->SetPoint(id, v);
	}
	points->Modified();
}
