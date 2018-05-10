#include "gridcreatingconditioncompoundchannel_switchstatuscommand.h"
#include "../gridcreatingconditioncompoundchannelcenterline.h"
#include "../gridcreatingconditioncompoundchannelgridregionpolygon.h"
#include "../gridcreatingconditioncompoundchannellowwaterchannelpolygon.h"

#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/informationdialog.h>

GridCreatingConditionCompoundChannel::SwitchStatusCommand::SwitchStatusCommand(GridCreatingConditionCompoundChannel::Status newStatus, GridCreatingConditionCompoundChannel* condition) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Finish Defining Polygon or Polygonal line")},
	m_newStatus {newStatus},
	m_condition {condition}
{}

void GridCreatingConditionCompoundChannel::SwitchStatusCommand::redo()
{
	m_condition->m_status = m_newStatus;
	switch (m_newStatus) {
	case GridCreatingConditionCompoundChannel::stDefiningLowWaterRegion:
		m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meBeforeDefining;
		m_condition->m_selectedPolygon = m_condition->m_lowWaterChannelPolygon;
		m_condition->m_gridRegionPolygon->setActive(false);
		m_condition->m_lowWaterChannelPolygon->setActive(true);
		InformationDialog::information(m_condition->preProcessorWindow(), GridCreatingConditionCompoundChannel::tr("Information"), GridCreatingConditionCompoundChannel::tr("Next, please define low water channel region. Water channel can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gccompoundchannel_lowwater");
		break;
	case GridCreatingConditionCompoundChannel::stDefiningCenterLine:
		m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meBeforeDefining;
		m_condition->m_selectedLine = m_condition->m_centerLine;
		m_condition->m_lowWaterChannelPolygon->setActive(false);
		m_condition->m_centerLine->setActive(true);
		InformationDialog::information(m_condition->preProcessorWindow(), GridCreatingConditionCompoundChannel::tr("Information"), GridCreatingConditionCompoundChannel::tr("Next, please define grid center line. Grid center line can be defined as polygonal line by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gccompoundchannel_centerline");
		break;
	case GridCreatingConditionCompoundChannel::stNormal:
		m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meNormal;
		m_condition->m_selectMode = GridCreatingConditionCompoundChannel::smLine;
		m_condition->m_selectedLine = m_condition->m_centerLine;
		m_condition->m_centerLine->setActive(true);
		m_condition->create(m_condition->preProcessorWindow());
		break;
	default:
		break;
	}
	m_condition->updateMouseCursor(m_condition->graphicsView());
	m_condition->updateActionStatus();
	m_condition->renderGraphicsView();
}

void GridCreatingConditionCompoundChannel::SwitchStatusCommand::undo() {
	switch (m_newStatus) {
	case GridCreatingConditionCompoundChannel::stDefiningLowWaterRegion:
		m_condition->m_status = GridCreatingConditionCompoundChannel::stDefiningRegion;
		m_condition->m_selectedPolygon = m_condition->m_gridRegionPolygon;
		m_condition->m_gridRegionPolygon->setActive(true);
		m_condition->m_lowWaterChannelPolygon->setActive(false);
		break;
	case GridCreatingConditionCompoundChannel::stDefiningCenterLine:
		m_condition->m_status = GridCreatingConditionCompoundChannel::stDefiningLowWaterRegion;
		m_condition->m_selectedPolygon = m_condition->m_lowWaterChannelPolygon;
		m_condition->m_lowWaterChannelPolygon->setActive(true);
		m_condition->m_centerLine->setActive(false);
		break;
	case GridCreatingConditionCompoundChannel::stNormal:
		m_condition->m_status = GridCreatingConditionCompoundChannel::stDefiningCenterLine;
		m_condition->m_selectedLine = m_condition->m_centerLine;
		m_condition->m_centerLine->setActive(true);
		break;
	default:
		break;
	}
	m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meDefining;

	m_condition->updateMouseCursor(m_condition->graphicsView());
	m_condition->updateActionStatus();
}
