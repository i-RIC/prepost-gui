#ifndef GEODATANETCDFCOLORSETTINGDIALOG_H
#define GEODATANETCDFCOLORSETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataNetcdfColorSettingDialog;
}

class GeoDataNetcdfColorSettingDialog : public QDialog
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

	GeoDataNetcdfColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataNetcdfColorSettingDialog();

	void setIsReferenceInformation(bool isRef);

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataNetcdfColorSettingDialog* ui;
};

#endif // GEODATANETCDFCOLORSETTINGDIALOG_H
