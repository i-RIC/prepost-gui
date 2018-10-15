#ifndef DISTANCEMEASURECOPYPROPERTYDIALOG_H
#define DISTANCEMEASURECOPYPROPERTYDIALOG_H

#include "distancemeasure_api.h"
#include "distancemeasuresetting.h"

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

	explicit DistanceMeasureCopyPropertyDialog(QWidget* parent = nullptr);
	~DistanceMeasureCopyPropertyDialog();

	QString name() const;
	void setName(const QString& name);

	DistanceMeasureSetting setting() const;
	void setSetting(const DistanceMeasureSetting& setting);

private slots:
	void updateAutoLabel();

private:
	QString autoLabel() const;

	DistanceMeasureSetting m_setting;
	Ui::DistanceMeasureCopyPropertyDialog* ui;
};

#endif // DISTANCEMEASURECOPYPROPERTYDIALOG_H
