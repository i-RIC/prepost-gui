#include "../../base/iricmainwindowinterface.h"
#include "measureddata.h"
#include "private/measureddata_impl.h"

#include <guibase/widget/waitdialog.h>
#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QCoreApplication>
#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>

#define REAL double
#define VOID void

#include <triangle/triangle.h>
#include <triangle/triangleexecutethread.h>

MeasuredData::Impl::Impl() :
	m_noPolyData {false}
{
	m_pointData = vtkSmartPointer<vtkPolyData>::New();
	m_polyData = vtkSmartPointer<vtkPolyData>::New();
}

MeasuredData::MeasuredData(ProjectDataItem* parent) :
	ProjectDataItem(parent),
	impl {new Impl()}
{}

MeasuredData::~MeasuredData()
{
	delete impl;
}

const QString& MeasuredData::name() const
{
	return impl->m_name;
}

void MeasuredData::setName(const QString& name)
{
	impl->m_name = name;
}

vtkPolyData* MeasuredData::pointData() const
{
	return impl->m_pointData;
}

vtkPolyData* MeasuredData::polyData() const
{
	return impl->m_polyData;
}

const std::vector<std::string>& MeasuredData::scalarNames() const
{
	return impl->m_scalarNames;
}

std::vector<std::string>& MeasuredData::scalarNames()
{
	return impl->m_scalarNames;
}

const std::vector<std::string>& MeasuredData::vectorNames() const
{
	return impl->m_vectorNames;
}

std::vector<std::string>& MeasuredData::vectorNames()
{
	return impl->m_vectorNames;
}

int MeasuredData::index() const
{
	return impl->m_index;
}

void MeasuredData::setIndex(int index)
{
	impl->m_index = index;
	setFilename(QString("MeasuredData_%1.vtk").arg(impl->m_index));
}

bool MeasuredData::noPolyData() const
{
	return impl->m_noPolyData;
}

void MeasuredData::applyOffset(double x, double y)
{
	doApplyOffset(x, y);
}

void MeasuredData::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	impl->m_name = elem.attribute("name");
	int index = elem.attribute("index").toInt();
	setIndex(index);
}

void MeasuredData::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", impl->m_name);
	writer.writeAttribute("index", QString::number(impl->m_index));
}

void MeasuredData::loadExternalData(const QString& filename)
{
	vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
	reader->SetFileName(iRIC::toStr(filename).c_str());
	reader->Update();
	impl->m_pointData->DeepCopy(reader->GetOutput());
	impl->m_scalarNames.clear();
	impl->m_vectorNames.clear();
	vtkPointData* pd = impl->m_pointData->GetPointData();
	for (int i = 0; i < pd->GetNumberOfArrays(); ++i) {
		vtkDataArray* da = pd->GetArray(i);
		if (da->GetNumberOfComponents() == 1) {
			impl->m_scalarNames.push_back(da->GetName());
		} else if (da->GetNumberOfComponents() == 3) {
			impl->m_vectorNames.push_back(da->GetName());
		}
	}
	setupPolyData();
}

void MeasuredData::saveExternalData(const QString& filename)
{
	vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	writer->SetFileName(iRIC::toStr(filename).c_str());
	writer->SetInputData(impl->m_pointData);
	writer->SetFileTypeToBinary();
	writer->Write();
}

void MeasuredData::setupPolyData()
{
	triangulateio in, out;
	in.pointlist = new double[impl->m_pointData->GetPoints()->GetNumberOfPoints() * 2];
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	in.numberofpoints = impl->m_pointData->GetPoints()->GetNumberOfPoints();
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

	for (vtkIdType i = 0; i < impl->m_pointData->GetPoints()->GetNumberOfPoints(); ++i) {
		double v[3];
		impl->m_pointData->GetPoints()->GetPoint(i, v);
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

	if (impl->m_pointData->GetPoints()->GetNumberOfPoints() < 3) {
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

	impl->m_polyData->Reset();
	impl->m_polyData->Allocate(out.numberoftriangles);
	impl->m_polyData->SetPoints(impl->m_pointData->GetPoints());
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < out.numberoftriangles; ++i) {
		vtkIdType nodeIds[3];
		nodeIds[0] = *(out.trianglelist + i * 3) - 1;
		nodeIds[1] = *(out.trianglelist + i * 3 + 1) - 1;
		nodeIds[2] = *(out.trianglelist + i * 3 + 2) - 1;
		cells->InsertNextCell(3, nodeIds);
	}
	impl->m_polyData->SetPolys(cells);
	impl->m_polyData->GetPointData()->Initialize();
	for (int i = 0; i < impl->m_pointData->GetPointData()->GetNumberOfArrays(); ++i) {
		vtkDataArray* da = impl->m_pointData->GetPointData()->GetArray(i);
		impl->m_polyData->GetPointData()->AddArray(da);
	}
	impl->m_polyData->BuildCells();
	impl->m_polyData->BuildLinks();
	impl->m_polyData->Modified();
	impl->m_polyData->GetPointData()->Modified();

	trifree(out.pointmarkerlist);
	trifree(out.trianglelist);
	trifree(out.segmentlist);
	trifree(out.segmentmarkerlist);

	if (out.numberoftriangles == 0) {
		impl->m_noPolyData = true;
	}
}

void MeasuredData::doApplyOffset(double x, double y)
{
	vtkPoints* points = impl->m_polyData->GetPoints();
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
