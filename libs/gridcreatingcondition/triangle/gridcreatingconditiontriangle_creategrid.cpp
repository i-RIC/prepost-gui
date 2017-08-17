#include "gridcreatingconditiontriangle.h"
#include "gridcreatingconditiontriangledivisionline.h"
#include "gridcreatingconditiontrianglegridregionpolygon.h"
#include "gridcreatingconditiontriangleholepolygon.h"
#include "gridcreatingconditiontriangleremeshpolygon.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/project/projectdata.h>
#include <misc/iricrootpath.h>
#include <misc/stringtool.h>
#include <triangle/triangleexecutethread.h>

#include <triangle/triangle.h>

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <vtkTriangle.h>

namespace {

void setupTriangleInput(GridCreatingConditionTriangleGridRegionPolygon* regionPolygon,
												const QList<GridCreatingConditionTriangleRemeshPolygon*>& remeshPolygons,
												const QList<GridCreatingConditionTriangleHolePolygon*>& holePolygons,
												const QList<GridCreatingConditionTriangleDivisionLine*>& divisionLines,
												bool areaConstraint, double area,
												const QPointF& offset,
												triangulateio* in)
{
	QPolygonF pol = regionPolygon->polygon();
	QVector<QPointF> line;
	int pointCount = 0;
	int segmentCount = 0;
	int regionCount = 1;
	pointCount += (pol.count() - 1);
	segmentCount += (pol.count() - 1);
	for (int i = 0; i < remeshPolygons.size(); ++i) {
		pol = remeshPolygons[i]->polygon();
		pointCount += (pol.count() - 1);
		segmentCount += (pol.count() - 1);
		++ regionCount;
	}
	for (int i = 0; i < holePolygons.size(); ++i) {
		pol = holePolygons[i]->polygon();
		pointCount += (pol.count() - 1);
		segmentCount += (pol.count() - 1);
	}
	for (int i = 0; i < divisionLines.size(); ++i) {
		line = divisionLines[i]->polyLine();
		pointCount += line.count();
		segmentCount += (line.count() - 1);
	}

	in->pointlist = new double[pointCount * 2];
	in->pointattributelist = NULL;
	in->pointmarkerlist = NULL;
	in->numberofpoints = pointCount;
	in->numberofpointattributes = 0;

	in->trianglelist = NULL;
	in->triangleattributelist = NULL;
	in->trianglearealist = NULL;
	in->neighborlist = NULL;
	in->numberoftriangles = 0;
	in->numberofcorners = 0;
	in->numberoftriangleattributes = 0;

	in->segmentlist = new int[segmentCount * 2];
	in->segmentmarkerlist = NULL;
	in->numberofsegments = segmentCount;

	in->holelist = new double[holePolygons.count() * 2];
	in->numberofholes = holePolygons.count();

	in->regionlist = new double[regionCount * 4];
	in->numberofregions = regionCount;

	in->edgelist = NULL;
	in->edgemarkerlist = NULL;
	in->normlist = NULL;
	in->numberofedges = 0;

	pol = regionPolygon->polygon(offset);
	for (int i = 0; i < pol.count() - 1; ++i) {
		*(in->pointlist + i * 2)     = pol.at(i).x();
		*(in->pointlist + i * 2 + 1) = pol.at(i).y();
		*(in->segmentlist + i * 2) = i + 1;
		*(in->segmentlist + i * 2 + 1) = (i + 1) % (pol.count() - 1) + 1;
	}
	QPointF innerP = regionPolygon->innerPoint(offset);
	*(in->regionlist)     = innerP.x();
	*(in->regionlist + 1) = innerP.y();
	*(in->regionlist + 2) = 0;
	if (areaConstraint) {
		*(in->regionlist + 3) = area;
	} else {
		// no area constraint.
		QRectF rect = pol.boundingRect();
		*(in->regionlist + 3) = rect.width() * rect.height();
	}
	int pOffset = 2 * (pol.count() - 1);
	int sOffset = (pol.count() - 1);
	int aOffset = 1;
	for (int i = 0; i < remeshPolygons.size(); ++i) {
		GridCreatingConditionTriangleRemeshPolygon* rpol = remeshPolygons[i];
		pol = rpol->polygon(offset);
		for (int j = 0; j < pol.count() - 1; ++j) {
			*(in->pointlist + j * 2 + pOffset) = pol.at(j).x();
			*(in->pointlist + j * 2 + 1 + pOffset) = pol.at(j).y();
			*(in->segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in->segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (pol.count() - 1) + sOffset + 1;
		}
		innerP = rpol->innerPoint(offset);
		*(in->regionlist + 4 * aOffset) = innerP.x();
		*(in->regionlist + 4 * aOffset + 1) = innerP.y();
		*(in->regionlist + 4 * aOffset + 2) = 0;
		*(in->regionlist + 4 * aOffset + 3) = rpol->cellSize();
		pOffset += 2 * (pol.count() - 1);
		sOffset += (pol.count() - 1);
		++aOffset;
	}

	for (int i = 0; i < holePolygons.size(); ++i) {
		GridCreatingConditionTriangleHolePolygon* hpol = holePolygons[i];
		pol = hpol->polygon(offset);
		for (int j = 0; j < pol.count() - 1; ++j) {
			*(in->pointlist + j * 2 + pOffset) = pol.at(j).x();
			*(in->pointlist + j * 2 + 1 + pOffset) = pol.at(j).y();
			*(in->segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in->segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (pol.count() - 1) + sOffset + 1;
		}
		innerP = hpol->innerPoint(offset);
		*(in->holelist + i * 2) = innerP.x();
		*(in->holelist + i * 2 + 1) = innerP.y();
		pOffset += 2 * (pol.count() - 1);
		sOffset += (pol.count() - 1);
	}

	for (int i = 0; i < divisionLines.size(); ++i) {
		GridCreatingConditionTriangleDivisionLine* line = divisionLines[i];
		QVector<QPointF> l = line->polyLine(offset);
		for (int j = 0; j < l.count(); ++j) {
			*(in->pointlist + j * 2 + pOffset)     = l.at(j).x();
			*(in->pointlist + j * 2 + 1 + pOffset) = l.at(j).y();
			if (j != 0) {
				*(in->segmentlist + (j - 1) * 2 + sOffset * 2) = j + pOffset / 2;
				*(in->segmentlist + (j - 1) * 2 + 1 + sOffset * 2) = j + 1 + pOffset / 2;
			}
		}
		pOffset += 2 * l.count();
		sOffset += (l.count() - 1);
	}
}

TriangleExecuteThread* setupTriangleThread(bool angleConstraint, double angle,
																					 bool areaConstraint, bool hasRemeshPolygon,
																					 PreProcessorGridAndGridCreatingConditionDataItemInterface* dataItemInterface,
																					 ProjectData* projectData)
{
	QString args("p");

	if (angleConstraint) {
		args.append(QString("q%1").arg(angle));
	}

	if (areaConstraint || hasRemeshPolygon) {
		args.append("a");
	}

	args.append("D");

	TriangleExecuteThread* thread = new TriangleExecuteThread(nullptr);

	thread->setArgs(args);

	std::string zoneName = dataItemInterface->zoneName();

	QString triExe = iRICRootPath::get().append("/triangle.exe");
	QString workFolder = projectData->workDirectory();
	QString polyFileName = QString("%1.poly").arg(zoneName.c_str());
	QString fileArgs = "-ve";
	fileArgs.append(args);

	thread->setFileOutputSetting(triExe, fileArgs, polyFileName, workFolder);

	return thread;
}

void clearOutputIO(triangulateio* out)
{
	out->pointlist = NULL;
	out->pointattributelist = NULL;
	out->pointmarkerlist = NULL;
	out->trianglelist = NULL;
	out->triangleattributelist = NULL;
	out->trianglearealist = NULL;
	out->neighborlist = NULL;
	out->segmentlist = NULL;
	out->segmentmarkerlist = NULL;
	out->holelist = NULL;
	out->regionlist = NULL;
	out->edgelist = NULL;
	out->edgemarkerlist = NULL;
	out->normlist = NULL;
}

WaitDialog* setupWaitDialog(QWidget* parent, int progress)
{
	WaitDialog* dialog = new WaitDialog(parent);
	dialog->showProgressBar();
	dialog->setRange(0, 100);
	dialog->setUnknownLimitMode(300);
	dialog->setProgress(progress);
	dialog->setMessage(GridCreatingConditionTriangle::tr("Generating grid..."));
	return dialog;
}

void outputTriangleInputToFile(triangulateio in, const QString& filename)
{
	QFile f(filename);
	f.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
	QTextStream o(&f);
	o.setRealNumberPrecision(20);
	o.setRealNumberNotation(QTextStream::ScientificNotation);
	o << in.numberofpoints << " 2 0 0\n";
	for (int i = 0; i < in.numberofpoints; ++i) {
		o << i + 1 << " " << *(in.pointlist + i * 2) << " " << *(in.pointlist + i * 2 + 1) << "\n";
	}
	o << in.numberofsegments << " 0\n";
	for (int i = 0; i < in.numberofsegments; ++i) {
		o << i + 1 << " " << *(in.segmentlist + i * 2) << " " << *(in.segmentlist + i * 2 + 1) << "\n";
	}
	o << in.numberofholes << "\n";
	for (int i = 0; i < in.numberofholes; ++i) {
		o << i + 1 << " " << *(in.holelist + i * 2) << " " << *(in.holelist + i * 2 + 1) << "\n";
	}
	o << in.numberofregions << "\n";
	for (int i = 0; i < in.numberofregions; ++i) {
		o << i + 1 << " " << *(in.regionlist + i * 4) << " " << *(in.regionlist + i * 4 + 1) << " " << *(in.regionlist + i * 4 + 2) << " " << *(in.regionlist + i * 4 + 3) << "\n";
	}
	f.close();
}

void freeInputIO(triangulateio in)
{
	delete in.pointlist;
	delete in.segmentlist;
	delete in.regionlist;
	delete in.holelist;
}

void freeOutputIO(triangulateio out)
{
	if (out.pointlist != NULL) {
		trifree(out.pointlist);
	}
	if (out.trianglelist != NULL) {
		trifree(out.trianglelist);
	}
}

void setupGridFromTriangleOutput(triangulateio out, const QPointF offset, Unstructured2DGrid* grid)
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(out.numberofpoints);
	points->SetDataTypeToDouble();
	for (int i = 0; i < out.numberofpoints; ++i) {
		double v[3];
		v[0] = *(out.pointlist + i * 2) + offset.x();
		v[1] = *(out.pointlist + i * 2 + 1) + offset.y();
		v[2] = 0;
		points->InsertNextPoint(v);
	}
	grid->vtkGrid()->SetPoints(points);

	grid->vtkGrid()->Allocate(out.numberoftriangles);
	for (int i = 0; i < out.numberoftriangles; ++i) {
		vtkIdType id1, id2, id3;
		vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
		id1 = *(out.trianglelist + i * 3) - 1;
		id2 = *(out.trianglelist + i * 3 + 1) - 1;
		id3 = *(out.trianglelist + i * 3 + 2) - 1;
		tri->GetPointIds()->SetId(0, id1);
		tri->GetPointIds()->SetId(1, id2);
		tri->GetPointIds()->SetId(2, id3);
		grid->vtkGrid()->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	}

	// allocate memory for all grid related conditions.
	QList<GridAttributeContainer*>& clist = grid->gridAttributes();
	for (GridAttributeContainer* container: clist) {
		container->allocate();
	}
	grid->setModified();
	grid->vtkGrid()->BuildLinks();
}

} // namespace

Grid* GridCreatingConditionTriangle::createGrid()
{
	Unstructured2DGrid* grid = new Unstructured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);

	triangulateio in, out;

	QPointF offset = m_gridRegionPolygon->polygon().boundingRect().topLeft();
	setupTriangleInput(m_gridRegionPolygon, m_remeshPolygons, m_holePolygons, m_divisionLines, m_areaConstraint, m_area, offset, &in);

	clearOutputIO(&out);

	auto di = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(m_conditionDataItem->parent());
	QString polyFileName = QString("%1.poly").arg(di->zoneName().c_str());

	TriangleExecuteThread* thread = setupTriangleThread(
				m_angleConstraint, m_angle, m_areaConstraint, m_remeshPolygons.size() > 0,
				di, projectData());
	thread->setIOs(&in, &out);

	outputTriangleInputToFile(in, QString("%1/%2").arg(projectData()->workDirectory()).arg(polyFileName));

	// start execution!
	thread->start();
	// wait for finish
	bool finished = thread->wait(2000);
	m_canceled = false;

	if (! finished) {
		int prog = 10;
		// Not finished yet. Show wait dialog.
		WaitDialog* waitDialog = setupWaitDialog(preProcessorWindow(), prog);
		connect(waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		waitDialog->show();
		while (! finished && ! m_canceled) {
			qApp->processEvents();
			finished = thread->wait(200);
			waitDialog->setProgress(prog);
			++prog;
		}
		waitDialog->hide();
		delete waitDialog;
		if (m_canceled) {
			// not finished, but canceled;
			thread->terminate();
			thread->wait();
		}
	}
	delete thread;

	// free memory
	freeInputIO(in);

	if (m_canceled) {
		freeOutputIO(out);
		return nullptr;
	}

	setupGridFromTriangleOutput(out, offset, grid);
	freeOutputIO(out);

	return grid;
}

