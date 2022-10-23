#ifndef IMAGESETTINGCONTAINER_UPDATECOMMAND_H
#define IMAGESETTINGCONTAINER_UPDATECOMMAND_H

#include "../imagesettingcontainer.h"

#include <QUndoCommand>

class ImageSettingContainer::UpdateCommand : public QUndoCommand
{
public:
	UpdateCommand(ImageSettingContainer* setting, VTKGraphicsView* v);

	void redo() override;
	void undo() override;

protected:
	ImageSettingContainer m_newSetting;
	ImageSettingContainer m_oldSetting;
	ImageSettingContainer* m_setting;

	VTKGraphicsView* m_view;
};

#endif // IMAGESETTINGCONTAINER_UPDATECOMMAND_H
