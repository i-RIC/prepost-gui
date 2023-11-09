#include "v4structured2dgrid_structurechecker.h"

#include <misc/mathsupport.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <QString>

bool v4Structured2dGrid::StructureChecker::check(v4Grid* grid, QStringList* messages, QTextStream* log)
{
	QSettings settings;
	settings.beginGroup("gridcheck");

	auto sgrid = dynamic_cast<v4Structured2dGrid*> (grid);

	if (settings.value("checkcrushed", true).value<bool>()) {
		// check whether it is crushed
		if (! isValid(sgrid, log)) {
			messages->push_back(v4Structured2dGrid::tr("An edge line intersects with that of another cell in this grid."));
		}
	}
	if (settings.value("checkaspectratio", false).value<bool>()) {
		double limit = settings.value("aspectratiolimit", 1.5).value<double>();
		if (! isAspectRatioOk(sgrid, limit, log)) {
			messages->push_back(v4Structured2dGrid::tr("Aspect ratio of grid cell exceed limit value %1 in more than one cell.").arg(limit));
		}
	}
	if (settings.value("checkvariation", false).value<bool>()) {
		double ilimit = settings.value("variationilimit", 1.5).value<double>();
		double jlimit = settings.value("variationjlimit", 1.5).value<double>();
		if (! isVariationOk(sgrid, ilimit, jlimit, log)) {
			messages->push_back(v4Structured2dGrid::tr("Variation of grid cell edge lengths exceed limit values (I-direction: %1, J-direction: %2) in more than one cell.").arg(ilimit).arg(jlimit));
		}
	}
	if (settings.value("checkangle", false).value<bool>()) {
		double limit = settings.value("anglelimit", 40).value<double>();
		if (! isAngleOk(sgrid, limit, log)) {
			messages->push_back(v4Structured2dGrid::tr("Angle of grid cell edge fall below limit value %1 in more than one cell.").arg(limit));
		}
	}

	return (messages->size() == 0);
}

bool v4Structured2dGrid::StructureChecker::isValid(v4Structured2dGrid* grid, QTextStream* log)
{
	bool allOk = true;
	*log << v4Structured2dGrid::tr("* Cells where edges intersect") << endl;
	for (vtkIdType j = 0; j < grid->dimensionJ() - 1; ++j) {
		for (vtkIdType i = 0; i < grid->dimensionI() - 1; ++i) {
			QPointF v0 = grid->point2d(i, j);
			QPointF v1 = grid->point2d(i + 1, j);
			QPointF v2 = grid->point2d(i + 1, j + 1);
			QPointF v3 = grid->point2d(i, j + 1);

			double tmpv1, tmpv2;
			tmpv1 = iRIC::outerProduct(v2 - v1, v0 - v1);
			tmpv2 = iRIC::outerProduct(v0 - v3, v2 - v3);

			if (tmpv1 * tmpv2 < 0.0) {
				if (allOk) {
					*log << "  I, J" << endl;
				}
				*log << "  " << i + 1 << ", " << j + 1 << endl;
				allOk = false;
			}
		}
	}
	if (allOk) {
		*log << "  " << tr("Not exist") << endl;
	}

	*log << endl;

	return allOk;
}

bool v4Structured2dGrid::StructureChecker::isAspectRatioOk(v4Structured2dGrid* grid, double limit, QTextStream* log)
{
	double lengths[4];
	QPointF v[4];
	bool allOk = true;

	*log << v4Structured2dGrid::tr("* Cells where aspect ratio of cell edges exceed %1").arg(limit) << endl;
	for (vtkIdType j = 0; j < grid->dimensionJ() - 1; ++j) {
		for (vtkIdType i = 0; i < grid->dimensionI() - 1; ++i) {
			v[0] = grid->point2d(i, j);
			v[1] = grid->point2d(i + 1, j);
			v[2] = grid->point2d(i + 1, j + 1);
			v[3] = grid->point2d(i, j + 1);

			for (int k = 0; k < 4; ++k) {
				lengths[k] = iRIC::length(v[(k + 1) % 4] - v[k]);
			}
			for (int k = 0; k < 4; ++k) {
				// check whether the length is too small.
				if (lengths[k] < 1.0E-12) {
					if (allOk) {
						*log << "  " << v4Structured2dGrid::tr("I, J, Aspect ratio") << endl;
					}
					*log << "  " << i + 1 << ", " << j + 1 << ", " << v4Structured2dGrid::tr("Edge length too small");
					allOk = false;
					break;
				}
				double ratio = lengths[(k + 1) % 4] / lengths[k];
				if (ratio > limit || ratio < 1. / limit) {
					if (ratio < 1. / limit) {ratio = 1 / ratio;}
					if (allOk) {
						*log << "  " << v4Structured2dGrid::tr("I, J, Aspect ratio") << endl;
					}
					*log << "  " << i + 1 << ", " << j + 1 << ", " << ratio << endl;
					allOk = false;
					break;
				}
			}
		}
	}

	if (allOk) {
		*log << "  " << v4Structured2dGrid::tr("Not exist") << endl;
	}

	*log << endl;
	return allOk;
}

bool v4Structured2dGrid::StructureChecker::isAngleOk(v4Structured2dGrid* grid, double limitAngle, QTextStream* log)
{
	bool allOk = true;
	*log << v4Structured2dGrid::tr("* Cells where angle of cell vertex is below %1").arg(limitAngle) << endl;

	QPointF v[4];
	for (vtkIdType j = 0; j < grid->dimensionJ() - 1; ++j) {
		for (vtkIdType i = 0; i < grid->dimensionI() - 1; ++i) {
			v[0] = grid->point2d(i, j);
			v[1] = grid->point2d(i + 1, j);
			v[2] = grid->point2d(i + 1, j + 1);
			v[3] = grid->point2d(i, j + 1);

			for (int k = 0; k < 4; ++k) {
				QPointF vec0 = v[(k + 1) % 4] - v[k];
				QPointF vec1 = v[(k + 3) % 4] - v[k];

				double dotProd = QPointF::dotProduct(vec0, vec1);
				double cosVal = dotProd / (iRIC::length(vec0) * iRIC::length(vec1));
				double angle = acos(cosVal) / M_PI * 180.;

				if (angle < limitAngle) {
					if (allOk) {
						*log << "  " << v4Structured2dGrid::tr("I, J, Angle") << endl;
					}
					*log << "  " << i + 1 << ", " << j + 1 << ", " << angle << endl;
					allOk = false;
				}
			}
		}
	}
	if (allOk) {
		*log << "  " << v4Structured2dGrid::tr("Not exist") << endl;
	}

	*log << endl;
	return allOk;
}

bool v4Structured2dGrid::StructureChecker::isVariationOk(v4Structured2dGrid* grid, double ilimit, double jlimit, QTextStream* log)
{
	bool iAllOk = true;
	*log << v4Structured2dGrid::tr("* Cells where variation of cell length along i-direction exceed %1").arg(ilimit) << endl;

	double lengths[2];
	QPointF v[3];
	for (vtkIdType j = 0; j < grid->dimensionJ(); ++j) {
		for (vtkIdType i = 0; i < grid->dimensionI() - 2; ++i) {
			v[0] = grid->point2d(i, j);
			v[1] = grid->point2d(i + 1, j);
			v[2] = grid->point2d(i + 2, j);

			for (int k = 0; k < 2; ++k) {
				lengths[k] = iRIC::length(v[k + 1] - v[k]);
			}
			double variation = lengths[1] / lengths[0];
			if (variation < 1) {variation = 1. / variation;}
			if (variation > ilimit) {
				if (iAllOk) {
					*log << "  " << v4Structured2dGrid::tr("I, J, Variation") << endl;
				}

				vtkIdType tmpj = j;
				if (tmpj == grid->dimensionJ() - 1) {tmpj = grid->dimensionJ() - 2;}
				*log << "  " << i + 2 << ", " << tmpj + 1 << ", " << variation << endl;
				iAllOk = false;
			}
		}
	}
	if (iAllOk) {
		*log << "  " << v4Structured2dGrid::tr("Not exist") << endl;
	}

	*log << endl;

	bool jAllOk = true;
	*log << v4Structured2dGrid::tr("* Cells where variation of cell length along j-direction exceed %1").arg(jlimit) << endl;
	for (vtkIdType i = 0; i < grid->dimensionI(); ++i) {
		for (vtkIdType j = 0; j < grid->dimensionJ() - 2; ++j) {
			v[0] = grid->point2d(i, j);
			v[1] = grid->point2d(i, j + 1);
			v[2] = grid->point2d(i, j + 2);

			for (int k = 0; k < 2; ++k) {
				lengths[k] = iRIC::length(v[k + 1] - v[k]);
			}
			double variation = lengths[1] / lengths[0];
			if (variation < 1) {variation = 1. / variation;}
			if (variation > jlimit) {
				if (jAllOk) {
					*log << "  " << v4Structured2dGrid::tr("I, J, Variation") << endl;
				}
				vtkIdType tmpi = i;
				if (tmpi == grid->dimensionI() - 1) {tmpi = grid->dimensionI() - 2;}
				*log << "  " << tmpi + 1 << ", " << j + 2 << ", " << variation << endl;
				jAllOk = false;
			}
		}
	}
	if (jAllOk) {
		*log << "  " << v4Structured2dGrid::tr("Not exist") << endl;
	}

	*log << endl;
	return iAllOk && jAllOk;
}
