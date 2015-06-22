#ifndef GEODATABACKGROUNDEDITDIALOG_H
#define GEODATABACKGROUNDEDITDIALOG_H

#include <QDialog>
#include <QVariant>
#include "geodatabackground.h"

class GridAttributeEditWidget;

namespace Ui
{
	class GeoDataBackgroundEditDialog;
}

class GeoDataBackgroundEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataBackgroundEditDialog(QWidget* parent = nullptr);
	~GeoDataBackgroundEditDialog();
	GeoDataBackground::Type type() const {return m_type;}
	void setType(GeoDataBackground::Type t) {m_type = t;}
	const QVariant& customValue() const {return m_customValue;}
	void setCustomValue(const QVariant& v) {
		m_customValue = v;
	}
	void setWidget(GridAttributeEditWidget* w);
	void setupDialog();
	void accept() override;

private slots:
	void updateCustomWidgetStatus();

private:
	Ui::GeoDataBackgroundEditDialog* ui;
	GeoDataBackground::Type m_type;
	QVariant m_customValue;
};

#endif // GEODATABACKGROUNDEDITDIALOG_H
