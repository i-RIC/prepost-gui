#include "imagesettingcontainer_updatecommand.h"

#include <guicore/datamodel/vtkgraphicsview.h>

ImageSettingContainer::UpdateCommand::UpdateCommand(ImageSettingContainer* setting, VTKGraphicsView* v) :
	m_newSetting {*setting},
	m_oldSetting {*setting},
	m_setting {setting},
	m_view {v}
{}

void ImageSettingContainer::UpdateCommand::redo()
{
	*m_setting = m_newSetting;
	m_setting->apply(m_view->size(), m_view);
	emit m_setting->updated();
}

void ImageSettingContainer::UpdateCommand::undo()
{
	*m_setting = m_oldSetting;
	m_setting->apply(m_view->size(), m_view);
	emit m_setting->updated();
}
