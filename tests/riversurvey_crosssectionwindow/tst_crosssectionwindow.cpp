#include "testsupport.h"
#include "modaldialogresponder.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingGL2PSOpenGL2);
VTK_MODULE_INIT(vtkIOExportOpenGL2);

#include <QAbstractButton>
#include <QTextStream>

#include <gui/main/iricmainwindow.h>
#include <gui/solverdef/solverdefinitionlist.h>
#include <guibase/widget/realnumbereditwidget.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/versionnumber.h>
#include <pre/preprocessorwindow.h>
#include <pre/datamodel/preprocessorgeodatagroupdataitem.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodata/geodataexporter.h>
#include <guicore/pre/geodata/geodataimporter.h>
#include <guicore/pre/geodata/geodataimportersetting.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <geodata/riversurvey/geodatarivercrosssection.h>
#include <geodata/riversurvey/geodatariverpathpoint.h>
#include <geodata/riversurvey/geodatariversurvey.h>
#include <geodata/riversurvey/geodatariversurveycrosssectionwindow.h>
#include <misc/errormessage.h>
#include <misc/iricrootpath.h>
#include <misc/iricundostack.h>

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QDialogButtonBox>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QRadioButton>
#include <QPushButton>
#include <QSettings>
#include <QScopeGuard>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QtTest>
#include <vtkStructuredGrid.h>
#include <vtkNew.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include <cmath>
#include <map>
#include <limits>
#include <vector>

namespace {

const double POS_TOL = 1.0E-3;
const double POINT_TOL = 1.0E-2;

QString repositoryRoot()
{
	QDir dir(QCoreApplication::applicationDirPath());
	// tests/riversurvey_crosssectionwindow/<configuration>
	dir.cdUp();
	dir.cdUp();
	dir.cdUp();
	return dir.absolutePath();
}

QString testProjectPath()
{
	// the default path is set by tools/riversurvey_tests/run_riversurvey_tests.ps1
	return qEnvironmentVariable("IRIC_RS_TEST_PROJECT");
}

GeoDataExporter* findExporter(GeoDataRiverSurvey* rs, const QString& extension)
{
	for (auto e : rs->creator()->exporters()) {
		if (e->fileDialogFilters().join(";").contains(extension)) {return e;}
	}
	return nullptr;
}

// gives access to the protected import function, that is used when the user imports data from the menu.
class GroupAccess : public PreProcessorGeoDataGroupDataItem
{
public:
	static void importFile(PreProcessorGeoDataGroupDataItem* group, GeoDataImporter* importer, const QString& filename)
	{
		auto func = static_cast<void (PreProcessorGeoDataGroupDataItem::*)(GeoDataImporter*, const QString&, const QString&)>(&GroupAccess::importGeoData);
		(group->*func)(importer, filename, "");
	}
};

std::vector<double> positions(GeoDataRiverPathPoint* p)
{
	std::vector<double> ret;
	for (const auto& a : p->crosssection().AltitudeInfo()) {
		ret.push_back(a.position());
	}
	return ret;
}

bool isSameName(const QString& n1, const QString& n2)
{
	bool ok1, ok2;
	double v1 = n1.toDouble(&ok1);
	double v2 = n2.toDouble(&ok2);
	if (ok1 && ok2) {return std::abs(v1 - v2) <= 1.0E-6;}
	return n1 == n2;
}

void processEvents(int msec = 50)
{
	QTest::qWait(msec);
}

struct VegetationSpec {
	double distance;
	double width;
	double height;
	int submerged;
};

// sets vegetation data through the vegetation table, in the same way as the user edits it.
void setVegetations(GeoDataRiverSurveyCrosssectionWindow* w, const std::vector<VegetationSpec>& specs)
{
	auto view = w->findChild<QTableView*>("vegetationTableView");
	auto model = qobject_cast<QStandardItemModel*> (view->model());
	model->removeRows(0, model->rowCount());
	for (int i = 0; i < static_cast<int> (specs.size()); ++i) {
		const auto& s = specs.at(i);
		model->insertRow(i);
		model->setData(model->index(i, 1), s.width);
		model->setData(model->index(i, 2), s.height);
		model->setData(model->index(i, 3), s.submerged);
		model->setData(model->index(i, 4), 0);
		model->setData(model->index(i, 5), 0);
		model->setData(model->index(i, 6), 0);
		model->setData(model->index(i, 7), 0.0);
		model->setData(model->index(i, 0), s.distance);
	}
	processEvents();
}

} // namespace

class CrosssectionWindowTest : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
	void cleanup();

	void openAndCloseWindowRepeatedly();
	void switchAllCrosssections();
	void deleteRowsSelectedInDescendingOrder();
	void deleteVegetationsSelectedInDescendingOrder();
	void actionsWithoutEditTarget();
	void undoAfterWindowIsClosed();
	void drawVegetationOutsideCrosssection();
	void unsetOdnIsKeptAfterTableEdit();
	void movingOdnPointKeepsOdnIndex();
	void referenceComboBoxWithoutSelection();
	void vegetationSubmergedCheckboxCanBeUnchecked();
	void exportImportRoundTripWithCsvCenterLine();
	void distanceOriginUndo_data();
	void distanceOriginUndo();
	void landXmlCoordinates_data();
	void landXmlCoordinates();
	void honryuDistanceOrigins_data();
	void honryuDistanceOrigins();
	void landXmlDistanceOrigins();
	void landXmlProfileElevations();
	void rivInputValidation_data();
	void rivInputValidation();
	void odnInactivePoints_data();
	void odnInactivePoints();
	void odnFieldWidths_data();
	void odnFieldWidths();
	void rivCenterLineOrigins_data();
	void rivCenterLineOrigins();
	void centerLineCsvValidation_data();
	void centerLineCsvValidation();

private:
	GeoDataRiverSurveyCrosssectionWindow* openWindow(GeoDataRiverSurvey* rs, const QString& crosssection);
	void closeWindow(GeoDataRiverSurveyCrosssectionWindow* w);
	void closeAllWindows();
	void refreshSurveyList();
	GeoDataRiverPathPoint* findPointWithMinimumAltitudes(GeoDataRiverSurvey* rs, int count);

	iRICMainWindow* m_mainWindow {nullptr};
	ModalDialogResponder* m_responder {nullptr};
	PreProcessorGeoDataGroupDataItem* m_group {nullptr};
	std::vector<GeoDataRiverSurvey*> m_surveys;
	QTemporaryDir m_tmpDir;
};

void CrosssectionWindowTest::initTestCase()
{
	QVERIFY(m_tmpDir.isValid());

	QString root = qEnvironmentVariable("IRIC_RS_TEST_ROOT");
	QVERIFY2(! root.isEmpty(), "IRIC_RS_TEST_ROOT is not set. Run the test with tools/riversurvey_tests/run_riversurvey_tests.ps1");
	QDir solversDir(root);
	solversDir.cdUp();
	solversDir.cdUp();
	QVERIFY2(solversDir.exists("solvers"), qPrintable(QString("solvers folder not found in %1").arg(solversDir.absolutePath())));
	QVERIFY2(QDir(root).exists("definition.xml"), qPrintable(QString("definition.xml not found in %1").arg(root)));
	QVERIFY2(QDir(root).exists("unzip.exe") && QDir(root).exists("zip.exe"), qPrintable(QString("zip.exe or unzip.exe not found in %1").arg(root)));
	iRICRootPath::set(root);

	QString udunits = QDir(root).absoluteFilePath("udunits/udunits2.xml");
	if (! QFile::exists(udunits)) {
		udunits = QDir::home().absoluteFilePath("iRIC_v4/guis/prepost/udunits/udunits2.xml");
	}
	if (QFile::exists(udunits)) {
		qputenv("UDUNITS2_XML_PATH", QDir::toNativeSeparators(udunits).toUtf8());
	}

	// settings are isolated from the real iRIC GUI by the application name (see main()).
	QSettings settings;
	settings.setValue("general/workspace", m_tmpDir.filePath("workspace"));
	settings.setValue("infodialog/riversurveyedit", true);
	settings.setValue("infodialog/geodatariversurvey_add_vegetation", true);
	QDir(m_tmpDir.path()).mkdir("workspace");

	QString src = testProjectPath();
	QVERIFY2(QFile::exists(src), qPrintable(QString("test project not found: %1 (set IRIC_RS_TEST_PROJECT)").arg(src)));
	QString dst = m_tmpDir.filePath("check.ipro");
	QVERIFY(QFile::copy(src, dst));

	m_responder = new ModalDialogResponder(this);

	try {
		m_mainWindow = new iRICMainWindow(false);
	} catch (const ErrorMessage& msg) {
		QFAIL(qPrintable(QString("iRICMainWindow could not be created: %1").arg(msg)));
	} catch (const std::exception& e) {
		QFAIL(qPrintable(QString("iRICMainWindow could not be created: %1").arg(e.what())));
	}
	m_mainWindow->resize(1600, 1000);
	if (! qEnvironmentVariableIsSet("IRIC_RS_TEST_HEADLESS")) {m_mainWindow->show();}
	processEvents(200);
	for (auto list : m_mainWindow->findChildren<SolverDefinitionList*>()) {
		for (auto s : list->solverList()) {
			qInfo("solver: %s %s (%s)", qPrintable(s->name().c_str()), qPrintable(s->version().toString()), qPrintable(s->absoluteFolderName()));
		}
	}
	m_mainWindow->openProject(dst);
	processEvents(500);
	for (const auto& r : m_responder->records()) {
		qInfo("dialog while opening: %s", qPrintable(r));
	}

	refreshSurveyList();
	QVERIFY2(m_group != nullptr, "No geographic data group with river survey data found");
	QVERIFY2(m_surveys.size() >= 2, "check.ipro should contain two river survey data");
	iRICUndoStack::instance().clear();
}

void CrosssectionWindowTest::cleanupTestCase()
{
	closeAllWindows();
	if (m_mainWindow != nullptr) {
		m_mainWindow->closeProject();
		processEvents(200);
		delete m_mainWindow;
		m_mainWindow = nullptr;
	}
}

void CrosssectionWindowTest::cleanup()
{
	m_responder->clearHandler();
	closeAllWindows();
}

void CrosssectionWindowTest::distanceOriginUndo_data()
{
	QTest::addColumn<bool>("expand");
	QTest::addColumn<bool>("apply");
	QTest::addColumn<bool>("cancel");
	for (bool expand : {false, true}) {
		for (bool apply : {false, true}) {
			for (bool cancel : {false, true}) {
				auto name = QString("%1-%2-%3").arg(expand ? "expand" : "shift")
					.arg(apply ? "apply" : "direct").arg(cancel ? "cancel" : "ok");
				QTest::newRow(qPrintable(name)) << expand << apply << cancel;
			}
		}
	}
}

void CrosssectionWindowTest::distanceOriginUndo()
{
	QFETCH(bool, expand);
	QFETCH(bool, apply);
	QFETCH(bool, cancel);
	auto rs = m_surveys.front();
	auto p = findPointWithMinimumAltitudes(rs, 3);
	QVERIFY(p != nullptr);
	for (auto point = rs->headPoint()->nextPoint(); point != nullptr; point = point->nextPoint()) {
		point->IsSelected = (point == p);
	}
	auto& cs = p->crosssection();
	const double originalShift = cs.leftShift();
	cs.setLeftShift(50);
	const auto before = positions(p);
	const auto center = p->position();
	const double amount = expand ? 1.1 : 1.0;
	iRICUndoStack::instance().clear();
	QVERIFY(QMetaObject::invokeMethod(rs, expand ? "expandSelectedPoints" : "shiftSelectedPoints", Qt::DirectConnection));
	QDialog* dialog = nullptr;
	const QString className = expand ? "GeoDataRiverPathPointExpandDialog" : "GeoDataRiverPathPointShiftDialog";
	for (auto d : m_mainWindow->findChildren<QDialog*>()) {
		if (d->metaObject()->className() == className) {dialog = d;}
	}
	QVERIFY(dialog != nullptr);
	if (expand) {
		auto radio = dialog->findChild<QRadioButton*>("ratioRadioButton");
		QVERIFY(radio != nullptr);
		radio->setChecked(true);
	}
	auto edit = dialog->findChild<RealNumberEditWidget*>(expand ? "ratioEdit" : "shiftEdit");
	QVERIFY(edit != nullptr);
	edit->setValue(amount);
	if (apply) {
		auto buttons = dialog->findChild<QDialogButtonBox*>("buttonBox");
		QVERIFY(buttons != nullptr);
		buttons->button(QDialogButtonBox::Apply)->click();
	}
	QVERIFY(QMetaObject::invokeMethod(dialog, cancel ? "reject" : "accept", Qt::DirectConnection));
	QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	auto check = [&](bool changed) {
		const double shift = changed ? (expand ? 50 * amount : 50 + amount) : 50;
		QVERIFY(std::abs(cs.leftShift() - shift) < 1E-8);
		const auto after = positions(p);
		QCOMPARE(after.size(), before.size());
		for (size_t i = 0; i < before.size(); ++i) {
			const double pos = changed ? (expand ? before[i] * amount : before[i] - amount) : before[i];
			QVERIFY(std::abs(after[i] - pos) < 1E-8);
			QVERIFY(std::abs(after[i] + cs.leftShift() - (pos + shift)) < 1E-8);
		}
		auto expectedCenter = changed && ! expand ? center + p->crosssectionDirection() * amount : center;
		QVERIFY(QLineF(p->position(), expectedCenter).length() < 1E-8);
	};
	check(! cancel);
	if (! cancel) {
		iRICUndoStack::instance().undo();
		check(false);
		iRICUndoStack::instance().redo();
		check(true);
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().clear();
	cs.setLeftShift(originalShift);
}

void CrosssectionWindowTest::landXmlCoordinates_data()
{
	QTest::addColumn<QPointF>("offset");
	QTest::newRow("zero") << QPointF(0, 0);
	QTest::newRow("nonzero") << QPointF(123456.125, -987654.875);
}

void CrosssectionWindowTest::landXmlCoordinates()
{
	QFETCH(QPointF, offset);
	auto rs = m_surveys.front();
	auto pd = m_group->projectData();
	auto exporter = findExporter(rs, "*.xml");
	QVERIFY(exporter != nullptr);
	const auto originalOffset = pd->mainfile()->offset();
	auto restoreOffset = qScopeGuard([&]() {pd->mainfile()->setOffset(originalOffset.x(), originalOffset.y());});
	pd->mainfile()->setOffset(offset.x(), offset.y());
	rs->cancelBackgroundGridUpdate();
	vtkNew<vtkStructuredGrid> grid;
	grid->DeepCopy(rs->backgroundGrid());
	const auto filename = m_tmpDir.filePath("coordinates.xml");
	bool ok = exporter->doExport(rs, filename, "", m_mainWindow, pd);
	QVERIFY(ok);
	QFile file(filename);
	QVERIFY(file.open(QFile::ReadOnly));
	QDomDocument doc;
	QVERIFY(doc.setContent(&file));
	QMap<QString, QPointF> bankPoints;
	for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
		bankPoints[p->name() + " L"] = p->crosssectionPosition(- p->crosssection().leftShift()) + offset;
		bankPoints[p->name() + " R"] = p->crosssectionPosition(p->crosssection().rightBank(true).position()) + offset;
	}
	for (const auto& tag : {"Start", "End"}) {
		auto nodes = doc.elementsByTagName(tag);
		QVERIFY(nodes.size() > 0);
		for (int i = 0; i < nodes.size(); ++i) {
			auto e = nodes.at(i).toElement();
			QVERIFY(bankPoints.contains(e.attribute("name")));
			auto expected = bankPoints.value(e.attribute("name"));
			auto vals = e.text().split(" ", QString::SkipEmptyParts);
			QCOMPARE(vals.size(), 2);
			QVERIFY(std::abs(vals[0].toDouble() - expected.y()) < 0.00051);
			QVERIFY(std::abs(vals[1].toDouble() - expected.x()) < 0.00051);
		}
	}
	auto values = vtkDoubleArray::SafeDownCast(grid->GetPointData()->GetArray("Data"));
	QVERIFY(values != nullptr);
	auto nodes = doc.elementsByTagName("P");
	QCOMPARE(nodes.size(), static_cast<int>(grid->GetNumberOfPoints()));
	for (int i = 0; i < nodes.size(); ++i) {
		double pos[3];
		grid->GetPoint(i, pos);
		auto vals = nodes.at(i).toElement().text().split(" ", QString::SkipEmptyParts);
		QCOMPARE(vals.size(), 3);
		QVERIFY(std::abs(vals[0].toDouble() - (pos[1] + offset.y())) < 0.00051);
		QVERIFY(std::abs(vals[1].toDouble() - (pos[0] + offset.x())) < 0.00051);
		QVERIFY(std::abs(vals[2].toDouble() - values->GetValue(i)) < 0.00051);
	}
}

void CrosssectionWindowTest::honryuDistanceOrigins_data()
{
	QTest::addColumn<bool>("csv");
	QTest::addColumn<bool>("legacy");
	QTest::addColumn<double>("shift");
	for (bool csv : {false, true}) {
		for (bool legacy : {false, true}) {
			for (double shift : {-50.0, 0.0, 50.0}) {
				auto name = QString("%1-%2-%3").arg(csv ? "csv" : "text").arg(legacy ? "legacy" : "default").arg(shift);
				QTest::newRow(qPrintable(name)) << csv << legacy << shift;
			}
		}
	}
}

void CrosssectionWindowTest::honryuDistanceOrigins()
{
	QFETCH(bool, csv);
	QFETCH(bool, legacy);
	QFETCH(double, shift);
	auto rs = m_surveys.front();
	QMap<GeoDataRiverPathPoint*, double> shifts;
	for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
		shifts[p] = p->crosssection().leftShift();
		p->crosssection().setLeftShift(shift);
	}
	auto first = rs->headPoint()->nextPoint();
	const bool active = first->crosssection().AltitudeInfo().front().active();
	first->crosssection().AltitudeInfo().front().setActive(false);
	auto restore = qScopeGuard([&]() {
		for (auto it = shifts.begin(); it != shifts.end(); ++it) {it.key()->crosssection().setLeftShift(it.value());}
		first->crosssection().AltitudeInfo().front().setActive(active);
		m_responder->clearHandler();
	});
	bool settingFound = false;
	int defaultIndex = -1;
	m_responder->setHandler([&](QWidget* dialog) {
		if (QString(dialog->metaObject()->className()) != "GeoDataRiverSurveyHonryuExporterSettingDialog") {return false;}
		auto combo = dialog->findChild<QComboBox*>("crossSectionDistanceComboBox");
		if (combo != nullptr) {
			settingFound = true;
			defaultIndex = combo->currentIndex();
			if (legacy) {combo->setCurrentIndex(1);}
		}
		qobject_cast<QDialog*>(dialog)->accept();
		return true;
	});
	auto exporter = findExporter(rs, "*.csv");
	QVERIFY(exporter != nullptr);
	auto filename = m_tmpDir.filePath("honryu.txt");
	QVERIFY(exporter->doExport(rs, filename, csv ? "CSV File (*.csv)" : "Text File (*.txt)", m_mainWindow, m_group->projectData()));
	QVERIFY(settingFound);
	QCOMPARE(defaultIndex, 0);
	QFile file(filename);
	QVERIFY(file.open(QFile::ReadOnly | QFile::Text));
	QTextStream stream(&file);
	for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
		const auto& cs = p->crosssection();
		const double origin = legacy ? p->crosssection().leftBank(true).position() : - cs.leftShift();
		for (const auto& alt : cs.AltitudeInfo()) {
			if (! alt.active()) {continue;}
			const auto vals = stream.readLine().split(csv ? "," : " ", QString::SkipEmptyParts);
			QCOMPARE(vals.size(), 4);
			QCOMPARE(vals[0], p->name());
			QVERIFY(std::abs(vals[2].toDouble() - (alt.position() - origin)) < 0.00051);
			QVERIFY(std::abs(vals[3].toDouble() - alt.height()) < 0.00051);
		}
		QVERIFY(stream.readLine().isEmpty());
	}
	QVERIFY(stream.atEnd());
}

void CrosssectionWindowTest::landXmlDistanceOrigins()
{
	auto rs = m_surveys.front();
	QMap<GeoDataRiverPathPoint*, double> shifts;
	for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
		shifts[p] = p->crosssection().leftShift();
		p->crosssection().setLeftShift(0);
	}
	auto restore = qScopeGuard([&]() {
		for (auto it = shifts.begin(); it != shifts.end(); ++it) {it.key()->crosssection().setLeftShift(it.value());}
	});
	auto exporter = findExporter(rs, "*.xml");
	QVERIFY(exporter != nullptr);
	const auto filename = m_tmpDir.filePath("origin.xml");
	for (bool outside : {false, true}) {
		auto first = rs->headPoint()->nextPoint();
		if (outside) {first->crosssection().setLeftShift(-first->crosssection().rightBank(true).position() - 10);}
		m_responder->clearRecords();
		QVERIFY(exporter->doExport(rs, filename, "", m_mainWindow, m_group->projectData()));
		QFile file(filename);
		QVERIFY(file.open(QFile::ReadOnly));
		QDomDocument doc;
		QVERIFY(doc.setContent(&file));
		QCOMPARE(doc.elementsByTagName("Profile").size(), outside ? 0 : 1);
		if (outside) {QVERIFY(m_responder->records().join("\n").contains("profile was omitted"));}
		auto nodes = doc.elementsByTagName("CrossSect");
		int index = 0;
		double station = 0;
		QPointF previous = first->crosssectionPosition(-first->crosssection().leftShift());
		for (auto p = first; p != nullptr; p = p->nextPoint(), ++index) {
			QVERIFY(index < nodes.size());
			auto e = nodes.at(index).toElement();
			auto mark = p->crosssectionPosition(-p->crosssection().leftShift());
			station += QLineF(previous, mark).length();
			previous = mark;
			QCOMPARE(e.attribute("name"), p->name());
			QVERIFY(std::abs(e.attribute("sta").toDouble() - station) < 0.00051);
			auto vals = e.elementsByTagName("PntList2D").at(0).toElement().text().split(" ", QString::SkipEmptyParts);
			int n = 0;
			for (const auto& alt : p->crosssection().AltitudeInfo()) {
				if (! alt.active()) {continue;}
				QVERIFY(n + 1 < vals.size());
				QVERIFY(std::abs(vals[n++].toDouble() - (alt.position() + p->crosssection().leftShift())) < 0.00051);
				QVERIFY(std::abs(vals[n++].toDouble() - alt.height()) < 0.00051);
			}
			QCOMPARE(n, vals.size());
		}
		QCOMPARE(index, nodes.size());
	}
}
void CrosssectionWindowTest::landXmlProfileElevations()
{
	auto rs = m_surveys.front();
	QMap<GeoDataRiverPathPoint*, GeoDataRiverCrosssection::AltitudeList> originals;
	QMap<GeoDataRiverPathPoint*, double> shifts;
	int index = 0;
	for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint(), ++index) {
		auto& cs = p->crosssection();
		originals[p] = cs.AltitudeInfo();
		shifts[p] = cs.leftShift();
		cs.setLeftShift(0);
		for (auto& alt : cs.AltitudeInfo()) {alt.setHeight(100 + index * 10 + 2 * alt.position());}
	}
	auto restore = qScopeGuard([&]() {
		for (auto it = originals.begin(); it != originals.end(); ++it) {
			it.key()->crosssection().AltitudeInfo() = it.value();
			it.key()->crosssection().setLeftShift(shifts.value(it.key()));
		}
	});
	auto exporter = findExporter(rs, "*.xml");
	QVERIFY(exporter != nullptr);
	auto filename = m_tmpDir.filePath("profile.xml");
	QVERIFY(exporter->doExport(rs, filename, "", m_mainWindow, m_group->projectData()));
	QFile file(filename);
	QVERIFY(file.open(QFile::ReadOnly));
	QDomDocument doc;
	QVERIFY(doc.setContent(&file));
	auto nodes = doc.elementsByTagName("PVI");
	QCOMPARE(nodes.size(), index);
	auto sections = doc.elementsByTagName("CrossSect");
	for (int i = 0; i < nodes.size(); ++i) {
		auto vals = nodes.at(i).toElement().text().split(" ", QString::SkipEmptyParts);
		QCOMPARE(vals.size(), 2);
		QVERIFY(std::abs(vals[0].toDouble() - sections.at(i).toElement().attribute("sta").toDouble()) < 0.00051);
		QVERIFY(std::abs(vals[1].toDouble() - (100 + i * 10)) < 0.00051);
	}
}

void CrosssectionWindowTest::rivInputValidation_data()
{
	QTest::addColumn<QByteArray>("content");
	QTest::addColumn<bool>("valid");
	const QByteArray survey = "#survey\nA 0 0 100 0\n";
	QTest::newRow("short-survey") << QByteArray("#survey\nA 0 0\n") << false;
	QTest::newRow("missing-count") << survey + "#x-section\nA\n" << false;
	QTest::newRow("negative-count") << survey + "#x-section\nA -1\n" << false;
	QTest::newRow("fractional-count") << survey + "#x-section\nA 2.5\n0 0 100 1\n" << false;
	QTest::newRow("huge-count") << survey + "#x-section\nA 1e100\n" << false;
	QTest::newRow("zero-count") << survey + "#x-section\nA 0\n" << false;
	QTest::newRow("partial-indices") << survey + "#x-section\nA 2 1 2\n0 0 100 1\n" << false;
	QTest::newRow("zero-index") << survey + "#x-section\nA 2 0 1 2 2\n0 0 100 1\n" << false;
	QTest::newRow("large-index") << survey + "#x-section\nA 2 1 1 2 3\n0 0 100 1\n" << false;
	QTest::newRow("unordered-indices") << survey + "#x-section\nA 2 1 2 1 2\n0 0 100 1\n" << false;
	QTest::newRow("odd-values") << survey + "#x-section\nA 2\n0 0 100\n" << false;
	QTest::newRow("too-many-points") << survey + "#x-section\nA 2\n0 0 50 1 100 2\n" << false;
	QTest::newRow("truncated") << survey + "#x-section\nA 2\n0 0\n" << false;
	QTest::newRow("nonfinite-coordinate") << QByteArray("#survey\nA nan 0 100 0\n") << false;
	QTest::newRow("coincident-banks") << QByteArray("#survey\nA 0 0 0 0\n") << false;
	QTest::newRow("nonfinite-distance") << survey + "#x-section\nA 2\n0 0 inf 1\n" << false;
	QTest::newRow("nonfinite-height") << survey + "#x-section\nA 2\n0 nan 100 1\n" << false;
	QTest::newRow("empty") << QByteArray() << false;
	QTest::newRow("missing-file") << QByteArray() << false;
	QTest::newRow("normal") << survey + "#x-section\nA 2\n10 1 90 2\n" << true;
	QTest::newRow("four-indices") << survey + "#x-section\nA 2 1 1 2 2\n10 1 90 2\n" << true;
	QTest::newRow("survey-only") << survey << true;
	QTest::newRow("blank-lines") << survey + "#x-section\nA 2\n\n10 1\n\n90 2\n\n" << true;
}

void CrosssectionWindowTest::rivInputValidation()
{
	QFETCH(QByteArray, content);
	QFETCH(bool, valid);
	const auto filename = m_tmpDir.filePath("validation.riv");
	QFile file(filename);
	if (QByteArray(QTest::currentDataTag()) == "missing-file") {
		if (file.exists()) {QVERIFY(file.remove());}
	} else {
		QVERIFY(file.open(QFile::WriteOnly));
		QCOMPARE(file.write(content), static_cast<qint64>(content.size()));
		file.close();
	}
	auto importer = m_group->importer("riversurvey");
	QVERIFY(importer != nullptr);
	auto previous = importer->setting();
	std::unique_ptr<GeoDataImporterSetting> setting(importer->createSetting());
	setting->setFileName(filename);
	importer->setSetting(setting.get());
	auto restore = qScopeGuard([&]() {importer->setSetting(previous);});
	m_responder->clearRecords();
	int count = 0;
	QCOMPARE(importer->importInit(&count, nullptr, m_group, m_mainWindow, false), valid);
	if (! valid) {
		QVERIFY(m_responder->records().join("\n").contains("validation.riv"));
		QVERIFY(! m_responder->records().join("\n").contains("GeoDataRiverSurveyImporterSettingDialog"));
	}
}

void CrosssectionWindowTest::odnInactivePoints_data()
{
	QTest::addColumn<bool>("proceed");
	QTest::newRow("cancel") << false;
	QTest::newRow("continue") << true;
}

void CrosssectionWindowTest::odnInactivePoints()
{
	QFETCH(bool, proceed);
	auto rs = m_surveys.front();
	auto p = findPointWithMinimumAltitudes(rs, 3);
	QVERIFY(p != nullptr);
	auto& alt = p->crosssection().AltitudeInfo().front();
	const bool active = alt.active();
	alt.setActive(false);
	auto restore = qScopeGuard([&]() {alt.setActive(active); m_responder->clearHandler();});
	bool warned = false;
	m_responder->setHandler([&](QWidget* widget) {
		auto box = qobject_cast<QMessageBox*>(widget);
		if (box == nullptr || ! box->text().contains("inactive point flags")) {return false;}
		warned = box->text().contains(p->name());
		box->button(proceed ? QMessageBox::Yes : QMessageBox::No)->click();
		return true;
	});
	auto exporter = findExporter(rs, "*.odn");
	QVERIFY(exporter != nullptr);
	auto filename = m_tmpDir.filePath("inactive.odn");
	if (QFile::exists(filename)) {QVERIFY(QFile::remove(filename));}
	QCOMPARE(exporter->doExport(rs, filename, "", m_mainWindow, m_group->projectData()), proceed);
	QVERIFY(warned);
	QCOMPARE(QFile::exists(filename), proceed);
	if (! proceed) {return;}
	QFile file(filename);
	QVERIFY(file.open(QFile::ReadOnly | QFile::Text));
	QTextStream stream(&file);
	std::vector<GeoDataRiverPathPoint*> points;
	for (auto point = rs->headPoint()->nextPoint(); point != nullptr; point = point->nextPoint()) {points.push_back(point);}
	for (auto it = points.rbegin(); it != points.rend(); ++it) {
		auto point = *it;
		const auto header = stream.readLine();
		QCOMPARE(header.mid(10, 5).toInt(), static_cast<int>(point->crosssection().AltitudeInfo().size()));
		QCOMPARE(header.mid(0, 5).toInt(), point->odn().nb(2) + 1);
		QCOMPARE(header.mid(5, 5).toInt(), point->odn().nb(3) + 1);
		QStringList values;
		for (const auto& a : point->crosssection().AltitudeInfo()) {
			if (values.isEmpty()) {values = stream.readLine().split(" ", QString::SkipEmptyParts);}
			QVERIFY(values.size() >= 2);
			QVERIFY(std::abs(values.takeFirst().toDouble() - (a.position() + point->crosssection().leftShift())) < 0.0051);
			QVERIFY(std::abs(values.takeFirst().toDouble() - a.height()) < 0.0051);
		}
		QVERIFY(values.isEmpty());
	}
	QVERIFY(stream.atEnd());
}

void CrosssectionWindowTest::odnFieldWidths_data()
{
	QTest::addColumn<QString>("field");
	QTest::addColumn<double>("value");
	QTest::addColumn<bool>("valid");
	QTest::newRow("distance-max") << QString("distance") << 9999.99 << true;
	QTest::newRow("distance-min") << QString("distance") << -999.99 << true;
	QTest::newRow("distance-rounded-overflow") << QString("distance") << 9999.999 << false;
	QTest::newRow("distance-negative-overflow") << QString("distance") << -1000.0 << false;
	QTest::newRow("height-max") << QString("height") << 9999.99 << true;
	QTest::newRow("height-overflow") << QString("height") << 10000.0 << false;
	QTest::newRow("height-nan") << QString("height") << std::numeric_limits<double>::quiet_NaN() << false;
	QTest::newRow("distance-infinity") << QString("distance") << std::numeric_limits<double>::infinity() << false;
	QTest::newRow("span-max") << QString("span") << 9999999.99 << true;
	QTest::newRow("span-overflow") << QString("span") << 10000000.0 << false;
	QTest::newRow("name-max") << QString("name") << 10.0 << true;
	QTest::newRow("name-overflow") << QString("name") << 11.0 << false;
}

void CrosssectionWindowTest::odnFieldWidths()
{
	QFETCH(QString, field);
	QFETCH(double, value);
	QFETCH(bool, valid);
	auto rs = m_surveys.front();
	auto p = rs->headPoint()->nextPoint();
	auto& cs = p->crosssection();
	const auto originalAltitudes = cs.AltitudeInfo();
	const double shift = cs.leftShift();
	const double span = p->odn().spanDistance();
	const auto name = p->name();
	auto restore = qScopeGuard([&]() {
		cs.AltitudeInfo() = originalAltitudes;
		cs.setLeftShift(shift);
		p->odn().setSpanDistance(span);
		p->setName(name);
		m_responder->clearHandler();
	});
	if (field == "distance") {cs.AltitudeInfo().front().setPosition(value - shift);}
	if (field == "height") {cs.AltitudeInfo().front().setHeight(value);}
	if (field == "span") {p->odn().setSpanDistance(value);}
	if (field == "name") {p->setName(QString(static_cast<int>(value), '1'));}
	bool rejected = false;
	m_responder->setHandler([&](QWidget* widget) {
		auto box = qobject_cast<QMessageBox*>(widget);
		if (box == nullptr || ! box->text().contains("fixed-width")) {return false;}
		rejected = true;
		box->accept();
		return true;
	});
	auto filename = m_tmpDir.filePath("width.odn");
	QFile file(filename);
	QVERIFY(file.open(QFile::WriteOnly));
	QCOMPARE(file.write("unchanged"), qint64(9));
	file.close();
	auto exporter = findExporter(rs, "*.odn");
	QVERIFY(exporter != nullptr);
	QCOMPARE(exporter->doExport(rs, filename, "", m_mainWindow, m_group->projectData()), valid);
	QCOMPARE(rejected, ! valid);
	QVERIFY(file.open(QFile::ReadOnly));
	if (! valid) {QCOMPARE(file.readAll(), QByteArray("unchanged")); return;}
	QTextStream stream(&file);
	while (! stream.atEnd()) {
		const auto header = stream.readLine();
		QCOMPARE(header.size(), 65);
		int count = header.mid(10, 5).toInt();
		QVERIFY(count > 0);
		int read = 0;
		while (read < count) {
			auto vals = stream.readLine().split(" ", QString::SkipEmptyParts);
			QVERIFY(! vals.isEmpty());
			QCOMPARE(vals.size() % 2, 0);
			for (const auto& val : vals) {
				bool ok;
				val.toDouble(&ok);
				QVERIFY(ok);
			}
			read += vals.size() / 2;
		}
		QCOMPARE(read, count);
	}
}

void CrosssectionWindowTest::rivCenterLineOrigins_data()
{
	QTest::addColumn<double>("center");
	QTest::addColumn<bool>("named");
	QTest::addColumn<bool>("multiple");
	QTest::addColumn<bool>("largeCoordinates");
	for (double center : {-200.0, -20.0, 0.0, 50.0}) {
		for (bool named : {false, true}) {
			auto tag = QString("%1-%2").arg(center).arg(named ? "named" : "unnamed");
			QTest::newRow(qPrintable(tag)) << center << named << false << false;
		}
	}
	QTest::newRow("multiple-named") << 70.0 << true << true << false;
	QTest::newRow("multiple-unnamed") << 10.0 << false << true << false;
	QTest::newRow("large-oblique-coordinates") << -20.0 << true << false << true;
}

void CrosssectionWindowTest::rivCenterLineOrigins()
{
	QFETCH(double, center);
	QFETCH(bool, named);
	QFETCH(bool, multiple);
	QFETCH(bool, largeCoordinates);
	auto worldPosition = [&](double x, double y) {
		return largeCoordinates ? QPointF(1234567.1234567 + std::cos(0.37) * x - std::sin(0.37) * y,
			2345678.2345678 + std::sin(0.37) * x + std::cos(0.37) * y) : QPointF(x, y);
	};
	auto coordinates = [&](double x, double y, const QString& separator) {
		const auto p = worldPosition(x, y);
		return QString::number(p.x(), 'g', 17) + separator + QString::number(p.y(), 'g', 17);
	};
	const double minimum = center < -100 ? -300 : -60;
	const double maximum = center < -100 ? -100 : 100;
	const auto input = m_tmpDir.filePath("center.riv");
	QFile file(input);
	QVERIFY(file.open(QFile::WriteOnly));
	const auto data = (QString("#survey\nA %1 %2\nB %3 %4\n").arg(coordinates(0, 0, " ")).arg(coordinates(100, 0, " "))
		.arg(coordinates(0, 10, " ")).arg(coordinates(100, 10, " ")) +
		QString("#x-section\nA 3\n%1 1 %2 2 %3 3\nB 3\n%1 4 %2 5 %3 6\n").arg(minimum).arg(center).arg(maximum)).toUtf8();
	QCOMPARE(file.write(data), static_cast<qint64>(data.size()));
	file.close();
	QFile csv(input + ".csv");
	QVERIFY(csv.open(QFile::WriteOnly));
	QString csvText = "x,y,name\n\n";
	if (multiple) {
		csvText += named ? "10,-1,other1\n10,11,other2\n70,11,other3\n70,0,A\n50,10,B\n\n" :
			"10,-1\n10,11\n70,11\n70,0\n50,10\n\n";
	} else if (named) {
		csvText += coordinates(center, 10, ",") + ",B\n" + coordinates(center, 0, ",") + ",A\n\n";
	} else {
		csvText += QString("%1,-1\n%1,11\n\n").arg(center);
	}
	csv.write(csvText.toUtf8());
	csv.close();
	m_responder->setHandler([](QWidget* dialog) {
		if (QString(dialog->metaObject()->className()) != "GeoDataRiverSurveyImporterSettingDialog") {return false;}
		dialog->findChild<QRadioButton*>("csvFileRadioButton")->setChecked(true);
		qobject_cast<QDialog*>(dialog)->accept();
		return true;
	});
	auto clearHandler = qScopeGuard([&]() {m_responder->clearHandler();});
	auto importer = m_group->importer("riversurvey");
	QVERIFY(importer != nullptr);
	auto import = [&](const QString& filename) -> GeoDataRiverSurvey* {
		QSet<GeoData*> known;
		for (auto item : m_group->geoDatas()) {known.insert(item->geoData());}
		GroupAccess::importFile(m_group, importer, filename);
		for (auto item : m_group->geoDatas()) {
			if (! known.contains(item->geoData())) {return dynamic_cast<GeoDataRiverSurvey*>(item->geoData());}
		}
		return nullptr;
	};
	m_responder->clearRecords();
	auto original = import(input);
	QVERIFY(original != nullptr);
	auto offset = m_group->projectData()->mainfile()->offset();
	int index = 0;
	for (auto p = original->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint(), ++index) {
		const double expectedCenter = multiple && named && p->name() == "B" ? 50 : center;
		QVERIFY(QLineF(p->position() + offset, worldPosition(expectedCenter, index * 10)).length() < 1E-6);
		QVERIFY(std::abs(p->crosssection().leftShift() - expectedCenter) < 1E-6);
	}
	QCOMPARE(index, 2);
	QVERIFY(! m_responder->records().join("\n").contains("does not cross"));
	auto exporter = findExporter(original, "*.riv");
	QVERIFY(exporter != nullptr);
	auto output = m_tmpDir.filePath("center-roundtrip.riv");
	QVERIFY(exporter->doExport(original, output, "", m_mainWindow, m_group->projectData()));
	auto reimported = import(output);
	QVERIFY(reimported != nullptr);
	QVERIFY(! m_responder->records().join("\n").contains("does not cross"));
	auto p = original->headPoint()->nextPoint();
	auto q = reimported->headPoint()->nextPoint();
	for (; p != nullptr && q != nullptr; p = p->nextPoint(), q = q->nextPoint()) {
		QCOMPARE(p->name(), q->name());
		QVERIFY(QLineF(p->position(), q->position()).length() < POS_TOL);
		QVERIFY(QLineF(p->crosssectionDirection(), q->crosssectionDirection()).length() < 1E-8);
		QVERIFY(std::abs(p->crosssection().leftShift() - q->crosssection().leftShift()) < POS_TOL);
		const auto& a = p->crosssection().AltitudeInfo();
		const auto& b = q->crosssection().AltitudeInfo();
		QCOMPARE(a.size(), b.size());
		for (size_t i = 0; i < a.size(); ++i) {
			QVERIFY(std::abs(a[i].position() + p->crosssection().leftShift() - b[i].position() - q->crosssection().leftShift()) < POS_TOL);
			QVERIFY(QLineF(p->crosssectionPosition(a[i].position()), q->crosssectionPosition(b[i].position())).length() < POS_TOL);
			QCOMPARE(a[i].height(), b[i].height());
		}
	}
	QVERIFY(p == nullptr && q == nullptr);
}

void CrosssectionWindowTest::centerLineCsvValidation_data()
{
	QTest::addColumn<QByteArray>("csv");
	QTest::addColumn<QString>("error");
	QTest::newRow("duplicate-names") << QByteArray("x,y,name\n10,0,A\n20,10,A\n") << QString("Duplicate cross-section name");
	QTest::newRow("no-points") << QByteArray("x,y,name\n\n") << QString("No center line points");
	QTest::newRow("bad-second-record") << QByteArray("x,y,name\nabc,0,A\n20,10,B\n") << QString("Line 2");
	QTest::newRow("nan-coordinate") << QByteArray("x,y,name\nnan,0,A\n20,10,B\n") << QString("Line 2");
	QTest::newRow("short-record") << QByteArray("x,y,name\n10\n") << QString("Line 2");
}

void CrosssectionWindowTest::centerLineCsvValidation()
{
	QFETCH(QByteArray, csv);
	QFETCH(QString, error);
	const auto filename = m_tmpDir.filePath("csv-validation.riv");
	QFile riv(filename);
	QVERIFY(riv.open(QFile::WriteOnly));
	riv.write("#survey\nA 0 0 100 0\nB 0 10 100 10\n#x-section\nA 2\n0 1 100 2\nB 2\n0 3 100 4\n");
	riv.close();
	QFile file(filename + ".csv");
	QVERIFY(file.open(QFile::WriteOnly));
	QCOMPARE(file.write(csv), static_cast<qint64>(csv.size()));
	file.close();
	m_responder->setHandler([](QWidget* dialog) {
		if (QString(dialog->metaObject()->className()) != "GeoDataRiverSurveyImporterSettingDialog") {return false;}
		dialog->findChild<QRadioButton*>("csvFileRadioButton")->setChecked(true);
		qobject_cast<QDialog*>(dialog)->accept();
		return true;
	});
	auto restore = qScopeGuard([&]() {m_responder->clearHandler();});
	m_responder->clearRecords();
	const auto count = m_group->geoDatas().size();
	GroupAccess::importFile(m_group, m_group->importer("riversurvey"), filename);
	QCOMPARE(m_group->geoDatas().size(), count);
	QVERIFY(m_responder->records().join("\n").contains(error));
}

void CrosssectionWindowTest::refreshSurveyList()
{
	m_group = nullptr;
	m_surveys.clear();

	auto preWindow = dynamic_cast<PreProcessorWindow*> (m_mainWindow->preProcessorWindow());
	auto model = dynamic_cast<QObject*> (preWindow->dataModel());
	auto groups = model->findChildren<PreProcessorGeoDataGroupDataItem*>();
	if (groups.isEmpty()) {
		QMap<QString, int> classes;
		for (auto o : model->findChildren<QObject*>()) {
			classes[o->metaObject()->className()] += 1;
		}
		for (auto it = classes.begin(); it != classes.end(); ++it) {
			qInfo("child of data model: %s x %d", qPrintable(it.key()), it.value());
		}
	}
	for (auto g : groups) {
		for (auto item : g->geoDatas()) {
			auto rs = dynamic_cast<GeoDataRiverSurvey*> (item->geoData());
			if (rs == nullptr) {continue;}
			m_group = g;
			m_surveys.push_back(rs);
		}
	}
}

GeoDataRiverSurveyCrosssectionWindow* CrosssectionWindowTest::openWindow(GeoDataRiverSurvey* rs, const QString& crosssection)
{
	auto before = m_mainWindow->findChildren<GeoDataRiverSurveyCrosssectionWindow*>();
	m_group->openCrossSectionWindow(rs, crosssection);
	processEvents(100);
	for (auto w : m_mainWindow->findChildren<GeoDataRiverSurveyCrosssectionWindow*>()) {
		if (! before.contains(w)) {return w;}
	}
	return nullptr;
}

void CrosssectionWindowTest::closeWindow(GeoDataRiverSurveyCrosssectionWindow* w)
{
	auto sub = qobject_cast<QMdiSubWindow*> (w->parentWidget());
	if (sub != nullptr) {
		sub->close();
	} else {
		w->close();
	}
	QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	processEvents(100);
}

void CrosssectionWindowTest::closeAllWindows()
{
	if (m_mainWindow == nullptr) {return;}
	for (auto w : m_mainWindow->findChildren<GeoDataRiverSurveyCrosssectionWindow*>()) {
		closeWindow(w);
	}
}

GeoDataRiverPathPoint* CrosssectionWindowTest::findPointWithMinimumAltitudes(GeoDataRiverSurvey* rs, int count)
{
	auto p = rs->headPoint()->nextPoint();
	while (p != nullptr) {
		const auto& cs = p->crosssection();
		if (static_cast<int> (cs.AltitudeInfo().size()) >= count && ! cs.fixedPointLSet() && ! cs.fixedPointRSet()) {return p;}
		p = p->nextPoint();
	}
	return nullptr;
}

void CrosssectionWindowTest::openAndCloseWindowRepeatedly()
{
	auto rs = m_surveys.at(0);
	auto name = rs->headPoint()->nextPoint()->name();
	for (int i = 0; i < 10; ++i) {
		auto w = openWindow(rs, name);
		QVERIFY(w != nullptr);
		closeWindow(w);
	}
	QCOMPARE(m_mainWindow->findChildren<GeoDataRiverSurveyCrosssectionWindow*>().size(), 0);
}

void CrosssectionWindowTest::switchAllCrosssections()
{
	auto rs = m_surveys.at(0);
	auto w = openWindow(rs, rs->headPoint()->nextPoint()->name());
	QVERIFY(w != nullptr);
	auto combo = w->findChild<QComboBox*>("crosssectionComboBox");
	QVERIFY(combo != nullptr);
	QVERIFY(combo->count() > 0);

	for (int i = 0; i < combo->count(); ++i) {
		combo->setCurrentIndex(i);
		processEvents(10);
		QCOMPARE(w->crosssectionName(), combo->itemText(i));
		if (w->target() != nullptr) {
			QVERIFY(isSameName(w->target()->name(), combo->itemText(i)));
			QCOMPARE(w->tableView()->model()->rowCount(), static_cast<int> (w->target()->crosssection().AltitudeInfo().size()));
		}
	}
	// names that are the same numerically should appear only once (B-09)
	for (int i = 0; i < combo->count(); ++i) {
		for (int j = i + 1; j < combo->count(); ++j) {
			QVERIFY2(! isSameName(combo->itemText(i), combo->itemText(j)),
							 qPrintable(QString("duplicated names: %1, %2").arg(combo->itemText(i)).arg(combo->itemText(j))));
		}
	}
}

void CrosssectionWindowTest::deleteRowsSelectedInDescendingOrder()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 6);
	QVERIFY(p != nullptr);
	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);
	QCOMPARE(w->target(), p);

	auto before = positions(p);
	int n = static_cast<int> (before.size());
	int rowA = 1;
	int rowB = n - 2;

	auto selModel = w->tableView()->selectionModel();
	auto model = w->tableView()->model();
	selModel->clearSelection();
	// select the row with larger index first.
	selModel->select(model->index(rowB, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	selModel->select(model->index(rowA, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	QVERIFY(w->deleteAction()->isEnabled());
	w->deleteAction()->trigger();
	processEvents();

	std::vector<double> expected;
	for (int i = 0; i < n; ++i) {
		if (i == rowA || i == rowB) {continue;}
		expected.push_back(before.at(i));
	}
	QCOMPARE(positions(p), expected);

	iRICUndoStack::instance().undo();
	processEvents();
	QCOMPARE(positions(p), before);
}

void CrosssectionWindowTest::deleteVegetationsSelectedInDescendingOrder()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 2);
	QVERIFY(p != nullptr);

	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);
	setVegetations(w, {{0, 1, 1, 0}, {10, 1, 2, 0}, {20, 1, 3, 0}, {30, 1, 4, 0}});
	QCOMPARE(static_cast<int> (p->jmk().items().size()), 4);

	auto vegView = w->findChild<QTableView*>("vegetationTableView");
	QVERIFY(vegView != nullptr);
	auto selModel = vegView->selectionModel();
	auto model = vegView->model();
	QCOMPARE(model->rowCount(), 4);

	selModel->clearSelection();
	selModel->select(model->index(3, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	selModel->select(model->index(1, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	w->deleteSelectedVegetationAction()->trigger();
	processEvents();

	QCOMPARE(static_cast<int> (p->jmk().items().size()), 2);
	QCOMPARE(p->jmk().items().at(0).distance, 0.0);
	QCOMPARE(p->jmk().items().at(1).distance, 20.0);
}

void CrosssectionWindowTest::actionsWithoutEditTarget()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 4);
	QVERIFY(p != nullptr);
	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);
	QVERIFY(w->target() != nullptr);

	// select a row, then hide the target survey.
	auto selModel = w->tableView()->selectionModel();
	selModel->select(w->tableView()->model()->index(1, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

	auto surveys = w->findChild<QTableWidget*>("surveysTableWidget");
	QVERIFY(surveys != nullptr);
	int targetRow = -1;
	for (int i = 0; i < surveys->rowCount(); ++i) {
		if (surveys->item(i, 1)->data(Qt::DisplayRole).toString() == rs->caption()) {targetRow = i;}
	}
	QVERIFY(targetRow >= 0);
	emit surveys->itemClicked(surveys->item(targetRow, 0));
	processEvents();
	QVERIFY(w->target() == nullptr);
	QVERIFY(! w->deleteAction()->isEnabled());

	// none of these may crash.
	QList<QAction*> actions;
	actions << w->deleteAction() << w->editFromSelectedPointWithDialogAction() << w->editFromSelectedPointAction()
					<< w->addPointAction() << w->addVegetationAction() << w->editSelectedVegetationAction() << w->deleteSelectedVegetationAction()
					<< w->odnLeftStartAction() << w->odnLeftMiddleAction() << w->odnLeftLowAction()
					<< w->odnRightLowAction() << w->odnRightMiddleAction() << w->odnRightStartAction()
					<< w->leftAddAction() << w->leftSubAction() << w->rightAddAction() << w->rightSubAction()
					<< w->inactivateByWEOnlyThisAction();
	for (auto a : actions) {
		a->trigger();
		processEvents(20);
	}

	// moving the mouse over the graphics view
	auto view = w->findChild<QAbstractItemView*>("graphicsView");
	QVERIFY(view != nullptr);
	auto vp = view->viewport();
	for (int i = 1; i < 10; ++i) {
		QTest::mouseMove(vp, QPoint(vp->width() * i / 10, vp->height() / 2));
		QMouseEvent move(QEvent::MouseMove, QPointF(vp->width() * i / 10, vp->height() / 2), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
		QApplication::sendEvent(vp, &move);
	}

	// editing water surface elevation table
	auto wses = w->findChild<QTableWidget*>("wsesTableWidget");
	QVERIFY(wses != nullptr);
	if (wses->rowCount() > 0) {
		wses->item(0, 2)->setData(Qt::DisplayRole, 123.0);
		processEvents();
	}
	auto pixmap = w->snapshot();
	QVERIFY(! pixmap.isNull());
}

void CrosssectionWindowTest::undoAfterWindowIsClosed()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 6);
	QVERIFY(p != nullptr);
	auto beforePositions = positions(p);
	auto beforeJmk = p->jmk().items().size();

	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);

	// command 1: delete an elevation point
	auto selModel = w->tableView()->selectionModel();
	selModel->select(w->tableView()->model()->index(2, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	w->deleteAction()->trigger();
	processEvents();

	// command 2: edit vegetation data from the table
	auto vegModel = w->findChild<QTableView*>("vegetationTableView")->model();
	if (vegModel->rowCount() == 0) {
		setVegetations(w, {{0, 1, 1, 0}});
	}
	vegModel->setData(vegModel->index(0, 2), 3.5);
	processEvents();

	// command 3: edit elevation from the table
	auto model = w->tableView()->model();
	model->setData(model->index(1, 3), model->data(model->index(1, 3)).toDouble() + 0.5);
	processEvents();

	closeWindow(w);
	QCOMPARE(m_mainWindow->findChildren<GeoDataRiverSurveyCrosssectionWindow*>().size(), 0);

	auto& stack = iRICUndoStack::instance();
	QVERIFY(stack.count() >= 3);
	while (stack.canUndo()) {
		stack.undo();
		processEvents(10);
	}
	QCOMPARE(positions(p), beforePositions);
	while (stack.canRedo()) {
		stack.redo();
		processEvents(10);
	}
	QCOMPARE(static_cast<int> (positions(p).size()), static_cast<int> (beforePositions.size()) - 1);
	Q_UNUSED(beforeJmk);
}

void CrosssectionWindowTest::drawVegetationOutsideCrosssection()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 2);
	QVERIFY(p != nullptr);

	const auto& cs = p->crosssection();
	double firstDistance = cs.AltitudeInfo().front().position() + cs.leftShift();

	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);
	setVegetations(w, {{firstDistance - 100, 10, 5, 0}});
	QCOMPARE(static_cast<int> (p->jmk().items().size()), 1);
	auto pixmap = w->snapshot();
	QVERIFY(! pixmap.isNull());
}

void CrosssectionWindowTest::unsetOdnIsKeptAfterTableEdit()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 4);
	QVERIFY(p != nullptr);
	for (int i = 0; i < 6; ++i) {
		p->odn().setNb(i, -1);
	}
	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);

	auto model = w->tableView()->model();
	model->setData(model->index(1, 3), model->data(model->index(1, 3)).toDouble() + 0.1);
	processEvents();

	for (int i = 0; i < 6; ++i) {
		QCOMPARE(p->odn().nb(i), -1);
	}
}

void CrosssectionWindowTest::movingOdnPointKeepsOdnIndex()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 5);
	QVERIFY(p != nullptr);
	p->odn().setNb(2, 2);
	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);

	const auto& alist = p->crosssection().AltitudeInfo();
	double newPos = (alist.at(1).position() + alist.at(2).position() * 3) / 4;
	auto model = w->tableView()->model();
	model->setData(model->index(2, 2), newPos);
	processEvents();

	QVERIFY(std::abs(p->crosssection().AltitudeInfo().at(2).position() - newPos) < 1.0E-9);
	QCOMPARE(p->odn().nb(2), 2);
}

void CrosssectionWindowTest::referenceComboBoxWithoutSelection()
{
	auto rs = m_surveys.at(0);
	auto w = openWindow(rs, rs->headPoint()->nextPoint()->name());
	QVERIFY(w != nullptr);
	auto check = w->findChild<QCheckBox*>("referenceCheckBox");
	auto combo = w->findChild<QComboBox*>("referenceComboBox");
	QVERIFY(check != nullptr);
	QVERIFY(combo != nullptr);
	check->setChecked(true);
	combo->setCurrentIndex(-1);
	processEvents();
	auto pixmap = w->snapshot();
	QVERIFY(! pixmap.isNull());
}

void CrosssectionWindowTest::vegetationSubmergedCheckboxCanBeUnchecked()
{
	auto rs = m_surveys.at(0);
	auto p = findPointWithMinimumAltitudes(rs, 2);
	QVERIFY(p != nullptr);

	auto w = openWindow(rs, p->name());
	QVERIFY(w != nullptr);
	setVegetations(w, {{0, 1, 1, 1}});
	auto vegView = w->findChild<QTableView*>("vegetationTableView");
	QVERIFY(vegView != nullptr);
	auto model = vegView->model();
	auto index = model->index(0, 3);
	QCOMPARE(model->data(index).toInt(), 1);

	auto delegate = vegView->itemDelegate();
	QStyleOptionViewItem option;
	auto editor = delegate->createEditor(vegView->viewport(), option, index);
	QVERIFY(editor != nullptr);
	delegate->setEditorData(editor, index);
	auto checkBox = editor->findChild<QCheckBox*>();
	QVERIFY(checkBox != nullptr);
	{
		QSignalBlocker blocker(checkBox);
		checkBox->setChecked(false);
	}
	delegate->setModelData(editor, model, index);
	delete editor;
	processEvents();

	QCOMPARE(model->data(index).toInt(), 0);
	QCOMPARE(p->jmk().items().at(0).submerged, 0);
}

void CrosssectionWindowTest::exportImportRoundTripWithCsvCenterLine()
{
	auto pd = m_group->projectData();
	auto importer = m_group->importer("riversurvey");
	QVERIFY(importer != nullptr);

	std::vector<GeoDataRiverSurvey*> originals = m_surveys;
	std::vector<GeoDataRiverSurvey*> known = m_surveys;
	QStringList report;
	int problems = 0;

	for (int sIdx = 0; sIdx < static_cast<int> (originals.size()); ++sIdx) {
		auto rs = originals.at(sIdx);
		GeoDataExporter* exporter = nullptr;
		for (auto e : rs->creator()->exporters()) {
			if (e->fileDialogFilters().join(";").contains("*.riv")) {exporter = e;}
		}
		QVERIFY(exporter != nullptr);
		QString rivFile = m_tmpDir.filePath(QString("export_%1.riv").arg(sIdx));
		QVERIFY(exporter->doExport(rs, rivFile, "", m_mainWindow, pd));
		QVERIFY(QFile::exists(rivFile + ".csv"));

		// import with "Import center line from CSV file" setting
		m_responder->setHandler([](QWidget* dialog) {
			if (QString(dialog->metaObject()->className()) != "GeoDataRiverSurveyImporterSettingDialog") {return false;}
			auto radio = dialog->findChild<QRadioButton*>("csvFileRadioButton");
			if (radio == nullptr) {return false;}
			radio->setChecked(true);
			qobject_cast<QDialog*> (dialog)->accept();
			return true;
		});
		m_responder->clearRecords();
		GroupAccess::importFile(m_group, importer, rivFile);
		processEvents(200);
		m_responder->clearHandler();
		for (const auto& r : m_responder->records()) {
			if (r.contains("does not cross")) {
				report << QString("[%1] %2").arg(rs->caption()).arg(r);
				++ problems;
			}
		}

		// find the imported data
		GeoDataRiverSurvey* imported = nullptr;
		for (auto item : m_group->geoDatas()) {
			auto r = dynamic_cast<GeoDataRiverSurvey*> (item->geoData());
			if (r == nullptr) {continue;}
			if (std::find(known.begin(), known.end(), r) != known.end()) {continue;}
			imported = r;
			break;
		}
		QVERIFY(imported != nullptr);
		known.push_back(imported);

		std::map<QString, GeoDataRiverPathPoint*> importedPoints;
		for (auto p = imported->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
			importedPoints.insert({p->name(), p});
		}

		int sections = 0;
		for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
			++ sections;
			auto it = importedPoints.find(p->name());
			if (it == importedPoints.end()) {
				report << QString("[%1/%2] not imported").arg(rs->caption()).arg(p->name());
				++ problems;
				continue;
			}
			auto q = it->second;

			double centerDiff = std::hypot(p->position().x() - q->position().x(), p->position().y() - q->position().y());

			std::vector<GeoDataRiverCrosssection::Altitude> origActive;
			for (const auto& a : p->crosssection().AltitudeInfo()) {
				if (a.active()) {origActive.push_back(a);}
			}
			const auto& impAlts = q->crosssection().AltitudeInfo();
			if (origActive.size() != impAlts.size()) {
				report << QString("[%1/%2] point count differs: %3 -> %4").arg(rs->caption()).arg(p->name()).arg(origActive.size()).arg(impAlts.size());
				++ problems;
				continue;
			}
			double maxDisplayDiff = 0;
			double maxWorldDiff = 0;
			double maxHeightDiff = 0;
			for (int i = 0; i < static_cast<int> (origActive.size()); ++i) {
				double x1 = origActive.at(i).position() + p->crosssection().leftShift();
				double x2 = impAlts.at(i).position() + q->crosssection().leftShift();
				maxDisplayDiff = std::max(maxDisplayDiff, std::abs(x1 - x2));
				auto w1 = p->crosssectionPosition(origActive.at(i).position());
				auto w2 = q->crosssectionPosition(impAlts.at(i).position());
				maxWorldDiff = std::max(maxWorldDiff, std::hypot(w1.x() - w2.x(), w1.y() - w2.y()));
				maxHeightDiff = std::max(maxHeightDiff, std::abs(origActive.at(i).height() - impAlts.at(i).height()));
			}
			double firstShift = (impAlts.front().position() + q->crosssection().leftShift()) - (origActive.front().position() + p->crosssection().leftShift());
			double predicted = - (origActive.front().position() + p->crosssection().leftShift());
			// the importer shifts points with the same distance by 1 mm, so the tolerance for each point is larger.
			if (centerDiff > POS_TOL || std::abs(firstShift) > POS_TOL || maxDisplayDiff > POINT_TOL || maxWorldDiff > POINT_TOL || maxHeightDiff > POS_TOL) {
				report << QString("[%1/%2] center %3, display x %4 (shift of first point %5, predicted by E-04 %6), map %7, height %8")
									.arg(rs->caption()).arg(p->name())
									.arg(centerDiff, 0, 'f', 4).arg(maxDisplayDiff, 0, 'f', 4).arg(firstShift, 0, 'f', 4).arg(predicted, 0, 'f', 4)
									.arg(maxWorldDiff, 0, 'f', 4).arg(maxHeightDiff, 0, 'f', 4);
				++ problems;
			}
		}
		report << QString("[%1] %2 sections compared").arg(rs->caption()).arg(sections);
	}

	QFile reportFile(QDir(repositoryRoot()).absoluteFilePath("tests/riversurvey_results/roundtrip_report.txt"));
	QDir().mkpath(QFileInfo(reportFile).absolutePath());
	if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream s(&reportFile);
		s.setCodec("UTF-8");
		for (const auto& line : report) {
			s << line << "\n";
		}
	}
	for (const auto& line : report) {
		qInfo("%s", qPrintable(line));
	}
	QVERIFY2(problems == 0, qPrintable(QString("%1 problems found in export/import round trip. See tests/riversurvey_results/roundtrip_report.txt").arg(problems)));
}

int main(int argc, char* argv[])
{
	RiverSurveyTestSupport::disableCrashDialogs();

	// iRICMainWindow parses QCoreApplication::arguments(), so test options are not passed to QApplication.
	static int appArgc = 1;
	static char* appArgv[] = {argv[0], nullptr};
	QApplication app(appArgc, appArgv);
	QCoreApplication::setOrganizationName("iRIC Organization");
	QCoreApplication::setOrganizationDomain("i-ric.org");
	QCoreApplication::setApplicationName("iRIC GUI 4 RiverSurveyTest");

	CrosssectionWindowTest test;
	return QTest::qExec(&test, argc, argv);
}

#include "tst_crosssectionwindow.moc"
