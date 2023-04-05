#include "geodatapointmap_pointsmanager.h"
#include "geodatapointmap_tinmanager_breakline.h"
#include "geodatapointmap_tinmanager_tinbuilder.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <triangle/triangle.h>
#include <triangle/triangleexecutethread.h>
#include <triangle/triangleutil.h>

namespace {

void setupWaitDialog(WaitDialog* dialog, int initialProgress, bool allowCancel)
{
	dialog->showProgressBar();
	dialog->setRange(0, 100);
	dialog->setUnknownLimitMode(300);
	dialog->setProgress(initialProgress);
	if (! allowCancel) {
		dialog->disableCancelButton();
	}
	dialog->setMessage(GeoDataPointmap::tr("Remeshing TINs..."));
}

} // namespace

GeoDataPointmap::TINManager::TINBuilder::TINBuilder(TINManager* manager) :
	m_manager {manager}
{}

bool GeoDataPointmap::TINManager::TINBuilder::build(bool allowCancel)
{
	if (m_manager->checkIfBreakLinesHasIntersections()) {return false;}

	triangulateio in, out;
	int segmentCount = 0;

	auto points = m_manager->m_pointsManager->points();

	for (auto line : m_manager->breakLines()) {
		auto indices = line->vertexIndices();
		if (indices.size() < 2) {continue;}

		auto prevIndex = indices[0];
		for (int i = 0; i < indices.size(); ++i) {
			auto currIndex = indices[i];
			if (prevIndex != currIndex) {++ segmentCount;}
			prevIndex = currIndex;
		}
	}

	TriangleUtil::clearTriangulateio(&in);
	in.numberofpoints = points->GetPoints()->GetNumberOfPoints();
	in.pointlist = new double[points->GetPoints()->GetNumberOfPoints() * 2];
	if (segmentCount > 0) {
		in.numberofsegments = segmentCount;
		in.segmentlist = new int[segmentCount * 2];
	}

	for (vtkIdType i = 0; i < points->GetPoints()->GetNumberOfPoints(); ++i) {
		double v[3];
		points->GetPoints()->GetPoint(i, v);
		*(in.pointlist + i * 2)     = v[0];
		*(in.pointlist + i * 2 + 1) = v[1];
	}
	if (segmentCount > 0) {
		int segmentIndex = 0;

		for (auto line : m_manager->breakLines()) {
			auto indices = line->vertexIndices();
			if (indices.size() < 2) {continue;}

			vtkIdType prevIndex = indices[0];
			for (int i = 1; i < indices.size(); ++i) {
				vtkIdType currIndex = indices[i];
				if (prevIndex != currIndex) {
					*(in.segmentlist + segmentIndex * 2) = prevIndex + 1;
					*(in.segmentlist + segmentIndex * 2 + 1) = currIndex + 1;
					++ segmentIndex;
				}
				prevIndex = currIndex;
			}
		}
	}

	TriangleUtil::clearTriangulateio(&out);
	out.pointlist = in.pointlist;

	QString args("pcj");

	TriangleExecuteThread* thread = new TriangleExecuteThread(m_manager);

	thread->setArgs(args);
	thread->setIOs(&in, &out);

	// start execution!
	thread->start();

	// wait for 1 second
	bool finished = thread->wait(1000);
	m_canceled = false;

	if (! finished) {
		int prog = 10;

		// Not finished yet. Show wait dialog.
		WaitDialog waitDialog(m_manager->m_parent->preProcessorWindow());
		setupWaitDialog(&waitDialog, prog, allowCancel);
		connect(&waitDialog, &WaitDialog::canceled, this, &TINBuilder::cancel);

		waitDialog.show();
		while (! finished && ! m_canceled) {
			qApp->processEvents();
			finished = thread->wait(200);
			waitDialog.setProgress(prog);
			++prog;
		}
		waitDialog.hide();
		if (m_canceled) {
			// not finished, but canceled;
			thread->terminate();
			thread->wait();
		}
	}

	delete in.pointlist;
	if (segmentCount != 0) {
		delete in.segmentlist;
	}

	if (! m_canceled) {
		auto tris = vtkSmartPointer<vtkCellArray>::New();

		vtkIdType pts[3];
		for (int i = 0; i < out.numberoftriangles; ++i) {
			pts[0] = *(out.trianglelist + i * 3) - 1;
			pts[1] = *(out.trianglelist + i * 3 + 1) - 1;
			pts[2] = *(out.trianglelist + i * 3 + 2) - 1;
			tris->InsertNextCell(3, pts);
		}
		auto tin = vtkSmartPointer<vtkPolyData>::New();
		tin->SetPoints(points->GetPoints());
		tin->SetPolys(tris);

		m_manager->setTinData(tin, m_manager->m_pointsManager->values());
	}

	trifree(out.pointmarkerlist);
	trifree(out.trianglelist);
	trifree(out.segmentlist);
	trifree(out.segmentmarkerlist);

	return (! m_canceled);
}

void GeoDataPointmap::TINManager::TINBuilder::cancel()
{
	m_canceled = true;
}
