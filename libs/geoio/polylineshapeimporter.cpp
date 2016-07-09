#include "polylineshapeimporter.h"
#include "private/dbfitemselectdialog.h"

#include <misc/stringtool.h>

#include <QMessageBox>
#include <shapefil.h>

#include <cmath>
#include <string>

namespace {
	const double LOCATION_DELTA = 1.0E-6;

	class ShapeCloser
	{
	public:
		ShapeCloser(SHPHandle shph) :
			m_shapeHandle {shph}
		{}

		~ShapeCloser()
		{
			SHPClose(m_shapeHandle);
		}

	private:
		SHPHandle m_shapeHandle;
	};

	QPolygonF importPolyline(const SHPHandle& shph, int index, QWidget *w)
	{
		QVector<QPointF> ret, errorRet;
		SHPObject* shpo = SHPReadObject(shph, index);
		if (shpo->nParts > 1) {
			QMessageBox::critical(w, PolylineShapeImporter ::tr("Error"), PolylineShapeImporter::tr("This polyline is composed of multiple line strnigs.. This polyline can not be imported."));
			return errorRet;
		}
		int start = *(shpo->panPartStart);
		int end = shpo->nVertices;
		QPointF lastPoint;
		for (int i = start; i < end; ++i) {
			QPointF point(*(shpo->padfX + i), *(shpo->padfY + i));
			QPointF diff = lastPoint - point;
			bool veryNear = (std::fabs(diff.x()) < LOCATION_DELTA && std::fabs(diff.y()) < LOCATION_DELTA);
			if (i != start && veryNear) {continue;}
			ret.push_back(point);
			lastPoint = point;
		}
		return ret;
	}
}

QVector<QPointF> PolylineShapeImporter::importData(const QString& filename, const QString& /*selectedFilter*/, QWidget* w)
{
	QVector<QPointF> ret, errorRet;

	auto fname = iRIC::toStr(filename);
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");
	ShapeCloser closer(shph);

	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];

	SHPGetInfo(shph, &numEntities, &shapeType, minBound, maxBound);
	if (shapeType != SHPT_ARC) {
		QMessageBox::critical(w, tr("Error"), tr("The shape type contained in this file is not polyline."));
		return errorRet;
	}
	if (numEntities == 1) {
		// it contains only one polyline. import it!
		return importPolyline(shph, 0, w);
	}
	// It has several polygons. Ask user to select which polygon to import.
	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp("shp$"), "dbf");

	DBFItemSelectDialog dialog(dbfFilename, w);
	int dialogRet = dialog.exec();
	if (dialogRet == QDialog::Rejected) {return errorRet;}

	int item = dialog.selectedItem();
	return importPolyline(shph, item, w);
}

QStringList PolylineShapeImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Shape file (*.shp)");

	return ret;
}

QStringList PolylineShapeImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "shp";

	return ret;
}
