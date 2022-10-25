#ifndef BACKGROUNDIMAGEINFO_SETACTORPROPERTYCOMMAND_H
#define BACKGROUNDIMAGEINFO_SETACTORPROPERTYCOMMAND_H

#include "../backgroundimageinfo.h"
#include "backgroundimageinfo_setting.h"

#include <QUndoCommand>

class BackgroundImageInfo::SetActorPropertyCommand : public QUndoCommand
{
public:
	SetActorPropertyCommand(const BackgroundImageInfo::Setting& newSetting, const BackgroundImageInfo::Setting& oldSetting, BackgroundImageInfo* info);

	void redo() override;
	void undo() override;

private:
	void applySetting(const BackgroundImageInfo::Setting& setting);

	BackgroundImageInfo::Setting m_newSetting;
	BackgroundImageInfo::Setting m_oldSetting;

	BackgroundImageInfo* m_info;
};

#endif // BACKGROUNDIMAGEINFO_SETACTORPROPERTYCOMMAND_H
