#ifndef GEODATAPOLYGONGROUPCOLORSETTINGDIALOG_H
#define GEODATAPOLYGONGROUPCOLORSETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPolygonGroupColorSettingDialog;
}

class GeoDataPolygonGroupColorSettingDialog : public QDialog
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

	GeoDataPolygonGroupColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataPolygonGroupColorSettingDialog();

	void setIsReferenceInformation(bool isRef);

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPolygonGroupColorSettingDialog* ui;
};

#endif // GEODATAPOLYGONGROUPCOLORSETTINGDIALOG_H
