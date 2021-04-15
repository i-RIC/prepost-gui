#ifndef POST3DWINDOWCELLRANGESETTINGWIDGET_H
#define POST3DWINDOWCELLRANGESETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class Post3dWindowCellRangeSettingWidget;
}

class PostZoneDataContainer;
class Post3dCellRangeSettingContainer;

class Post3dWindowCellRangeSettingWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Post3dWindowCellRangeSettingWidget(QWidget *parent = nullptr);
	~Post3dWindowCellRangeSettingWidget();

	void setZoneData(PostZoneDataContainer* zd);

	void setSetting(const Post3dCellRangeSettingContainer& setting);
	Post3dCellRangeSettingContainer setting() const;

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

#endif // POST3DWINDOWCELLRANGESETTINGWIDGET_H
