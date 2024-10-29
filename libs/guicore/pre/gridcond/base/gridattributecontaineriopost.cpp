#include "gridattributecontainer.h"
#include "gridattributecontaineriopost.h"
#include "../../../grid/v4grid.h"
#include "../../../postcontainer/postsolutioninfo.h"

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <h5cgnsgridattributes.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

GridAttributeContainerIoPost::GridAttributeContainerIoPost(int dim, const std::string& zoneName, PostSolutionInfo* info) :
	m_dim {dim},
	m_zoneName {zoneName},
	m_solutionInfo {info}
{}

int GridAttributeContainerIoPost::loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts, GridAttributeContainer* cont)
{
	int ier = cont->loadFromCgnsFileForIndex(atts, 0);
	if (ier == IRIC_NO_ERROR) {
		cont->setMapped(true);
	}
	return ier;
}

int GridAttributeContainerIoPost::saveToCgnsFile(iRICLib::H5CgnsGridAttributes* /*atts*/, GridAttributeContainer* /*cont*/)
{
	return IRIC_NO_ERROR;
}

void GridAttributeContainerIoPost::clearTemporaryData(GridAttributeContainer* cont)
{}

void GridAttributeContainerIoPost::handleDimensionCurrentIndexChange(int /*oldIndex*/, int newIndex, GridAttributeContainer* cont)
{
	auto atts = m_solutionInfo->cgnsFile()->base(m_dim)->zone(m_zoneName)->gridAttributes();
	cont->loadFromCgnsFileForIndex(*atts, newIndex);

	emit cont->grid()->grid()->changed();
}
