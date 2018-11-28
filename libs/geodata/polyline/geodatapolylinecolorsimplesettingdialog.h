#ifndef GEODATAPOLYLINECOLORSIMPLESETTINGDIALOG_H
#define GEODATAPOLYLINECOLORSIMPLESETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPolyLineColorSimpleSettingDialog;
}

class GeoDataPolyLineColorSimpleSettingDialog : public QDialog
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);
		Setting& operator=(const Setting& s);

		ColorContainer color;
	};

	GeoDataPolyLineColorSimpleSettingDialog(QWidget* parent = nullptr);
	~GeoDataPolyLineColorSimpleSettingDialog();

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPolyLineColorSimpleSettingDialog* ui;
};

#endif // GEODATAPOLYLINECOLORSIMPLESETTINGDIALOG_H
