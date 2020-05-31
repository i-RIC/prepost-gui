#ifndef GEODATAPOLYDATAGROUPCOLORSETTINGDIALOG_H
#define GEODATAPOLYDATAGROUPCOLORSETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPolyDataGroupColorSettingDialog;
}

class GeoDataPolyDataGroupColorSettingDialog : public QDialog
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

	GeoDataPolyDataGroupColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataPolyDataGroupColorSettingDialog();

	void setIsReferenceInformation(bool isRef);

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPolyDataGroupColorSettingDialog* ui;
};

#endif // GEODATAPOLYDATAGROUPCOLORSETTINGDIALOG_H
