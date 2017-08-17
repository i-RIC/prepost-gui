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
} // namespace

Grid* GridCreatingConditionTriangle::createGrid()
{
	Unstructured2DGrid* grid = new Unstructured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);
	QPolygonF pol = m_gridRegionPolygon->polygon();
	QVector<QPointF> line;
	int pointCount = 0;
	int segmentCount = 0;
	int regionCount = 1;
	pointCount += (pol.count() - 1);
	segmentCount += (pol.count() - 1);
	QPointF offset = pol.boundingRect().topLeft();
	for (int i = 0; i < m_remeshPolygons.count(); ++i) {
		pol = m_remeshPolygons[i]->polygon();
		pointCount += (pol.count() - 1);
		segmentCount += (pol.count() - 1);
		++regionCount;
	}
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		pol = m_holePolygons[i]->polygon();
		pointCount += (pol.count() - 1);
		segmentCount += (pol.count() - 1);
	}
	for (int i = 0; i < m_divisionLines.count(); ++i) {
		line = m_divisionLines[i]->polyLine();
		pointCount += line.count();
		segmentCount += (line.count() - 1);
	}
	triangulateio in, out;

	in.pointlist = new double[pointCount * 2];
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	in.numberofpoints = pointCount;
	in.numberofpointattributes = 0;

	in.trianglelist = NULL;
	in.triangleattributelist = NULL;
	in.trianglearealist = NULL;
	in.neighborlist = NULL;
	in.numberoftriangles = 0;
	in.numberofcorners = 0;
	in.numberoftriangleattributes = 0;

	in.segmentlist = new int[segmentCount * 2];
	in.segmentmarkerlist = NULL;
	in.numberofsegments = segmentCount;

	in.holelist = new double[m_holePolygons.count() * 2];
	in.numberofholes = m_holePolygons.count();

	in.regionlist = new double[regionCount * 4];
	in.numberofregions = regionCount;

	in.edgelist = NULL;
	in.edgemarkerlist = NULL;
	in.normlist = NULL;
	in.numberofedges = 0;

	pol = m_gridRegionPolygon->polygon(offset);
	for (int i = 0; i < pol.count() - 1; ++i) {
		*(in.pointlist + i * 2)     = pol.at(i).x();
		*(in.pointlist + i * 2 + 1) = pol.at(i).y();
		*(in.segmentlist + i * 2) = i + 1;
		*(in.segmentlist + i * 2 + 1) = (i + 1) % (pol.count() - 1) + 1;
	}
	QPointF innerP = m_gridRegionPolygon->innerPoint(offset);
	*(in.regionlist)     = innerP.x();
	*(in.regionlist + 1) = innerP.y();
	*(in.regionlist + 2) = 0;
	if (m_areaConstraint) {
		*(in.regionlist + 3) = m_area;
	} else {
		// no area constraint.
		QRectF rect = pol.boundingRect();
		*(in.regionlist + 3) = rect.width() * rect.height();
	}
	int pOffset = 2 * (pol.count() - 1);
	int sOffset = (pol.count() - 1);
	int aOffset = 1;
	for (int i = 0; i < m_remeshPolygons.count(); ++i) {
		GridCreatingConditionTriangleRemeshPolygon* rpol = m_remeshPolygons[i];
		pol = rpol->polygon(offset);
		for (int j = 0; j < pol.count() - 1; ++j) {
			*(in.pointlist + j * 2 + pOffset)     = pol.at(j).x();
			*(in.pointlist + j * 2 + 1 + pOffset) = pol.at(j).y();
			*(in.segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in.segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (pol.count() - 1) + sOffset + 1;
		}
		innerP = rpol->innerPoint(offset);
		*(in.regionlist + 4 * aOffset) = innerP.x();
		*(in.regionlist + 4 * aOffset + 1) = innerP.y();
		*(in.regionlist + 4 * aOffset + 2) = 0;
		*(in.regionlist + 4 * aOffset + 3) = rpol->cellSize();
		pOffset += 2 * (pol.count() - 1);
		sOffset += (pol.count() - 1);
		++aOffset;
	}
	for (int i = 0; i < m_holePolygons.count(); ++i) {
		GridCreatingConditionTriangleHolePolygon* hpol = m_holePolygons[i];
		pol = hpol->polygon(offset);
		for (int j = 0; j < pol.count() - 1; ++j) {
			*(in.pointlist + j * 2 + pOffset)     = pol.at(j).x();
			*(in.pointlist + j * 2 + 1 + pOffset) = pol.at(j).y();
			*(in.segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in.segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (pol.count() - 1) + sOffset + 1;
		}
		innerP = hpol->innerPoint(offset);
		*(in.holelist + i * 2) = innerP.x();
		*(in.holelist + i * 2 + 1) = innerP.y();
		pOffset += 2 * (pol.count() - 1);
		sOffset += (pol.count() - 1);
	}
	for (int i = 0; i < m_divisionLines.count(); ++i) {
		GridCreatingConditionTriangleDivisionLine* line = m_divisionLines[i];
		QVector<QPointF> l = line->polyLine(offset);
		for (int j = 0; j < l.count(); ++j) {
			*(in.pointlist + j * 2 + pOffset)     = l.at(j).x();
			*(in.pointlist + j * 2 + 1 + pOffset) = l.at(j).y();
			if (j != 0) {
				*(in.segmentlist + (j - 1) * 2 + sOffset * 2) = j + pOffset / 2;
				*(in.segmentlist + (j - 1) * 2 + 1 + sOffset * 2) = j + 1 + pOffset / 2;
			}
		}
		pOffset += 2 * l.count();
		sOffset += (l.count() - 1);
	}

	clearOutputIO(&out);

	QString argstr;
	argstr.append("p");
	if (m_angleConstraint) {
		argstr.append(QString("q%1").arg(m_angle));
	}
	if (m_areaConstraint || m_remeshPolygons.count() > 0) {
		argstr.append("a");
	}
	argstr.append("D");
	char* arg = new char[argstr.length() + 1];
	strcpy(arg, iRIC::toStr(argstr).c_str());

	TriangleExecuteThread* thread = new TriangleExecuteThread(this);

	thread->setArgs(arg);
	thread->setIOs(&in, &out);

	PreProcessorGridAndGridCreatingConditionDataItemInterface* di = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(m_conditionDataItem->parent());
	std::string zoneName = di->zoneName();

	QString triExe = iRICRootPath::get().append("/triangle.exe");
	QString workFolder = projectData()->workDirectory();
	QString polyFileName = QString("%1.poly").arg(zoneName.c_str());
	QString fileArgs = "-ve";
	fileArgs.append(argstr);

	thread->setFileOutputSetting(triExe, fileArgs, polyFileName, workFolder);

	// file output.
	QFile f(QString("%1/%2").arg(workFolder).arg(polyFileName));
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

	// start execution!
	thread->start();
	// wait for finish
	bool finished = thread->wait(2000);
	m_canceled = false;

	if (! finished) {
		int prog = 10;
		// Not finished yet. Show wait dialog.
		WaitDialog* waitDialog = new WaitDialog(preProcessorWindow());
		waitDialog->showProgressBar();
		waitDialog->setRange(0, 100);
		waitDialog->setUnknownLimitMode(300);
		waitDialog->setProgress(prog);
		waitDialog->setMessage(tr("Generating grid..."));
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
	delete arg;
	delete in.pointlist;
	delete in.segmentlist;
	delete in.regionlist;
	delete in.holelist;

	if (m_canceled) {
		if (out.pointlist != NULL) {
			trifree(out.pointlist);
		}
		if (out.trianglelist != NULL) {
			trifree(out.trianglelist);
		}
		return nullptr;
	}
	// copy the result to VTK containers.
	vtkPoints* points = vtkPoints::New();
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
	for (auto it = clist.begin(); it != clist.end(); ++it) {
		(*it)->allocate();
	}
	grid->setModified();
	grid->vtkGrid()->BuildLinks();
	if (out.pointlist != NULL) {
		trifree(out.pointlist);
	}
	if (out.trianglelist != NULL) {
		trifree(out.trianglelist);
	}

	return grid;
}

