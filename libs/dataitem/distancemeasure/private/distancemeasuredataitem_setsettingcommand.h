#ifndef DISTANCEMEASUREDATAITEM_SETSETTINGCOMMAND_H
#define DISTANCEMEASUREDATAITEM_SETSETTINGCOMMAND_H

#include "../distancemeasuredataitem.h"

#include <QUndoCommand>

class DistanceMeasureDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const QString& name, const QVector2D& v1, const QVector2D& v2, bool showLabel, vtkLabel2DActor::LabelPosition lp, DistanceMeasureCopyPropertyDialog::LabelMode lm, int fontSize, const QString& customLabel, bool showMarker, int markerSize, QColor color, DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

private:
	QString m_newName;
	QVector2D m_newPoint1;
	QVector2D m_newPoint2;
	bool m_newShowLabel;
	vtkLabel2DActor::LabelPosition m_newLabelPosition;
	DistanceMeasureCopyPropertyDialog::LabelMode m_newLabelMode;
	int m_newFontSize;
	QString m_newCustomLabel;
	bool m_newShowMarker;
	int m_newMarkerSize;
	QColor m_newColor;

	QString m_oldName;
	QVector2D m_oldPoint1;
	QVector2D m_oldPoint2;
	bool m_oldShowLabel;
	vtkLabel2DActor::LabelPosition m_oldLabelPosition;
	DistanceMeasureCopyPropertyDialog::LabelMode m_oldLabelMode;
	int m_oldFontSize;
	QString m_oldCustomLabel;
	bool m_oldShowMarker;
	int m_oldMarkerSize;
	QColor m_oldColor;

	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_SETSETTINGCOMMAND_H
