#include "geodatapointmaprealimporter.h"
#include "geodatapointmaprealimporterfilterdialog.h"
#include "geodatapointmapt.h"

#include <misc/stringtool.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

GeoDataPointmapRealImporter::GeoDataPointmapRealImporter(GeoDataCreator* creator) :
	GeoDataImporter("tpo", tr("DEM etc. (*.tpo)"), creator)
{}

bool GeoDataPointmapRealImporter::doInit(const QString& /*filename*/, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* w)
{
	GeoDataPointmapRealImporterFilterDialog filterDialog(w);
	if (filterDialog.exec() != QDialog::Accepted) {
		return false;
	}
	m_filterValue = filterDialog.filterValue();
	return true;
}

bool GeoDataPointmapRealImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	QFileInfo finfo(filename());
	// Allocate objects to hold points and vertex cells.
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkDoubleArray> values = vtkSmartPointer<vtkDoubleArray>::New();

	double xt[3];
	auto filter = selectedFilter();
	if (filter == tr("Topography File (*.tpo)") || finfo.suffix() == "tpo")
	{
		QFile file(filename());
		if (! file.open(QIODevice::ReadOnly)) {
			QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename()));
			return false;
		}
		QTextStream in(&file);
		int index;
		double x[3];
		int counter = 0;
		int filterCounter = 0;

		QMultiMap<double, double>* map = new QMultiMap<double, double>;
		do {
			QString qstr = in.readLine();
			qstr.replace(",", " ");

			auto frags = qstr.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if (frags.size() < 3) {continue;}

			QTextStream in2(&qstr, QIODevice::ReadOnly);
			if (frags.size() == 3) {
				in2 >> x[0] >> x[1] >> x[2];
			} else if (frags.size() == 4) {
				in2 >> index >> x[0] >> x[1] >> x[2];
			}
			if (map->contains(x[0], x[1])) {continue;}

			if (in.status() == QTextStream::Ok && filterCounter == 0) {
				xt[0] = x[0]; xt[1] = x[1]; xt[2] = 0;
				points->InsertNextPoint(xt);
				values->InsertNextValue(x[2]);
				map->insert(x[0], x[1]);
				counter++;
			}
			++ filterCounter;
			if (filterCounter == m_filterValue) {
				filterCounter = 0;
			}
			if (in.status() == QTextStream::ReadCorruptData) {
				return false;
			}
		} while (! in.atEnd());
		map->clear();
		delete map;
		if (points->GetNumberOfPoints() < 3) {
			QMessageBox::critical(w, tr("Error"), tr("The data contains only %1 points. Point cloud data must contain 3 points at least.").arg(points->GetNumberOfPoints()));
			return false;
		}
		file.close();
	}
	pmap->setPoints(points, values);
	pmap->doDelaunay();
	return true;
}

const QStringList GeoDataPointmapRealImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Topography File (*.tpo)");
	return ret;
}

const QStringList GeoDataPointmapRealImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "tpo";
	return ret;
}
