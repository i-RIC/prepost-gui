#ifndef DISTANCEMEASURECOPYPROPERTYDIALOG_H
#define DISTANCEMEASURECOPYPROPERTYDIALOG_H

#include "distancemeasure_api.h"

#include <guibase/vtktool/vtklabel2dactor.h>

#include <QDialog>
class QVector2D;

namespace Ui
{
	class DistanceMeasureCopyPropertyDialog;
}

class DISTANCEMEASURE_API DistanceMeasureCopyPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	enum LabelMode {
		lmAuto,
		lmCustom
	};

	explicit DistanceMeasureCopyPropertyDialog(QWidget* parent = nullptr);
	~DistanceMeasureCopyPropertyDialog();

	void setName(const QString& name);
	void setPoints(const QVector2D& v1, const QVector2D& v2);
	void setShowLabel(bool show);
	void setLabelPosition(vtkLabel2DActor::LabelPosition pos);
	void setLabelMode(LabelMode lm);
	void setCustomlabel(const QString& label);
	void setLabelFontSize(int fontSize);
	void setShowMarkers(bool show);
	void setMarkerSize(int size);
	void setColor(QColor color);

	QString name() const;
	QVector2D point1() const;
	QVector2D point2() const;
	bool showLabel() const;
	vtkLabel2DActor::LabelPosition labelPosition() const;
	LabelMode labelMode() const;
	QString autoLabel() const;
	QString customLabel() const;
	int fontSize() const;
	bool showMarkers() const;
	int markerSize() const;
	QColor color() const;

private slots:
	void updateAutoLabel();

private:
	Ui::DistanceMeasureCopyPropertyDialog* ui;
};

#endif // DISTANCEMEASURECOPYPROPERTYDIALOG_H
