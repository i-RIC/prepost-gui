#ifndef V4STRUCTURED2DGRID_STRUCTURECHECKER_H
#define V4STRUCTURED2DGRID_STRUCTURECHECKER_H

#include "../v4gridstructurecheckeri.h"
#include "../v4structured2dgrid.h"

class v4Structured2dGrid::StructureChecker : public v4GridStructureCheckerI
{
public:
	bool check(v4Grid* grid, QStringList* messages, QTextStream* log) override;

private:
	static bool isValid(v4Structured2dGrid* grid, QTextStream* log);
	static bool isAspectRatioOk(v4Structured2dGrid* grid, double limit, QTextStream* log);
	static bool isAngleOk(v4Structured2dGrid* grid, double limitAngle, QTextStream* log);
	static bool isVariationOk(v4Structured2dGrid* grid, double ilimit, double jlimit, QTextStream* log);
};

#endif // V4STRUCTURED2DGRID_STRUCTURECHECKER_H
