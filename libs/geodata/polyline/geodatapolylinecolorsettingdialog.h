#ifndef GEODATAPOLYLINECOLORSETTINGDIALOG_H
#define GEODATAPOLYLINECOLORSETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPolyLineColorSettingDialog;
}

class GeoDataPolyLineColorSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum Mapping {Value, Arbitrary};

	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);
		Setting& operator=(const Setting& s);

		ColorContainer color;
		EnumContainerT<Mapping> mapping;
	};

	GeoDataPolyLineColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataPolyLineColorSettingDialog();

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPolyLineColorSettingDialog* ui;
};

#endif // GEODATAPOLYLINECOLORSETTINGDIALOG_H
