#include "gridcreatingconditionlaplace_banklinesbuilder.h"

#include <misc/mathsupport.h>

#include <QVector2D>

void GridCreatingConditionLaplace::BankLinesBuilder::buildBankLines(const std::vector<QPointF>& centerLine, double leftDist, double rightDist, std::vector<QPointF>* leftBank, std::vector<QPointF>* rightBank)
{
	for (int i = 0; i < centerLine.size(); ++i) {
		QPointF v;
		if (i == 0) {
			QPointF p1 = centerLine.at(0);
			QPointF p2 = centerLine.at(1);
			v = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v);
		} else if (i == centerLine.size() - 1) {
			QPointF p1 = centerLine.at(centerLine.size() - 2);
			QPointF p2 = centerLine.at(centerLine.size() - 1);
			v = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v);
		} else {
			QPointF p1 = centerLine.at(i - 1);
			QPointF p2 = centerLine.at(i);
			QPointF p3 = centerLine.at(i + 1);
			QPointF v1 = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v1);
			QPointF v2 = iRIC::normalize(p3 - p2);
			iRIC::rotateVector270(v2);
			v = iRIC::normalize(v1 + v2);
		}

		QPointF lb = centerLine.at(i);
		lb.setX(lb.x() - leftDist * v.x());
		lb.setY(lb.y() - leftDist * v.y());
		leftBank->push_back(lb);

		QPointF rb = centerLine.at(i);
		rb.setX(rb.x() + rightDist * v.x());
		rb.setY(rb.y() + rightDist * v.y());
		rightBank->push_back(rb);
	}
}
