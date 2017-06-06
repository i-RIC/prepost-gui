#include "sacguiimporter.h"
#include "../data/arbitraryhwm/arbitraryhwm.h"
#include "../data/crosssection/crosssection.h"
#include "../data/elevationpoints/elevationpoints.h"
#include "../data/leftbankhwm/leftbankhwm.h"
#include "../data/project/project.h"
#include "../data/rightbankhwm/rightbankhwm.h"
#include "../data/watersurfaceelevationpoints/watersurfaceelevationpoints.h"
#include "../geom/geometrypoint.h"

#include <QFile>
#include <QFileDialog>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector2D>

#include <map>
#include <vector>

namespace {

void registerToMap(const QString& name, GeometryPoint* p, std::map<QString, std::vector<GeometryPoint*> >* points)
{
	std::vector<GeometryPoint*> emptyVec;
	auto it = points->find(name);
	if (it == points->end()) {
		points->insert(std::make_pair(name, emptyVec));
		it = points->find(name);
	}
	it->second.push_back(p);
}

} // namespace

bool SACGUIImporter::importData(Project* project, std::vector<CrossSection*>* newCrossSections, QWidget* w)
{
	QString fname = QFileDialog::getOpenFileName(w, tr("Select SAC GUI input file"), "", tr("SAC GUI input file (*.txt *.csv)"));
	if (fname.isNull()) {return false;}

	QPointF offset = project->offset();

	std::vector<GeometryPoint*> arbVec;
	std::vector<GeometryPoint*> leftVec;
	std::vector<GeometryPoint*> rightVec;
	std::vector<GeometryPoint*> elevVec;
	std::vector<GeometryPoint*> bmVec;
	std::vector<GeometryPoint*> rmVec;
	std::vector<GeometryPoint*> hubVec;
	std::vector<GeometryPoint*> ghVec;

	QFile file(fname);
	if (! file.open(QIODevice::ReadOnly)) {return false;}

	QTextStream stream(&file);
	QRegExp lbExp("LH.*");
	QRegExp rbExp("RH.*");
	QRegExp elevExp("(X[0-9]+).*");
	QRegExp bmExp("BM.*");
	QRegExp rmExp("RM.*");
	QRegExp hubExp("HUB.*");
	QRegExp ghExp("GH.*");
	bool first = true;

	std::map<QString, std::vector<GeometryPoint*> > xsecPoints;

	do {
		QString line = stream.readLine();
		line.replace(",", "\t");
		QStringList frags = line.split("\t");
		if (frags.size() < 5) {continue;}
		double x, y, z;
		x = frags.at(1).toDouble();
		y = frags.at(2).toDouble();
		z = frags.at(3).toDouble();
		QString name = frags.at(4);

		if (first && offset.x() == 0 && offset.y() == 0) {
			offset.setX(x);
			offset.setY(y);
		}

		GeometryPoint* point = new GeometryPoint(x - offset.x(), y - offset.y(), z, name);
		if (lbExp.exactMatch(name)) {
			leftVec.push_back(point);
		} else if (rbExp.exactMatch(name)) {
			rightVec.push_back(point);
		} else if (bmExp.exactMatch(name)) {
			bmVec.push_back(point);
		} else if (rmExp.exactMatch(name)) {
			rmVec.push_back(point);
		} else if (hubExp.exactMatch(name)) {
			hubVec.push_back(point);
		} else if (ghExp.exactMatch(name)) {
			ghVec.push_back(point);
		} else if (elevExp.exactMatch(name)) {
			elevVec.push_back(point);
			registerToMap(elevExp.cap(1), point, &xsecPoints);
		} else {
			arbVec.push_back(point);
		}

		first = false;
	} while (! stream.atEnd());

	project->elevationPoints().setPoints(elevVec);
	auto& wsePoints = project->waterSurfaceElevationPoints();
	wsePoints.leftBankHWM().setPoints(leftVec);
	wsePoints.rightBankHWM().setPoints(rightVec);
	wsePoints.arbitraryHWM().setPoints(arbVec);
	wsePoints.benchmark().setPoints(bmVec);
	wsePoints.referenceMark().setPoints(rmVec);
	wsePoints.hub().setPoints(hubVec);
	wsePoints.streamGage().setPoints(ghVec);
	project->setOffset(offset);

	int id = 0;
	for (auto& pair : xsecPoints) {
		if (pair.second.size() < 2) {continue;}

		std::multimap<double, std::pair<GeometryPoint*, GeometryPoint*> > distances;
		const auto& pvec = pair.second;
		for (int i = 0; i < pvec.size(); ++i) {
			GeometryPoint* p1 = pvec.at(i);
			for (int j = i + 1; j < pvec.size(); ++j) {
				GeometryPoint* p2 = pvec.at(j);
				double d = QVector2D(p2->x() - p1->x(), p2->y() - p1->y()).lengthSquared();
				distances.insert(std::make_pair(d, std::make_pair(p1, p2)));
			}
		}
		// get largest
		auto max_it = distances.rbegin();

		auto cs = new CrossSection(nullptr);
		cs->setPoint1(QPointF(max_it->second.first->x(), max_it->second.first->y()));
		cs->setPoint2(QPointF(max_it->second.second->x(), max_it->second.second->y()));
		cs->setName(pair.first);
		newCrossSections->push_back(cs);
		++ id;
	}

	return true;
}
