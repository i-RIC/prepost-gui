#ifndef PREPROCESSORINPUTCONDITIONDATAITEM_H
#define PREPROCESSORINPUTCONDITIONDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/project/projectdata.h>

class InputConditionDialog;
class QAction;

class PreProcessorInputConditionDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorInputConditionDataItem(GraphicsWindowDataItem* parent);
	virtual ~PreProcessorInputConditionDataItem();
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	ProjectData* projectData() const {return dynamic_cast<ProjectData*>(ProjectDataItem::projectData());}
	void handleStandardItemDoubleClicked() override {showDialog();}
	bool importInputCondition(const QString& filename);
	bool exportInputCondition(const QString& filename);
	bool isSet() {return m_isSet;}
	bool isSetupCorrectly() {return m_dialog != nullptr;}

public slots:
	void setModified() override {
		PreProcessorDataItem::setModified();
	}
	void showDialog(bool readonly = false);

private:
	bool m_isSet;
	InputConditionDialog* m_dialog;
};

#endif // PREPROCESSORINPUTCONDITIONDATAITEM_H
