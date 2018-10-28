#ifndef GEODATAPOLYGONCOLORSETTINGDIALOG_H
#define GEODATAPOLYGONCOLORSETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPolygonColorSettingDialog;
}

class GeoDataPolygonColorSettingDialog : public QDialog
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

	GeoDataPolygonColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataPolygonColorSettingDialog();

	void setIsReferenceInformation(bool isRef);

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPolygonColorSettingDialog* ui;
};

#endif // GEODATAPOLYGONCOLORSETTINGDIALOG_H
