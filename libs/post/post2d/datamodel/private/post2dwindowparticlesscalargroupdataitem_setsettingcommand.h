#ifndef POST2DWINDOWPARTICLESSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWPARTICLESSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowparticlesscalargroupdataitem.h"

#include <QUndoCommand>

class Post2dWindowParticlesScalarGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ScalarSettingContainer& setting, const LookupTableContainer& ltc, int size, const QColor& color, const QString& colorbarTitle, Post2dWindowParticlesScalarGroupDataItem* item);

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

	Post2dWindowParticlesScalarGroupDataItem* m_item;
};

#endif // POST2DWINDOWPARTICLESSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
