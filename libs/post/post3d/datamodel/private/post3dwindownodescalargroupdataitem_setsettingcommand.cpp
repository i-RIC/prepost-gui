#include "post3dwindownodescalargroupdataitem_setsettingcommand.h"

Post3dWindowNodeScalarGroupDataItem::SetSettingCommand::SetSettingCommand(
		bool enabled, const std::string& sol,
		bool fullrange, StructuredGridRegion::Range3d range,
		double isovalue, const QColor& color, const OpacityContainer& opacity, Post3dWindowNodeScalarGroupDataItem* item) :
	QUndoCommand(Post3dWindowNodeScalarGroupDataItem::tr("Update Contour Setting"))
{
	m_newEnabled = enabled;
	m_newCurrentSolution = sol;
	m_newFullRange = fullrange;
	m_newRange = range;
	m_newIsoValue = isovalue;
	m_newColor = color;
	m_newOpacity = opacity;

	m_oldEnabled = item->isEnabled();
	m_oldCurrentSolution = item->m_target;
	m_oldFullRange = item->m_fullRange;
	m_oldRange = item->m_range;
	m_oldIsoValue = item->m_isoValue;
	m_oldColor = item->m_color;
	m_oldOpacity = item->m_opacity;

	m_item = item;
}

void Post3dWindowNodeScalarGroupDataItem::SetSettingCommand::redo()
{
	m_item->setIsCommandExecuting(true);
	m_item->setEnabled(m_newEnabled);
	m_item->setTarget(m_newCurrentSolution);
	m_item->m_fullRange = m_newFullRange;
	m_item->m_range = m_newRange;
	m_item->m_isoValue = m_newIsoValue;
	m_item->m_color = m_newColor;
	m_item->m_opacity = m_newOpacity;

	m_item->updateActorSettings();
	m_item->renderGraphicsView();
	m_item->setIsCommandExecuting(false);
}

void Post3dWindowNodeScalarGroupDataItem::SetSettingCommand::undo()
{
	m_item->setIsCommandExecuting(true);
	m_item->setEnabled(m_oldEnabled);
	m_item->setTarget(m_oldCurrentSolution);
	m_item->m_fullRange = m_oldFullRange;
	m_item->m_range = m_oldRange;
	m_item->m_isoValue = m_oldIsoValue;
	m_item->m_color = m_oldColor;
	m_item->m_opacity = m_oldOpacity;

	m_item->updateActorSettings();
	m_item->renderGraphicsView();
	m_item->setIsCommandExecuting(false);
}
