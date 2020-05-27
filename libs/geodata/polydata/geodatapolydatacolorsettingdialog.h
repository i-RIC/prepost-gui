#ifndef GEODATAPOLYDATACOLORSETTINGDIALOG_H
#define GEODATAPOLYDATACOLORSETTINGDIALOG_H

#include "gd_polydata_global.h"

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPolyDataColorSettingDialog;
}

class GD_POLYDATA_EXPORT GeoDataPolyDataColorSettingDialog : public QDialog
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
		OpacityContainer opacity;
		EnumContainerT<Mapping> mapping;
	};

	GeoDataPolyDataColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataPolyDataColorSettingDialog();

	void setIsReferenceInformation(bool isRef);

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPolyDataColorSettingDialog* ui;
};

#endif // GEODATAPOLYDATACOLORSETTINGDIALOG_H
