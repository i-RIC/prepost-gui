#ifndef GEODATAPOLYDATAGROUPCOPYSETTINGDIALOG_H
#define GEODATAPOLYDATAGROUPCOPYSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataPolyDataGroupCopySettingDialog;
}

class GeoDataPolyDataGroup;
class GridAttributeEditWidget;

class QStandardItemModel;

class GeoDataPolyDataGroupCopySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPolyDataGroupCopySettingDialog(GeoDataPolyDataGroup* orig, GeoDataPolyDataGroup* copy, QWidget *parent);
	~GeoDataPolyDataGroupCopySettingDialog();

public slots:
	void accept();

private:
	GeoDataPolyDataGroup* m_originalData;
	GeoDataPolyDataGroup* m_copyData;
	QStandardItemModel* m_model;

	GridAttributeEditWidget* m_allEditWidget;
	Ui::GeoDataPolyDataGroupCopySettingDialog *ui;
};

#endif // GEODATAPOLYDATAGROUPCOPYSETTINGDIALOG_H
