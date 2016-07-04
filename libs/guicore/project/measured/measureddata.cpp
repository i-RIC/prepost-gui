#include "../../base/iricmainwindowinterface.h"
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

MeasuredData::MeasuredData(ProjectDataItem* parent) :
	ProjectDataItem(parent)
{
	m_pointData = vtkSmartPointer<vtkPolyData>::New();
	m_polyData = vtkSmartPointer<vtkPolyData>::New();
	m_noPolyData = false;
}

const QString& MeasuredData::name() const
{
	return m_name;
}

void MeasuredData::setName(const QString& name)
{
	m_name = name;
}

vtkPolyData* MeasuredData::pointData() const
{
	return m_pointData;
}

vtkPolyData* MeasuredData::polyData() const
{
	return m_polyData;
}

const std::vector<std::string>& MeasuredData::scalarNames() const
{
	return m_scalarNames;
}

std::vector<std::string>& MeasuredData::scalarNames()
{
	return m_scalarNames;
}

const std::vector<std::string>& MeasuredData::vectorNames() const
{
	return m_vectorNames;
}

std::vector<std::string>& MeasuredData::vectorNames()
{
	return m_vectorNames;
}

int MeasuredData::index() const
{
	return m_index;
}

void MeasuredData::setIndex(int index)
{
	m_index = index;
	setFilename(QString("MeasuredData_%1.vtk").arg(m_index));
}

bool MeasuredData::noPolyData() const
{
	return m_noPolyData;
}

void MeasuredData::applyOffset(double x, double y)
{
	doApplyOffset(x, y);
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
	m_scalarNames.clear();
	m_vectorNames.clear();
	vtkPointData* pd = m_pointData->GetPointData();
	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		vtkDataArray* da = pd->GetArray(i);
		if (da->GetNumberOfComponents() == 1) {
			m_scalarNames.push_back(da->GetName());
		} else if (da->GetNumberOfComponents() == 3) {
			m_vectorNames.push_back(da->GetName());
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
	argstr.append("pcjQ");
	std::vector<char> arg (argstr.length() + 1);
	strcpy(arg.data(), iRIC::toStr(argstr).c_str());
	bool finished = false;

	TriangleExecuteThread* thread = new TriangleExecuteThread(this);

	thread->setArgs(arg.data());
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
