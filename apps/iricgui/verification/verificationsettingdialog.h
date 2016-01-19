#ifndef VERIFICATIONSETTINGDIALOG_H
#define VERIFICATIONSETTINGDIALOG_H

#include <QDialog>

class PostSolutionInfo;
class MeasuredData;
class PostZoneDataContainer;

namespace Ui
{
	class VerificationSettingDialog;
}

class VerificationSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit VerificationSettingDialog(QWidget* parent = nullptr);
	~VerificationSettingDialog();
	void setPostSolutionInfo(PostSolutionInfo* info);
	void setMeasuredDatas(const std::vector<MeasuredData*>& data);

	void setTimeStep(int step);
	void setPostZoneDataContainer(PostZoneDataContainer* cont);
	void setActiveResult(const QString& result);

	void setMeasuredData(MeasuredData* md);
	void setActiveValue(const QString& value);

	int timeStep();
	PostZoneDataContainer* postZoneDataContainer();
	QString activeResult();

	MeasuredData* measuredData();
	QString activeValue();

public slots:
	void accept() override;

private slots:
	void selectZone(int zoneid);
	void selectFile(int fileid);

private:
	PostSolutionInfo* m_postSolutionInfo;
	std::vector<MeasuredData*> m_measuredData;

	Ui::VerificationSettingDialog* ui;
};

#endif // VERIFICATIONSETTINGDIALOG_H
