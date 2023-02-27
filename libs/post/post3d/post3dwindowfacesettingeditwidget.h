#ifndef POST3DWINDOWFACESETTINGEDITWIDGET_H
#define POST3DWINDOWFACESETTINGEDITWIDGET_H

#include "post3dwindowfacesettingcontainer.h"

#include <QWidget>

namespace Ui {
class Post3dWindowFaceSettingEditWidget;
}

class PostZoneDataContainer;

class Post3dWindowFaceSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Post3dWindowFaceSettingEditWidget(QWidget *parent = nullptr);
	~Post3dWindowFaceSettingEditWidget();

	void setZoneData(PostZoneDataContainer* zd);
	void setDimensions(int* dims);

	Post3dWindowFaceSettingContainer setting() const;
	void setSetting(const Post3dWindowFaceSettingContainer& setting);

private slots:
	void iMinChanged(int imin);
	void iMaxChanged(int imax);
	void jMinChanged(int jmin);
	void jMaxChanged(int jmax);
	void kMinChanged(int kmin);
	void kMaxChanged(int kmax);

	void iDirClicked();
	void jDirClicked();
	void kDirClicked();

private:
	void applyDirectionSetting();
	void maximizeRange();

	Post3dWindowFaceSettingContainer::Direction direction() const;

	Ui::Post3dWindowFaceSettingEditWidget *ui;
};

#endif // POST3DWINDOWFACESETTINGEDITWIDGET_H
