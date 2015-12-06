#ifndef POST2DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem.h"

namespace Ui
{
	class Post2dWindowStreamlineStructuredSettingDialog;
}
class PostZoneDataContainer;

class Post2dWindowStreamlineStructuredSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowStreamlineStructuredSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowStreamlineStructuredSettingDialog();
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available) {m_activeAvailable = available;}

	void setSettings(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting>& stSettings);
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting setting() const;
	const QList<Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting>& stSettings() const {return m_stSettings;}

private slots:
	void activeDataChanged(int index);
	void iMinChanged(int min);
	void iMaxChanged(int max);
	void jMinChanged(int min);
	void jMaxChanged(int max);
	void handleSpaceSliderChange(int val);
	void colorChanged(const QColor& color);
	void widthChanged(int size);

	void addData();
	void removeData();
	void showRegionDialog();

private:
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post2dWindowStreamlineStructuredSettingDialog* ui;
	std::vector<std::string> m_solutions;

	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting m_setting;
	QList<Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting> m_stSettings;

	Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting* m_activeSetting;
	bool m_activeAvailable;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	bool m_applying;
};

#endif // POST2DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H
