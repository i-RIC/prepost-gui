#ifndef GEODATAGDALCOLORSETTINGDIALOG_H
#define GEODATAGDALCOLORSETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataGdalColorSettingDialog;
}

class GeoDataGdalColorSettingDialog : public QDialog
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

	GeoDataGdalColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataGdalColorSettingDialog();

	void setIsReferenceInformation(bool isRef);

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataGdalColorSettingDialog* ui;
};

#endif // GEODATAGDALCOLORSETTINGDIALOG_H
