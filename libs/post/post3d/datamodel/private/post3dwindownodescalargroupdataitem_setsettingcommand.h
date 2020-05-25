#ifndef POST3DWINDOWNODESCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWNODESCALARGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindownodescalargroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowNodeScalarGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(
			bool enabled, const std::string& sol,
			bool fullrange, StructuredGridRegion::Range3d range,
			double isovalue, const QColor& color, int opacity, Post3dWindowNodeScalarGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	bool m_newEnabled;
	std::string m_newCurrentSolution;
	bool m_newFullRange;
	StructuredGridRegion::Range3d m_newRange;
	double m_newIsoValue;
	QColor m_newColor;
	int m_newOpacity;

	bool m_oldEnabled;
	std::string m_oldCurrentSolution;
	bool m_oldFullRange;
	StructuredGridRegion::Range3d m_oldRange;
	double m_oldIsoValue;
	QColor m_oldColor;
	int m_oldOpacity;

	Post3dWindowNodeScalarGroupDataItem* m_item;
};

#endif // POST3DWINDOWNODESCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
