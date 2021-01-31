#include "project.h"
#include "../arbitraryhwm/arbitraryhwm.h"
#include "../crosssection/crosssection.h"
#include "../leftbankhwm/leftbankhwm.h"
#include "../points/pointsgraphicssetting.h"
#include "../rightbankhwm/rightbankhwm.h"
#include "../../geom/geometrypoint.h"
#include "../../misc/mathutil.h"

#include "private/project_impl.h"

#include <misc/ziparchive.h>

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QTemporaryDir>
#include <QXmlStreamWriter>
#include <QWidget>

#include <map>
#include <string>
#include <vector>

namespace {

const QString PROJECT_FILENAME = "project.xml";

void addToVector(const Points& points, const BaseLine& line, std::multimap<double, double>* vals, bool* all_internal)
{
	*all_internal = true;
	bool l_internal;
	for (GeometryPoint* p : points.points()) {
		double pos = line.calcPosition(p->x(), p->y(), &l_internal);
		vals->insert(std::make_pair(pos, p->z()));
		*all_internal = *all_internal && l_internal;
	}
}

void addToElevMap(std::map<CrossSection*, std::vector<double> >* elevVals, CrossSection* cs, double val)
{
	auto it = elevVals->find(cs);
	if (it == elevVals->end()) {
		std::vector<double> emptyVec;
		elevVals->insert(std::make_pair(cs, emptyVec));
		it = elevVals->find(cs);
	}
	it->second.push_back(val);
}

void updatePointsAutoSize(int numPoints, PointsGraphicsSetting* setting)
{
	if (numPoints < 30) {
		setting->autoSize = 7;
	}	else if (numPoints < 100) {
		setting->autoSize = 6;
	} else if (numPoints < 300) {
		setting->autoSize = 5;
	} else if (numPoints < 1000) {
		setting->autoSize = 4;
	} else if (numPoints < 3000) {
		setting->autoSize = 3;
	} else if (numPoints < 10000) {
		setting->autoSize = 2;
	} else {
		setting->autoSize	= 1;
	}
}

std::map<double, double>::const_iterator leftValue(const std::map<double, double>& vals, double pos)
{
	std::map<double, double>::const_iterator it = vals.begin();
	if (it->first > pos) {return vals.end();}

	while (1) {
		auto it2 = it;
		++ it2;
		if (it2 == vals.end()) {return it;}
		if (it2->first > pos) {return it;}

		++ it;
		if (it == vals.end()) {break;}
	}
	return vals.end();
}

std::map<double, double>::const_iterator rightValue(const std::map<double, double>& vals, double pos)
{
	std::map<double, double>::const_iterator it = vals.begin();

	while (1) {
		if (it->first > pos) {return it;}

		++ it;
		if (it == vals.end()) {break;}
	}
	return vals.end();
}

double interpolate(const std::map<double, double>& vals, double pos)
{
	auto left = leftValue(vals, pos);
	auto right = rightValue(vals, pos);

	if (left == vals.end()) {return Project::INVALID_HWM;}
	if (right == vals.end()) {return Project::INVALID_HWM;}

	double val = left->second +
			(right->second - left->second) / (right->first - left->first) * (pos - left->first);
	return val;
}

double calcHWMAtCrossSection(CrossSection* cs, BaseLine& bl, Points& hwm)
{
	bool crosses;
	double x, y, pos;

	bl.getCrossingPoint(cs, &crosses, &x, &y, &pos);

	if (! crosses) {return Project::INVALID_HWM;}

	std::map<double, double> vals;
	bool l_internal;
	for (GeometryPoint* p : hwm.points()) {
		double pos = bl.calcPosition(p->x(), p->y(), &l_internal);
		vals.insert(std::make_pair(pos, p->z()));
	}

	return interpolate(vals, pos);
}

int count(std::multimap<double, double>::const_iterator& b, std::multimap<double, double>::const_iterator& e)
{
	int ret = 0;
	auto it = b;
	while (it != e) {
		++ it;
		++ ret;
	}
	return ret;
}

} // namespace

Project::Impl::Impl(Project *project) :
	m_rootDataItem {project},
	m_elevationPoints {&m_rootDataItem},
	m_waterSurfaceElevationPoints {&m_rootDataItem},
	m_crossSections {&m_rootDataItem},
	m_baseLine {&m_rootDataItem},
	m_currentBuilder {& m_builderNearest},
	m_version {0},
	m_isModified {false}
{}

Project::Impl::~Impl()
{}

// public interfraces

const double Project::INVALID_HWM = -99999;

Project::Project() :
	impl {new Impl {this}}
{}

Project::~Project()
{
	delete impl;
}

bool Project::load(const QString& filename)
{
	QFile f2(filename);
	if (! f2.exists()) {return false;}

	QTemporaryDir dir;
	impl->m_tempDir = dir.path();

	iRIC::UnzipArchive(filename, dir.path());

	QDir d(dir.path());
	QFile f(d.absoluteFilePath(PROJECT_FILENAME));
	QDomDocument doc;
	doc.setContent(&f);

	rootDataItem()->loadFromMainFile(doc.documentElement());

	impl->m_filename = filename;
	impl->m_isModified = false;

	updatePointsAutoSize();
	mapPointsToCrossSections();

	return true;
}

bool Project::save(const QString& filename)
{
	QTemporaryDir dir;
	impl->m_tempDir = dir.path();

	QDir d(dir.path());
	QFile f(d.absoluteFilePath(PROJECT_FILENAME));

	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QXmlStreamWriter w(&f);
	w.setAutoFormatting(true);
	w.writeStartDocument("1.0");
	w.writeStartElement("RivMakerProject");

	rootDataItem()->saveToMainFile(&w);

	w.writeEndElement();
	w.writeEndDocument();
	f.close();

	QStringList files = impl->m_rootDataItem.containedFiles();
	files << PROJECT_FILENAME;

	QFile oldFile(filename);
	if (oldFile.exists()) {
		ok = oldFile.remove();
		if (! ok) {return false;}
	}

	bool ret = iRIC::ZipArchive(filename, dir.path(), files);
	impl->m_filename = filename;

	impl->m_isModified = false;

	return ret;
}

RootDataItem* Project::rootDataItem() const
{
	return &(impl->m_rootDataItem);
}

int Project::version() const
{
	return impl->m_version;
}

void Project::setVersion(int v)
{
	impl->m_version = v;
}

const ElevationPoints& Project::elevationPoints() const
{
	return impl->m_elevationPoints;
}

ElevationPoints& Project::elevationPoints()
{
	return impl->m_elevationPoints;
}

const WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints() const
{
	return impl->m_waterSurfaceElevationPoints;
}

WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints()
{
	return impl->m_waterSurfaceElevationPoints;
}

const BaseLine& Project::baseLine() const
{
	return impl->m_baseLine;
}

BaseLine& Project::baseLine()
{
	return impl->m_baseLine;
}

const CrossSections& Project::crossSections() const
{
	return impl->m_crossSections;
}

CrossSections& Project::crossSections()
{
	return impl->m_crossSections;
}

const QPointF& Project::offset() const
{
	return impl->m_offset;
}

void Project::setOffset(const QPointF& offset)
{
	impl->m_offset = offset;
}

const QString& Project::rivFileName() const
{
	return impl->m_rivFileName;
}

void Project::setRivFileName(const QString& rivFileName)
{
	impl->m_rivFileName = rivFileName;
}

bool Project::isCsvFileNameSet() const
{
	return ! impl->m_csvFileName.isEmpty();
}

const QString& Project::csvFileName() const
{
	return impl->m_csvFileName;
}

void Project::setCsvFileName(const QString& csvFileName)
{
	impl->m_csvFileName = csvFileName;

	emit csvFileNameSet(isCsvFileNameSet());
}

const QString& Project::rivCsvFolderName() const
{
	return impl->m_rivCsvFolderName;
}

void Project::setRivCsvFolderName(const QString& folderName)
{
	impl->m_rivCsvFolderName = folderName;
}

void Project::updatePointsAutoSize()
{
	int numElevationPoints = static_cast<int>(impl->m_elevationPoints.points().size());
	::updatePointsAutoSize(numElevationPoints, &(PointsGraphicsSetting::elevationPointsSetting));

	int numWaterElevationPoints = 0;
	numWaterElevationPoints += static_cast<int>(impl->m_waterSurfaceElevationPoints.leftBankHWM().points().size());
	numWaterElevationPoints += static_cast<int>(impl->m_waterSurfaceElevationPoints.rightBankHWM().points().size());
	numWaterElevationPoints += static_cast<int>(impl->m_waterSurfaceElevationPoints.arbitraryHWM().points().size());

	::updatePointsAutoSize(numWaterElevationPoints, &(PointsGraphicsSetting::waterElevationPointsSetting));
}

Project::MappingMethod Project::mappingMethod() const
{
	if (impl->m_currentBuilder == &(impl->m_builderNearest)) {
		return MappingMethod::AllMapToNearestCrossSection;
	} else if (impl->m_currentBuilder == &(impl->m_builderTin)) {
		return MappingMethod::TIN;
	} else {
		return MappingMethod::Template;
	}
}

void Project::setMappingMethod(MappingMethod method)
{
	if (method == MappingMethod::AllMapToNearestCrossSection) {
		impl->m_currentBuilder = &(impl->m_builderNearest);
	} else if (method == MappingMethod::TIN) {
		impl->m_currentBuilder = &(impl->m_builderTin);
	} else {
		impl->m_currentBuilder = &(impl->m_builderTemplate);
	}
}

double Project::templateMappingResolution() const
{
	return impl->m_builderTemplate.mappingResolution();
}

void Project::setTemplateMappingResolution(double resolution)
{
	impl->m_builderTemplate.setMappingResolution(resolution);
}

double Project::templateMappingStreamWiseLength() const
{
	return impl->m_builderTemplate.streamWiseLength();
}

void Project::setTemplateMappingStreamWiseLength(double len)
{
	impl->m_builderTemplate.setStreamWiseLength(len);
}

double Project::templateMappingCrossStreamWidth() const
{
	return impl->m_builderTemplate.crossStreamWidth();
}

void Project::setTemplateMappingCrossStreamWidth(double width)
{
	impl->m_builderTemplate.setCrossStreamWidth(width);
}

int Project::templateMappingNumberOfExpansions() const
{
	return impl->m_builderTemplate.numberOfExpansions();
}

void Project::setTemplateMappingNumberOfExpansions(int num)
{
	impl->m_builderTemplate.setNumberOfExpansions(num);
}

double Project::templateMappingWeightExponent() const
{
	return impl->m_builderTemplate.weightExponent();
}

void Project::setTemplateMappingWeightExponent(double exp)
{
	impl->m_builderTemplate.setWeightExponent(exp);
}

double Project::calcLeftBankHWMAtCrossSection(CrossSection* cs)
{
	return calcHWMAtCrossSection(cs, baseLine(), waterSurfaceElevationPoints().leftBankHWM());
}

double Project::calcRightBankHWMAtCrossSection(CrossSection* cs)
{
	return calcHWMAtCrossSection(cs, baseLine(), waterSurfaceElevationPoints().rightBankHWM());
}

void Project::calcCrossSectionElevations(bool* all_internal)
{
	auto& bl = baseLine();
	auto csVec = crossSections().crossSectionVector();

	if (bl.polyLine().size() < 2) {return;}

	std::multimap<double, CrossSection*> posMap;
	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		bl.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (crosses) {
			posMap.insert(std::make_pair(pos, cs));
		}
	}

	std::multimap<double, double> vals;

	auto& wse = waterSurfaceElevationPoints();

	*all_internal = true;
	bool l_internal;

	addToVector(wse.leftBankHWM(), bl, &vals, &l_internal);
	*all_internal = *all_internal && l_internal;
	addToVector(wse.rightBankHWM(), bl, &vals, &l_internal);
	*all_internal = *all_internal && l_internal;
	addToVector(wse.arbitraryHWM(), bl, &vals, &l_internal);
	*all_internal = *all_internal && l_internal;

	std::map<CrossSection*, std::vector<double> > elevVals;

	for (auto it = posMap.begin(); it != posMap.end(); ++it) {
		auto it_b = vals.begin();
		auto it_e = vals.end();

		// setup it_b
		if (it != posMap.begin()) {
			auto it2 = it;
			-- it2;
			it_b = vals.lower_bound(it2->first);
		}

		// setup it_e
		auto it2 = it;
		++ it2;
		if (it2 != posMap.end()) {
			it_e = vals.upper_bound(it2->first);
		}
		if (count(it_b, it_e) < 2) {
			if (it_b == vals.begin()) {
				while (count(it_b, it_e) < 2 && it_e != vals.end()) {
					++ it_e;
				}
			} else if (it_e == vals.end()) {
				while (count(it_b, it_e) < 2 && it_b != vals.begin()) {
					-- it_b;
				}
			} else {
				-- it_b;
				++ it_e;
			}
		}
		std::vector<double> xvec, yvec;
		for (auto tmp_it = it_b; tmp_it != it_e; ++ tmp_it) {
			xvec.push_back(tmp_it->first);
			yvec.push_back(tmp_it->second);
		}
		double a, b;
		if (xvec.size() > 1) {
			MathUtil::leastSquares(xvec, yvec, &a, &b);
			addToElevMap(&elevVals, it->second,  a * it->first + b);
		} else {
			// not enough points for interpolation
			QMessageBox::warning(nullptr, tr("Warning"), tr("Calculating initial WSE for Cross-section %1 failed").arg(it->second->name()));
		}
	}

	for (auto pair : elevVals) {
		if (pair.second.size() == 0) {continue;}
		if (pair.first->waterElevationIsSet()) {continue;}

		double sum = 0;
		for (double v : pair.second) {
			sum += v;
		}
		double avg = sum / pair.second.size();
		pair.first->setWaterElevation(avg);
		pair.first->setWaterElevationIsSet(true);
	}
}

void Project::mapPointsToCrossSections()
{
	if (crossSections().crossSectionVector().size() == 0) {return;}

	const auto& csVec = crossSections().crossSectionVector();
	for (auto cs : csVec) {
		cs->clearMappedPoints();
	}
	impl->m_currentBuilder->build(elevationPoints(), &(crossSections()));
}

bool Project::sortCrossSectionsIfPossible()
{
	auto& bl = baseLine();
	auto csVec = crossSections().crossSectionVector();

	if (bl.polyLine().size() < 2) {return false;}

	std::multimap<double, CrossSection*> posMap;
	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		bl.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {return false;}

		posMap.insert(std::make_pair(- pos, cs));
	}

	auto& childItems = crossSections().childItems();
	childItems.clear();
	for (auto pair : posMap) {
		childItems.push_back(pair.second);
	}
	return true;
}

QString Project::filename() const
{
	return impl->m_filename;
}

QString Project::tempDir() const
{
	return impl->m_tempDir;
}

bool Project::isModified() const
{
	return impl->m_isModified;
}

void Project::setModified()
{
	impl->m_isModified = true;
}

void Project::emitUpdated()
{
	emit updated();
}
