#ifndef POST3DWINDOWCELLRANGESETTINGEDITWIDGET_H
#define POST3DWINDOWCELLRANGESETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class Post3dWindowCellRangeSettingWidget;
}

class PostZoneDataContainer;
class Post3dWindowCellRangeSettingContainer;

class Post3dWindowCellRangeSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Post3dWindowCellRangeSettingEditWidget(QWidget *parent = nullptr);
	~Post3dWindowCellRangeSettingEditWidget();

	void setZoneData(PostZoneDataContainer* zd);
	void setDimensions(int* dims);

	Post3dWindowCellRangeSettingContainer setting() const;
	void setSetting(const Post3dWindowCellRangeSettingContainer& setting);

private slots:
	void iMinChanged(int imin);
	void iMaxChanged(int imax);
	void jMinChanged(int jmin);
	void jMaxChanged(int jmax);
	void kMinChanged(int kmin);
	void kMaxChanged(int kmax);

private:
	Ui::Post3dWindowCellRangeSettingWidget *ui;
};

#endif // POST3DWINDOWCELLRANGESETTINGEDITWIDGET_H
