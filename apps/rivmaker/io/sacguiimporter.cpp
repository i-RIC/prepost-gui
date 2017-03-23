#include "sacguiimporter.h"
#include "../data/arbitraryhwm/arbitraryhwm.h"
#include "../data/elevationpoints/elevationpoints.h"
#include "../data/leftbankhwm/leftbankhwm.h"
#include "../data/project/project.h"
#include "../data/rightbankhwm/rightbankhwm.h"
#include "../data/watersurfaceelevationpoints/watersurfaceelevationpoints.h"

#include <QFile>
#include <QFileDialog>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector3D>

bool SACGUIImporter::importData(Project* project, QWidget* w)
{
	QString fname = QFileDialog::getOpenFileName(w, tr("Select SAC GUI input file"), "", tr("SAC GUI input file (*.txt)"));
	if (fname.isNull()) {return false;}

	QPointF offset = project->offset();

	std::vector<QVector3D*> arbVec;
	std::vector<QVector3D*> leftVec;
	std::vector<QVector3D*> rightVec;
	std::vector<QVector3D*> elevVec;

	QFile file(fname);
	if (! file.open(QIODevice::ReadOnly)) {return false;}

	QTextStream stream(&file);
	QRegExp lbExp("LH\\-.+");
	QRegExp rbExp("RH\\-.+");
	QRegExp elevExp("X[0-9]+\\-.+");
	bool first = true;

	do {
		QString line = stream.readLine();
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

		if (lbExp.exactMatch(name)) {
			leftVec.push_back(new QVector3D(x - offset.x(), y - offset.y(), z));
		} else if (rbExp.exactMatch(name)) {
			rightVec.push_back(new QVector3D(x - offset.x(), y - offset.y(), z));
		} else if (elevExp.exactMatch(name)) {
			elevVec.push_back(new QVector3D(x - offset.x(), y - offset.y(), z));
		} else {
			arbVec.push_back(new QVector3D(x - offset.x(), y - offset.y(), z));
		}

		first = false;
	} while (! stream.atEnd());

	project->elevationPoints().setPoints(elevVec);
	project->waterSurfaceElevationPoints().leftBankHWM().setPoints(leftVec);
	project->waterSurfaceElevationPoints().rightBankHWM().setPoints(rightVec);
	project->waterSurfaceElevationPoints().arbitraryHWM().setPoints(arbVec);
	project->setOffset(offset);

	return true;
}
