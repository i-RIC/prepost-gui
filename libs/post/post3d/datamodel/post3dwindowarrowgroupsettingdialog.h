#ifndef POST3DWINDOWARROWGROUPSETTINGDIALOG_H
#define POST3DWINDOWARROWGROUPSETTINGDIALOG_H

#include "post3dwindowarrowgroupdataitem.h"
#include "post3dwindowfacedataitem.h"

#include <QDialog>
#include <QMap>

class PostZoneDataContainer;
class Post3dWindowGridTypeDataItem;
class QListWidgetItem;
class ArrowSettingContainer;

namespace Ui {
		class Post3dWindowArrowGroupSettingDialog;
}

class Post3dWindowArrowGroupSettingDialog : public QDialog {
		Q_OBJECT
public:
	Post3dWindowArrowGroupSettingDialog(QWidget *parent = 0);
	~Post3dWindowArrowGroupSettingDialog();
	void setColor(const QColor &color);
	QColor color();
	void setMapping(Post3dWindowArrowGroupDataItem::Mapping m);
	Post3dWindowArrowGroupDataItem::Mapping mapping();
	void setScalarValueName(const QString &name);
	QString scalarValueName();
	void setSampleRate(int rate);
	int sampleRate();
	void setCurrentSolution(QString sol);
	QString currentSolution();
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting> &map);
	const QMap<QString, Post3dWindowFaceDataItem::Setting>& faceMap();
	void setZoneData(PostZoneDataContainer *zoneData);
	void setLengthMode(Post3dWindowArrowGroupDataItem::LengthMode lm);
	Post3dWindowArrowGroupDataItem::LengthMode lengthMode();
	void setStandardValue(double stdVal);
	double standardValue();
	void setLegendLength(int len);
	int legendLength();
	void setMinimumValue(double minVal);
	double minimumValue();
	void setArrowSetting(const ArrowSettingContainer& arrowSetting);
	ArrowSettingContainer arrowSetting();


public slots:
	void addFaceSetting();
	void removeFaceSetting();
	void switchFaceSetting(QListWidgetItem* current, QListWidgetItem* previous);
private slots:
	void solutionChanged(int index);
	void checkSelectedNumber();
	void allSamplingToggled(bool toggled);
	void updateFaceMap();
private:
	QList<QString> m_solutions;
	QList<QString> m_scalars;
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_faceMap;
	bool m_isRemoving;

		Ui::Post3dWindowArrowGroupSettingDialog *ui;
};

#endif // POST3DWINDOWARROWGROUPSETTINGDIALOG_H
