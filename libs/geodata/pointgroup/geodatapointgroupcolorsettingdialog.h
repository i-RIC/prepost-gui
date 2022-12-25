#ifndef GEODATAPOINTGROUPCOLORSETTINGDIALOG_H
#define GEODATAPOINTGROUPCOLORSETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/intcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPointGroupColorSettingDialog;
}

class GeoDataPointGroupColorSettingDialog : public QDialog
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
		IntContainer pointSize;
	};

	GeoDataPointGroupColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataPointGroupColorSettingDialog();

	void setIsReferenceInformation(bool isRef);

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPointGroupColorSettingDialog* ui;
};

#endif // GEODATAPOINTGROUPCOLORSETTINGDIALOG_H
