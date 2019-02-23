#include "gridcreatingconditionlaplace_edgesetinterpolationmodecommand.h"

GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::EdgeSetInterpolationModeCommand(Impl::EdgeType edgeType, int edgeId, Impl::InterpolationType type, GridCreatingConditionLaplace::Impl* impl) :
	m_newType {type},
	m_edgeType {edgeType},
	m_edgeId {edgeId},
	m_impl {impl}
{
	m_oldType = targetType();
}

void GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::redo()
{
	targetType() = m_newType;
}

void GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::undo()
{
	targetType() = m_oldType;
}

GridCreatingConditionLaplace::Impl::InterpolationType& GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::targetType()
{
	if (m_edgeType == Impl::EdgeType::StreamWise) {
		return m_impl->m_edgeInterpolationStreamWise[m_edgeId];
	} else {
		return m_impl->m_edgeInterpolationCrossSection[m_edgeId];
	}
}
