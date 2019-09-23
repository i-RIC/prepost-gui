#ifndef GRAPH2DVERIFICATIONSETTINGDIALOG_H
#define GRAPH2DVERIFICATIONSETTINGDIALOG_H

#include <QDialog>
#include "graph2dverificationwindowresultsetting.h"

class PostSolutionInfo;
class MeasuredData;
class PostZoneDataContainer;

namespace Ui
{
	class Graph2dVerificationSettingDialog;
}

class Graph2dVerificationSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dVerificationSettingDialog(QWidget* parent = nullptr);
	~Graph2dVerificationSettingDialog();

	void setSetting(const Graph2dVerificationWindowResultSetting& setting);
	const Graph2dVerificationWindowResultSetting& setting() const; //  { return m_setting; }

	void setPostSolutionInfo(PostSolutionInfo* info);
	void setMeasuredDatas(const std::vector<MeasuredData*>& data);

	void setTimeStep(int step);
	void setPostZoneDataContainer(PostZoneDataContainer* cont);
	void setActiveResult(const QString& result);

	void setMeasuredData(MeasuredData* md);
	void setActiveValue(const QString& value);

	int timeStep() const;
	PostZoneDataContainer* postZoneDataContainer() const;
	QString activeResult() const;

	MeasuredData* measuredData() const;
	QString activeFile() const;
	
	QString activeValue() const;
	///void setActiveValue(QString activeValue);
	void setActiveFile(const QString& file);

	QString activeZone() const;

public slots:
	void accept() override;

private slots:
	void selectZone(int zoneid);
	void selectFile(int fileid);

private:
	PostSolutionInfo* m_postSolutionInfo;
	std::vector<MeasuredData*> m_measuredData;

	Ui::Graph2dVerificationSettingDialog* ui;
	mutable Graph2dVerificationWindowResultSetting m_setting;
};

#endif // GRAPH2DVERIFICATIONSETTINGDIALOG_H
