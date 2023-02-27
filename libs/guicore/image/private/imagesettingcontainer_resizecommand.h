#ifndef IMAGESETTINGCONTAINER_RESIZECOMMAND_H
#define IMAGESETTINGCONTAINER_RESIZECOMMAND_H

#include "../imagesettingcontainer.h"
#include "imagesettingcontainer_updatecommand.h"

class ImageSettingContainer::ResizeCommand : public UpdateCommand
{
public:
	ResizeCommand(bool keyDown, const QPoint& from, const QPoint& to, ImageSettingContainer* setting, VTKGraphicsView* v);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
};

#endif // IMAGESETTINGCONTAINER_RESIZECOMMAND_H
