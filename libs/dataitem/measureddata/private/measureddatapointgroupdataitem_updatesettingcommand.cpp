#include "measureddatapointgroupdataitem_impl.h"
#include "measureddatapointgroupdataitem_updatesettingcommand.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

MeasuredDataPointGroupDataItem::UpdateSettingCommand::UpdateSettingCommand(bool allowMerge, const Setting& s, ColorMapSettingContainer* cs, MeasuredDataPointGroupDataItem* item) :
	QUndoCommand {},
	m_settingCommand {new ValueModifyCommmand<Setting>("Modify Setting", s, &item->impl->m_setting)},
	m_colormapCommand {nullptr},
	m_allowMerge {allowMerge},
	m_item {item}
{
	if (cs != nullptr) {
		auto oldcs = dynamic_cast<ColorMapSettingContainer*> (item->colorMapSetting(iRIC::toStr(s.value)));
		m_colormapCommand = new ValueModifyCommmand<ColorMapSettingContainer> ("Modify Colormap", *cs, oldcs);
	}
}

void MeasuredDataPointGroupDataItem::UpdateSettingCommand::redo()
{
	m_settingCommand->redo();
	if (m_colormapCommand != nullptr) {
		m_colormapCommand->redo();
	}

	m_item->updateActorSetting();
}

void MeasuredDataPointGroupDataItem::UpdateSettingCommand::undo()
{
	m_settingCommand->undo();
	if (m_colormapCommand != nullptr) {
		m_colormapCommand->undo();
	}

	m_item->updateActorSetting();
}

int MeasuredDataPointGroupDataItem::UpdateSettingCommand::id() const
{
	return iRIC::generateCommandId("MeasuredDataPointGroupDataItem::UpdateSettingCommand");
}

bool MeasuredDataPointGroupDataItem::UpdateSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_allowMerge) {return false;}

	auto other2 = const_cast<QUndoCommand*> (other);
	auto c = dynamic_cast<UpdateSettingCommand*> (other2);
	if (c == nullptr) {return false;}

	m_settingCommand->mergeWith(c->m_settingCommand);

	if (m_colormapCommand == nullptr) {
		m_colormapCommand = c->m_colormapCommand;
		c->m_colormapCommand = nullptr;
	} else if (c->m_colormapCommand == nullptr) {
		m_colormapCommand->undo();
		delete m_colormapCommand;
		m_colormapCommand = nullptr;
	} else {
		bool ok = m_colormapCommand->mergeWith(c->m_colormapCommand);

		if (!ok) {
			m_colormapCommand->undo();
			delete m_colormapCommand;
			m_colormapCommand = c->m_colormapCommand;
			c->m_colormapCommand = nullptr;
		}
	}
	m_allowMerge = c->m_allowMerge;
	return true;
}
