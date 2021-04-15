#ifndef POST3DWINDOWNODEGROUPARROWFACESETTINGWIDGET_H
#define POST3DWINDOWNODEGROUPARROWFACESETTINGWIDGET_H

#include "post3dfacesettingcontainer.h"

#include <QWidget>

#include <string>
#include <vector>

namespace Ui {
class Post3dWindowNodeGroupArrowFaceSettingWidget;
}

class ArrowSettingContainer;
class Post3dFaceSettingContainer;
class Post3dSamplingRateSettingContainer;
class PostZoneDataContainer;

class Post3dWindowNodeGroupArrowFaceSettingWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Post3dWindowNodeGroupArrowFaceSettingWidget(QWidget *parent = nullptr);
	~Post3dWindowNodeGroupArrowFaceSettingWidget();

	void setZoneData(PostZoneDataContainer* zd);

	void setSettings(bool enabled, const ArrowSettingContainer& arrowSetting, const Post3dFaceSettingContainer& faceSetting, const Post3dSamplingRateSettingContainer& samplingRateSetting);

	bool enabled() const;
	ArrowSettingContainer arrowSetting() const;
	Post3dFaceSettingContainer faceSetting() const;
	Post3dSamplingRateSettingContainer samplingRateSetting() const;

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
	void applyDirectionSetting(const Post3dFaceSettingContainer &fs);
	void applyFaceSetting(const Post3dFaceSettingContainer& faceSetting);
	void maximizeRange();

	std::vector<std::string> m_scalars;

	Ui::Post3dWindowNodeGroupArrowFaceSettingWidget *ui;
};

#endif // POST3DWINDOWNODEGROUPARROWFACESETTINGWIDGET_H
