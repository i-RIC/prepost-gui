#ifndef POST3DWINDOWPARTICLESSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWPARTICLESSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowparticlesscalargroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowParticlesScalarGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ScalarSettingContainer& setting, const LookupTableContainer& ltc, int size, const QColor& color, const QString& colorbarTitle, Post3dWindowParticlesScalarGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	void applySetting(const ScalarSettingContainer& setting, const LookupTableContainer& lut, int size, const QColor& color, const QString& title);

	ScalarSettingContainer m_newSetting;
	LookupTableContainer m_newLookupTable;
	int m_newSize;
	QColor m_newColor;
	QString m_newScalarBarTitle;

	ScalarSettingContainer m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	int m_oldSize;
	QColor m_oldColor;
	QString m_oldScalarBarTitle;

	Post3dWindowParticlesScalarGroupDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
