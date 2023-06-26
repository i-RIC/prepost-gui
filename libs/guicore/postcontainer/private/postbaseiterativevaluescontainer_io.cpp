#include "postbaseiterativevaluescontainer_impl.h"
#include "postbaseiterativevaluescontainer_io.h"
#include "../postsolutioninfo.h"
#include "../../project/projectdata.h"

#include <QDir>

namespace {

const QString FILENAME = "output.dat";

} // namespace

PostBaseIterativeValuesContainer::IO::IO(PostBaseIterativeValuesContainer* container) :
	m_container {container}
{
	m_file.setFileName(filename(container->impl->m_projectData->workDirectory()));
}

PostBaseIterativeValuesContainer::IO::~IO()
{}

QString PostBaseIterativeValuesContainer::IO::filename() const
{
	return FILENAME;
}

QString PostBaseIterativeValuesContainer::IO::filename(const QString& workdir)
{
	QDir dir(workdir);
	return dir.absoluteFilePath(FILENAME);
}
