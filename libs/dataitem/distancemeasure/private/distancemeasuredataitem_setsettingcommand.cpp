#include "distancemeasuredataitem_setsettingcommand.h"

DistanceMeasureDataItem::SetSettingCommand::SetSettingCommand(const QString& name, const QVector2D& v1, const QVector2D& v2, bool showLabel, DistanceMeasureCopyPropertyDialog::LabelPosition lp, DistanceMeasureCopyPropertyDialog::LabelMode lm, int fontSize, const QString& customLabel, bool showMarker, int markerSize, QColor color, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Edit Distance Measure Property")),
	m_newName {name},
	m_newPoint1 {v1},
	m_newPoint2 {v2},
	m_newShowLabel {showLabel},
	m_newLabelPosition {lp},
	m_newLabelMode {lm},
	m_newFontSize {fontSize},
	m_newCustomLabel {customLabel},
	m_newShowMarker {showMarker},
	m_newMarkerSize {markerSize},
	m_newColor {color},
	m_oldName {item->standardItem()->text()},
	m_oldPoint1 {item->m_point1},
	m_oldPoint2 {item->m_point2},
	m_oldShowLabel {item->m_showLabel},
	m_oldLabelPosition {item->m_labelPosition},
	m_oldLabelMode {item->m_labelMode},
	m_oldFontSize {item->m_fontSize},
	m_oldCustomLabel {item->m_customLabel},
	m_oldShowMarker {item->m_showMarkers},
	m_oldMarkerSize {item->m_markerSize},
	m_oldColor {item->m_color},
	m_item {item}
{}

void DistanceMeasureDataItem::SetSettingCommand::redo()
{
	m_item->m_isCommandExecuting = true;
	m_item->standardItem()->setText(m_newName);
	m_item->m_point1 = m_newPoint1;
	m_item->m_point2 = m_newPoint2;
	m_item->m_showLabel = m_newShowLabel;
	m_item->m_labelPosition = m_newLabelPosition;
	m_item->m_labelMode = m_newLabelMode;
	m_item->m_fontSize = m_newFontSize;
	m_item->m_customLabel = m_newCustomLabel;
	m_item->m_showMarkers = m_newShowMarker;
	m_item->m_markerSize = m_newMarkerSize;
	m_item->m_color = m_newColor;
	m_item->m_isCommandExecuting = false;

	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}

void DistanceMeasureDataItem::SetSettingCommand::undo()
{
	m_item->m_isCommandExecuting = true;
	m_item->standardItem()->setText(m_oldName);
	m_item->m_point1 = m_oldPoint1;
	m_item->m_point2 = m_oldPoint2;
	m_item->m_showLabel = m_oldShowLabel;
	m_item->m_labelPosition = m_oldLabelPosition;
	m_item->m_labelMode = m_oldLabelMode;
	m_item->m_fontSize = m_oldFontSize;
	m_item->m_customLabel = m_oldCustomLabel;
	m_item->m_showMarkers = m_oldShowMarker;
	m_item->m_markerSize = m_oldMarkerSize;
	m_item->m_color = m_oldColor;
	m_item->m_isCommandExecuting = false;

	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}
