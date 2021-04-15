#ifndef POST3DWINDOWNODEVECTORARROWSETTINGDIALOG_H
#define POST3DWINDOWNODEVECTORARROWSETTINGDIALOG_H

#include "post3dfacesettingcontainer.h"
#include "post3dsamplingratesettingcontainer.h"

#include <guibase/arrowsettingcontainer.h>

#include <QString>
#include <QDialog>

#include <string>
#include <vector>

namespace Ui {
class Post3dWindowNodeVectorArrowSettingDialog;
}

class ArrowSettingContainer;
class PostZoneDataContainer;

class Post3dWindowNodeVectorArrowSettingDialog : public QDialog
{
	Q_OBJECT

public:
	struct FaceSetting {
		QString caption;
		bool enabled = true;
		Post3dFaceSettingContainer faceSetting;
		Post3dSamplingRateSettingContainer samplingRateSetting;
		ArrowSettingContainer arrowSetting;
	};

	explicit Post3dWindowNodeVectorArrowSettingDialog(QWidget *parent = nullptr);
	~Post3dWindowNodeVectorArrowSettingDialog();

	void setZoneData(PostZoneDataContainer* zoneData);

	ArrowSettingContainer arrowSetting() const;
	void setArrowSetting(const ArrowSettingContainer& setting);

	std::vector<FaceSetting> faceSettings() const;
	void setFaceSettings(const std::vector<FaceSetting>& faceSettings);

public slots:
	void accept() override;

private slots:
	void addFace();
	void removeFace();
	void setCurrentFace(int row);

private:
	void updateFaceList();
	void saveCurrentFace();

	std::vector<FaceSetting> m_faceSettings;
	std::vector<std::string> m_targets;
	int m_gridDims[3];
	int m_currentRow;

	Ui::Post3dWindowNodeVectorArrowSettingDialog *ui;
};

#endif // POST3DWINDOWNODEVECTORARROWSETTINGDIALOG_H
