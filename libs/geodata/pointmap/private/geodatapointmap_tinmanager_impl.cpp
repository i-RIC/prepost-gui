#include "geodatapointmap_tinmanager_addbreaklinecontroller.h"
#include "geodatapointmap_tinmanager_deletebreaklinecontroller.h"
#include "geodatapointmap_tinmanager_impl.h"

#include <guibase/vtktool/vtkpointsetgeos2dindex.h>
#include <misc/mouseeventcontroller.h>

#include <vtkLODActor.h>
#include <vtkPolyData.h>

GeoDataPointmap::TINManager::Impl::Impl(TINManager* tinManager, GeoDataPointmap* parent) :
	m_tin {vtkPolyData::New()},
	m_tinActor {vtkLODActor::New()},
	m_index {nullptr},
	m_needRebuild {false},
	m_normalController {new MouseEventController {}},
	m_addBreakLineController {new AddBreakLineController {tinManager}},
	m_deleteBreakLineController {new DeleteBreakLineController {tinManager}},
	m_activeController {m_normalController},
	m_parent {parent}
{
	m_controllers.push_back(m_normalController);
	m_controllers.push_back(m_addBreakLineController);
	m_controllers.push_back(m_deleteBreakLineController);
}

GeoDataPointmap::TINManager::Impl::~Impl()
{
	m_tin->Delete();
	m_tinActor->Delete();
	delete m_index;

	delete m_normalController;
	delete m_addBreakLineController;
	delete m_deleteBreakLineController;
}
