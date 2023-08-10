#include "postbaseiterativevaluescontainer.h"
#include "postsolutioninfo.h"
#include "private/postbaseiterativevaluescontainer_basecontainer.h"
#include "private/postbaseiterativevaluescontainer_binaryio.h"
#include "private/postbaseiterativevaluescontainer_impl.h"
#include "private/postbaseiterativevaluescontainer_integervaluecontainer.h"
#include "private/postbaseiterativevaluescontainer_io.h"
#include "private/postbaseiterativevaluescontainer_realvaluecontainer.h"
#include "private/postbaseiterativevaluescontainer_textio.h"
#include "../project/projectdata.h"
#include "../project/projectmainfile.h"

#include <misc/stringtool.h>

#include <QFile>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <h5cgnsfileseparatesolutionutil.h>

#include <stdexcept>

PostBaseIterativeValuesContainer::PostBaseIterativeValuesContainer(ProjectData* projectData) :
	impl {new Impl {}}
{
	impl->m_projectData = projectData;
	impl->m_io = new TextIO(this);
}

PostBaseIterativeValuesContainer::~PostBaseIterativeValuesContainer()
{
	impl->m_io->save();

	clear();
	delete impl;
}

QString PostBaseIterativeValuesContainer::filename() const
{
	return impl->m_io->filename();
}

void PostBaseIterativeValuesContainer::load()
{
	impl->m_io->load();
}

bool PostBaseIterativeValuesContainer::save()
{
	return impl->m_io->save();
}

void PostBaseIterativeValuesContainer::updateIfNeeded(int *progress, int *invalidId)
{
	*progress = 0;
	*invalidId = -1;

	int cCount = resultCgnsCount();
	int vCount = valueCount();

	if (vCount > cCount) {
		rebuild(progress, invalidId);
		return;
	}

	if (vCount < cCount) {
		if (vCount == 0) {
			setupContainers();
		}
		for (int solId = vCount; solId < cCount; ++solId) {
			bool ok = addValuesFor(solId);
			if (! ok) {
				*invalidId = solId;

				QDir workDir(impl->m_projectData->workDirectory());
				auto resultFolder = iRIC::toStr(workDir.absoluteFilePath("result"));
				auto invalidCgnsName = iRICLib::H5CgnsFileSeparateSolutionUtil::fileNameForSolution(resultFolder, solId + 1); // solId starts from 0
				QFile::remove(invalidCgnsName.c_str());

				return;
			}
			*progress = static_cast<int>(100.0 * (solId + 1 - vCount) / (cCount - vCount));
		}
	}
}

const std::vector<PostBaseIterativeValuesContainer::BaseContainer*>& PostBaseIterativeValuesContainer::baseContainers() const
{
	return impl->m_baseContainers;
}

std::vector<PostBaseIterativeValuesContainer::BaseContainer*>& PostBaseIterativeValuesContainer::baseContainers()
{
	return impl->m_baseContainers;
}

PostBaseIterativeValuesContainer::BaseContainer* PostBaseIterativeValuesContainer::baseContainer(int baseId) const
{
	for (auto c : impl->m_baseContainers) {
		if (c->baseId() == baseId) {
			return c;
		}
	}
	return nullptr;
}

QString PostBaseIterativeValuesContainer::filename(const QString workdir)
{
	return IO::filename(workdir);
}

void PostBaseIterativeValuesContainer::rebuild(int* progress, int *invalidId)
{
	setupContainers();
	int cCount = resultCgnsCount();
	*progress = 0;
	*invalidId = -1;
	for (int solId = 0; solId < cCount; ++solId) {
		bool ok = addValuesFor(solId);
		if (! ok) {
			*invalidId = solId;
			return;
		}
		*progress = static_cast<int>(100.0 * (solId + 1) / cCount);
	}
}

void PostBaseIterativeValuesContainer::setupContainers()
{
	impl->m_baseContainers.clear();

	QDir workDir(impl->m_projectData->workDirectory());
	auto resultFolder = iRIC::toStr(workDir.absoluteFilePath("result"));
	auto cgnsName = iRICLib::H5CgnsFileSeparateSolutionUtil::fileNameForSolution(resultFolder, 1);
	QFile f(cgnsName.c_str());
	if (! f.exists()) {return;}

	try {
		iRICLib::H5CgnsFile cgnsFile(cgnsName, iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		for (int baseId = 1; baseId <= cgnsFile.baseNum(); ++baseId) {
			auto base = cgnsFile.baseById(baseId);
			auto bIter = base->biterData();
			if (bIter == nullptr) {continue;}

			auto c = new BaseContainer(baseId);
			c->setupContainers(bIter);
			impl->m_baseContainers.push_back(c);
		}
	} catch (...) {
		// do nothing
	}
}

bool PostBaseIterativeValuesContainer::addValuesFor(int solId)
{
	QDir workDir(impl->m_projectData->workDirectory());
	auto resultFolder = iRIC::toStr(workDir.absoluteFilePath("result"));
	auto cgnsName = iRICLib::H5CgnsFileSeparateSolutionUtil::fileNameForSolution(resultFolder, solId + 1); // solId starts from 0
	QFile f(cgnsName.c_str());
	if (! f.exists()) {return false;}

	try {
		iRICLib::H5CgnsFile cgnsFile(cgnsName, iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		for (auto c : impl->m_baseContainers) {
			auto base = cgnsFile.baseById(c->baseId());
			c->addValues(base->biterData());
		}
		return true;
	} catch (...) {
		return false;
	}
}

void PostBaseIterativeValuesContainer::clear()
{
	for (auto c : impl->m_baseContainers) {
		delete c;
	}
	impl->m_baseContainers.clear();
}

int PostBaseIterativeValuesContainer::resultCgnsCount() const
{
	int solId = 0;
	QDir workDir(impl->m_projectData->workDirectory());
	auto resultFolder = iRIC::toStr(workDir.absoluteFilePath("result"));

	while (true) {
		auto fName = iRICLib::H5CgnsFileSeparateSolutionUtil::fileNameForSolution(resultFolder, solId + 1); // solId starts from 0
		QFile f(fName.c_str());
		if (! f.exists()) {
			return solId;
		}
		++ solId;
	}
}

int PostBaseIterativeValuesContainer::valueCount() const
{
	for (const auto& c : impl->m_baseContainers) {
		if (c->integerContainers().size() > 0) {
			auto ic = c->integerContainers().at(0);
			return static_cast<int>(ic->values().size());
		}
		if (c->realContainers().size() > 0) {
			auto rc = c->realContainers().at(0);
			return static_cast<int>(rc->values().size());
		}
	}
	return 0;
}
