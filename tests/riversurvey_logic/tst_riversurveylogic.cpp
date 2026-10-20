#include "testsupport.h"

#include <QTextStream>

#include <vector>

#include <geodata/riversurvey/geodatarivercrosssection.h>
#include <geodata/riversurvey/geodatarivercrosssectionlogic.h>
#include <geodata/riversurvey/geodatariverpathpointodndata.h>

#include <QCoreApplication>
#include <QtTest>

#include <vector>

using Altitude = GeoDataRiverCrosssection::Altitude;
using AltitudeList = GeoDataRiverCrosssection::AltitudeList;

namespace {

AltitudeList makeList(const std::vector<double>& positions)
{
	AltitudeList ret;
	for (double p : positions) {
		ret.push_back(Altitude(p, p * 0.1));
	}
	return ret;
}

std::vector<double> positions(const AltitudeList& list)
{
	std::vector<double> ret;
	for (const auto& a : list) {
		ret.push_back(a.position());
	}
	return ret;
}

void setupCrosssection(GeoDataRiverCrosssection* cross, const std::vector<double>& pos)
{
	cross->AltitudeInfo() = makeList(pos);
}

GeoDataRiverPathPointOdnData makeOdn(int l0, int l1, int l2, int r3, int r4, int r5)
{
	GeoDataRiverPathPointOdnData odn;
	odn.setNb(0, l0);
	odn.setNb(1, l1);
	odn.setNb(2, l2);
	odn.setNb(3, r3);
	odn.setNb(4, r4);
	odn.setNb(5, r5);
	return odn;
}

std::vector<int> nbs(const GeoDataRiverPathPointOdnData& odn)
{
	std::vector<int> ret;
	for (int i = 0; i < 6; ++i) {
		ret.push_back(odn.nb(i));
	}
	return ret;
}

} // namespace

class RiverSurveyLogicTest : public QObject
{
	Q_OBJECT

private slots:
	// GeoDataRiverCrosssection::removePoint (B-01)
	void removePoint_ascendingIndices();
	void removePoint_descendingIndices();
	void removePoint_firstAndLastInDescendingOrder();
	void removePoint_fixedPointIsRejected();
	void removePoint_tooFewPointsIsRejected();
	void removePoint_updatesFixedPointIndices();

	// sortedUniqueIndices (B-02)
	void sortedUniqueIndices();

	// remapOdn (B-04, B-05, B-17, #1570)
	void remapOdn_unsetStaysUnset();
	void remapOdn_addPointShiftsIndex();
	void remapOdn_addPointAtRightEnd();
	void remapOdn_removeRightEndPoint();
	void remapOdn_removeOdnPointFallsBackToEnds();
	void remapOdn_moveOdnPointKeepsIndex();
	void remapOdn_emptyBefore();
	void remapOdn_emptyAfter();

	// editFromPoint (B-03)
	void editFromPoint_leftBetweenPoints();
	void editFromPoint_leftOnPoint();
	void editFromPoint_leftOutside();
	void editFromPoint_rightBetweenPoints();
	void editFromPoint_rightOnPoint();
	void editFromPoint_rightOutside();
	void editFromPoint_invalidIndex();

	// crosssection names (B-09, cff35e30d)
	void isSameCrosssectionName_data();
	void isSameCrosssectionName();
	void buildCrosssectionNames_numeric();
	void buildCrosssectionNames_numericDuplicatesInDifferentNotation();
	void buildCrosssectionNames_mixed();
};

void RiverSurveyLogicTest::removePoint_ascendingIndices()
{
	GeoDataRiverCrosssection cross;
	setupCrosssection(&cross, {-5, -3, -1, 1, 3, 5});
	cross.removePoint(std::vector<int>({1, 4}));
	QCOMPARE(positions(cross.AltitudeInfo()), std::vector<double>({-5, -1, 1, 5}));
}

void RiverSurveyLogicTest::removePoint_descendingIndices()
{
	GeoDataRiverCrosssection cross;
	setupCrosssection(&cross, {-5, -3, -1, 1, 3, 5});
	cross.removePoint(std::vector<int>({4, 1}));
	QCOMPARE(positions(cross.AltitudeInfo()), std::vector<double>({-5, -1, 1, 5}));
}

void RiverSurveyLogicTest::removePoint_firstAndLastInDescendingOrder()
{
	GeoDataRiverCrosssection cross;
	setupCrosssection(&cross, {-5, -3, -1, 1, 3, 5});
	cross.removePoint(std::vector<int>({5, 0}));
	QCOMPARE(positions(cross.AltitudeInfo()), std::vector<double>({-3, -1, 1, 3}));
}

void RiverSurveyLogicTest::removePoint_fixedPointIsRejected()
{
	GeoDataRiverCrosssection cross;
	setupCrosssection(&cross, {-5, -3, -1, 1, 3, 5});
	cross.setFixedPointL(1);
	bool thrown = false;
	try {
		cross.removePoint(std::vector<int>({1}));
	} catch (GeoDataRiverCrosssection::ErrorCodes ec) {
		thrown = (ec == GeoDataRiverCrosssection::ec_FixDelete);
	}
	QVERIFY(thrown);
	QCOMPARE(static_cast<int> (cross.AltitudeInfo().size()), 6);
}

void RiverSurveyLogicTest::removePoint_tooFewPointsIsRejected()
{
	GeoDataRiverCrosssection cross;
	setupCrosssection(&cross, {-1, 1, 3});
	bool thrown = false;
	try {
		cross.removePoint(std::vector<int>({2, 1}));
	} catch (GeoDataRiverCrosssection::ErrorCodes ec) {
		thrown = (ec == GeoDataRiverCrosssection::ec_AltitudesMustExistTwo);
	}
	QVERIFY(thrown);
	QCOMPARE(positions(cross.AltitudeInfo()), std::vector<double>({-1, 1, 3}));
}

void RiverSurveyLogicTest::removePoint_updatesFixedPointIndices()
{
	GeoDataRiverCrosssection cross;
	setupCrosssection(&cross, {-7, -5, -3, -1, 1, 3, 5, 7});
	cross.setFixedPointL(1);
	cross.setFixedPointR(6);
	cross.removePoint(std::vector<int>({4, 3}));

	QCOMPARE(cross.fixedPointLIndex(), 1);
	QCOMPARE(cross.fixedPointRIndex(), 4);
	QCOMPARE(cross.fixedPointL().position(), -5.0);
	QCOMPARE(cross.fixedPointR().position(), 5.0);
}

void RiverSurveyLogicTest::sortedUniqueIndices()
{
	auto ret = GeoDataRiverCrosssectionLogic::sortedUniqueIndices({4, 1, 4, 0, 2});
	QCOMPARE(ret, std::vector<int>({0, 1, 2, 4}));
}

void RiverSurveyLogicTest::remapOdn_unsetStaysUnset()
{
	auto before = makeList({-5, -3, -1, 1, 3, 5});
	auto after = makeList({-5, -3, 1, 3, 5});
	GeoDataRiverPathPointOdnData odn;
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({-1, -1, -1, -1, -1, -1}));
}

void RiverSurveyLogicTest::remapOdn_addPointShiftsIndex()
{
	auto before = makeList({-5, -3, -1, 1, 3, 5});
	auto after = makeList({-5, -4, -3, -1, 1, 3, 5});
	auto odn = makeOdn(0, 1, 2, 3, 4, 5);
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({0, 2, 3, 4, 5, 6}));
}

void RiverSurveyLogicTest::remapOdn_addPointAtRightEnd()
{
	auto before = makeList({-5, -3, -1, 1, 3, 5});
	auto after = makeList({-5, -3, -1, 1, 3, 5, 7});
	auto odn = makeOdn(0, 1, 2, 3, 4, 5);
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({0, 1, 2, 3, 4, 5}));
}

void RiverSurveyLogicTest::remapOdn_removeRightEndPoint()
{
	auto before = makeList({-5, -3, -1, 1, 3, 5});
	auto after = makeList({-5, -3, -1, 1, 3});
	auto odn = makeOdn(0, 1, 2, 3, 4, 5);
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({0, 1, 2, 3, 4, 4}));
}

void RiverSurveyLogicTest::remapOdn_removeOdnPointFallsBackToEnds()
{
	auto before = makeList({-5, -3, -1, 1, 3, 5});
	auto after = makeList({-5, -3, 3, 5});
	auto odn = makeOdn(0, 1, 2, 3, 4, 5);
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({0, 1, 0, 3, 2, 3}));
}

void RiverSurveyLogicTest::remapOdn_moveOdnPointKeepsIndex()
{
	auto before = makeList({-5, -3, -1, 1, 3, 5});
	auto after = makeList({-5, -3, -0.5, 1, 3, 5});
	auto odn = makeOdn(0, 1, 2, 3, 4, 5);
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({0, 1, 2, 3, 4, 5}));
}

void RiverSurveyLogicTest::remapOdn_emptyBefore()
{
	AltitudeList before;
	auto after = makeList({-1, 1});
	auto odn = makeOdn(0, 0, 0, 1, 1, 1);
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({0, 0, 0, 1, 1, 1}));
}

void RiverSurveyLogicTest::remapOdn_emptyAfter()
{
	auto before = makeList({-1, 1});
	AltitudeList after;
	auto odn = makeOdn(0, 0, 0, 1, 1, 1);
	auto ret = GeoDataRiverCrosssectionLogic::remapOdn(before, after, odn);
	QCOMPARE(nbs(ret), std::vector<int>({-1, -1, -1, -1, -1, -1}));
}

void RiverSurveyLogicTest::editFromPoint_leftBetweenPoints()
{
	auto before = makeList({0, 1, 2, 3, 4});
	int newIndex;
	auto after = GeoDataRiverCrosssectionLogic::editFromPoint(before, 3, Altitude(1.5, 0), &newIndex);
	QCOMPARE(positions(after), std::vector<double>({0, 1, 1.5, 3, 4}));
	QCOMPARE(newIndex, 2);
}

void RiverSurveyLogicTest::editFromPoint_leftOnPoint()
{
	auto before = makeList({0, 1, 2, 3, 4});
	int newIndex;
	auto after = GeoDataRiverCrosssectionLogic::editFromPoint(before, 3, Altitude(1, 0), &newIndex);
	QCOMPARE(positions(after), std::vector<double>({0, 1, 3, 4}));
	QCOMPARE(newIndex, 1);
	QCOMPARE(after.at(1).height(), 0.0);
}

void RiverSurveyLogicTest::editFromPoint_leftOutside()
{
	auto before = makeList({0, 1, 2, 3, 4});
	int newIndex;
	auto after = GeoDataRiverCrosssectionLogic::editFromPoint(before, 2, Altitude(-1, 0), &newIndex);
	QCOMPARE(positions(after), std::vector<double>({-1, 2, 3, 4}));
	QCOMPARE(newIndex, 0);
}

void RiverSurveyLogicTest::editFromPoint_rightBetweenPoints()
{
	auto before = makeList({0, 1, 2, 3, 4});
	int newIndex;
	auto after = GeoDataRiverCrosssectionLogic::editFromPoint(before, 1, Altitude(2.5, 0), &newIndex);
	QCOMPARE(positions(after), std::vector<double>({0, 1, 2.5, 3, 4}));
	QCOMPARE(newIndex, 2);
}

void RiverSurveyLogicTest::editFromPoint_rightOnPoint()
{
	auto before = makeList({0, 1, 2, 3, 4});
	int newIndex;
	auto after = GeoDataRiverCrosssectionLogic::editFromPoint(before, 1, Altitude(3, 0), &newIndex);
	QCOMPARE(positions(after), std::vector<double>({0, 1, 3, 4}));
	QCOMPARE(newIndex, 2);
	QCOMPARE(after.at(2).height(), 0.0);
}

void RiverSurveyLogicTest::editFromPoint_rightOutside()
{
	auto before = makeList({0, 1, 2, 3, 4});
	int newIndex;
	auto after = GeoDataRiverCrosssectionLogic::editFromPoint(before, 2, Altitude(10, 0), &newIndex);
	QCOMPARE(positions(after), std::vector<double>({0, 1, 2, 10}));
	QCOMPARE(newIndex, 3);
}

void RiverSurveyLogicTest::editFromPoint_invalidIndex()
{
	auto before = makeList({0, 1, 2});
	int newIndex;
	auto after = GeoDataRiverCrosssectionLogic::editFromPoint(before, 5, Altitude(10, 0), &newIndex);
	QCOMPARE(positions(after), positions(before));
	QCOMPARE(newIndex, -1);
}

void RiverSurveyLogicTest::isSameCrosssectionName_data()
{
	QTest::addColumn<QString>("n1");
	QTest::addColumn<QString>("n2");
	QTest::addColumn<bool>("same");

	QTest::newRow("same string") << "No.1" << "No.1" << true;
	QTest::newRow("different string") << "No.1" << "No.2" << false;
	QTest::newRow("integer and real") << "100" << "100.0" << true;
	QTest::newRow("exponent") << "1e2" << "100" << true;
	QTest::newRow("within tolerance") << "100" << "100.0000001" << true;
	QTest::newRow("outside tolerance") << "100" << "100.00001" << false;
	QTest::newRow("number and string") << "100" << "100a" << false;
}

void RiverSurveyLogicTest::isSameCrosssectionName()
{
	QFETCH(QString, n1);
	QFETCH(QString, n2);
	QFETCH(bool, same);
	QCOMPARE(GeoDataRiverCrosssectionLogic::isSameCrosssectionName(n1, n2), same);
}

void RiverSurveyLogicTest::buildCrosssectionNames_numeric()
{
	auto names = GeoDataRiverCrosssectionLogic::buildCrosssectionNames({"200", "100", "50", "100"});
	QCOMPARE(names, QList<QString>({"50", "100", "200"}));
}

void RiverSurveyLogicTest::buildCrosssectionNames_numericDuplicatesInDifferentNotation()
{
	auto names = GeoDataRiverCrosssectionLogic::buildCrosssectionNames({"100", "200", "100.0", "50"});
	QCOMPARE(names, QList<QString>({"50", "100", "200"}));
}

void RiverSurveyLogicTest::buildCrosssectionNames_mixed()
{
	auto names = GeoDataRiverCrosssectionLogic::buildCrosssectionNames({"B", "100", "A", "100.0"});
	QCOMPARE(names, QList<QString>({"100", "A", "B"}));
}

int main(int argc, char* argv[])
{
	RiverSurveyTestSupport::disableCrashDialogs();
	QCoreApplication app(argc, argv);
	RiverSurveyLogicTest test;
	return QTest::qExec(&test, argc, argv);
}

#include "tst_riversurveylogic.moc"
