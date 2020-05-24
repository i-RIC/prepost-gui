#ifndef GEODATAPOINTCOLORSIMPLESETTINGDIALOG_H
#define GEODATAPOINTCOLORSIMPLESETTINGDIALOG_H

#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>

#include <QDialog>

namespace Ui
{
	class GeoDataPointColorSimpleSettingDialog;
}

class GeoDataPointColorSimpleSettingDialog : public QDialog
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

	GeoDataPointColorSimpleSettingDialog(QWidget* parent = nullptr);
	~GeoDataPointColorSimpleSettingDialog();

	void setSetting(const Setting &setting);
	Setting setting() const;

private:
	Ui::GeoDataPointColorSimpleSettingDialog* ui;
};

#endif // GEODATAPOINTCOLORSIMPLESETTINGDIALOG_H
