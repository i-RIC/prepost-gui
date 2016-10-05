#include "backgroundimageinfo_setactorpropertycommand.h"

BackgroundImageInfo::SetActorPropertyCommand::SetActorPropertyCommand(double posx, double posy, double scale, double theta, BackgroundImageInfo* info) :
	QUndoCommand(QObject::tr("Reallocate Background Image")),
	m_newTranslateX {posx},
	m_newTranslateY {posy},
	m_newScale {scale},
	m_newTheta {theta},
	m_oldTranslateX {info->m_oldTranslateX},
	m_oldTranslateY {info->m_oldTranslateY},
	m_oldScale {info->m_oldScale},
	m_oldTheta {info->m_oldTheta},
	m_info {info}
{}

void BackgroundImageInfo::SetActorPropertyCommand::redo()
{
	applySetting(m_newTranslateX, m_newTranslateY, m_newScale, m_newTheta);
}

void BackgroundImageInfo::SetActorPropertyCommand::undo()
{
	applySetting(m_oldTranslateX, m_oldTranslateY, m_oldScale, m_oldTheta);
}

void BackgroundImageInfo::SetActorPropertyCommand::applySetting(double posx, double posy, double scale, double theta)
{
	m_info->m_translateX = posx;
	m_info->m_translateY = posy;
	m_info->m_scale = scale;
	m_info->m_angle = theta;

	m_info->informChange();
}
