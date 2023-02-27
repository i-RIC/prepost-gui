#ifndef IMAGESETTINGCONTAINER_MOVECOMMAND_H
#define IMAGESETTINGCONTAINER_MOVECOMMAND_H

#include "../imagesettingcontainer.h"
#include "imagesettingcontainer_updatecommand.h"

class ImageSettingContainer::MoveCommand : public UpdateCommand
{
public:
	MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, ImageSettingContainer* setting, VTKGraphicsView* v);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
};

#endif // IMAGESETTINGCONTAINER_MOVECOMMAND_H
