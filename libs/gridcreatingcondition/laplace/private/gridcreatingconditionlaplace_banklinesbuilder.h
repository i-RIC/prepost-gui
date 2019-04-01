#ifndef GRIDCREATINGCONDITIONLAPLACE_BANKLINESBUILDER_H
#define GRIDCREATINGCONDITIONLAPLACE_BANKLINESBUILDER_H

#include "../gridcreatingconditionlaplace.h"

class GridCreatingConditionLaplace::BankLinesBuilder
{
public:
	static void buildBankLines(const std::vector<QPointF>& center, double leftDist, double rightDist, std::vector<QPointF>* leftBank, std::vector<QPointF>* rightBank);

private:
	BankLinesBuilder();
};

#endif // GRIDCREATINGCONDITIONLAPLACE_BANKLINESBUILDER_H
