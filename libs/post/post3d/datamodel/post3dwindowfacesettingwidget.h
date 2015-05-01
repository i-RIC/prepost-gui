#ifndef POST3DWINDOWFACESETTINGWIDGET_H
#define POST3DWINDOWFACESETTINGWIDGET_H

#include <QWidget>
#include "post3dwindowfacedataitem.h"

namespace Ui
{
	class Post3dWindowFaceSettingWidget;
}

class PostZoneDataContainer;

class Post3dWindowFaceSettingWidget : public QWidget
{
	Q_OBJECT
public:
	explicit Post3dWindowFaceSettingWidget(QWidget* parent = nullptr);
	~Post3dWindowFaceSettingWidget();
	void setZoneData(PostZoneDataContainer* zd);
	void setSetting(const Post3dWindowFaceDataItem::Setting& setting);
	const Post3dWindowFaceDataItem::Setting& setting() const {
		return m_setting;
	}
	void sliderRange(int range[6]);
	void setEnabledCheck(bool check);
	void setMultiSelected(bool selected);
signals:
	void settingChanged();
private slots:
	void setMyEnabled(bool enabled);
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
	void applySliderValues();
	Ui::Post3dWindowFaceSettingWidget* ui;
	Post3dWindowFaceDataItem::Setting m_setting;
	bool m_multiSelected;
};

#endif // POST3DWINDOWFACESETTINGWIDGET_H
