#ifndef DISTANCEMEASUREPROPERTYDIALOG_H
#define DISTANCEMEASUREPROPERTYDIALOG_H

#include "distancemeasure_api.h"
#include "distancemeasuresetting.h"

#include <QDialog>

class QVector2D;

namespace Ui
{
	class DistanceMeasurePropertyDialog;
}

class DISTANCEMEASURE_API DistanceMeasurePropertyDialog : public QDialog
{
	Q_OBJECT

public:

	explicit DistanceMeasurePropertyDialog(QWidget* parent = nullptr);
	~DistanceMeasurePropertyDialog();

	QString name() const;
	void setName(const QString& name);

	DistanceMeasureSetting setting() const;
	void setSetting(const DistanceMeasureSetting& setting);

private slots:
	void updateAutoLabel();

private:
	QString autoLabel() const;

	DistanceMeasureSetting m_setting;
	Ui::DistanceMeasurePropertyDialog* ui;
};

#endif // DISTANCEMEASUREPROPERTYDIALOG_H
