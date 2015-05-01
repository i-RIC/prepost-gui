#ifndef PREPROCESSORINPUTCONDITIONDATAITEM_H
#define PREPROCESSORINPUTCONDITIONDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/project/projectdata.h>

class CgnsFileInputConditionDialog;
class QAction;

class PreProcessorInputConditionDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorInputConditionDataItem(GraphicsWindowDataItem* parent);
	virtual ~PreProcessorInputConditionDataItem();
	void loadFromCgnsFile(const int fn);
	void saveToCgnsFile(const int fn);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	ProjectData* projectData() {return dynamic_cast<ProjectData*>(ProjectDataItem::projectData());}
	void handleStandardItemDoubleClicked() {showDialog();}
	bool importInputCondition(const QString& filename);
	bool exportInputCondition(const QString& filename);
	bool isSet() {return m_isSet;}
	bool isSetupCorrectly() {return m_dialog != nullptr;}
public slots:
	void setModified() {
		PreProcessorDataItem::setModified();
	}
	void showDialog(bool readonly = false);
private:
	bool m_isSet;
	CgnsFileInputConditionDialog* m_dialog;
};

#endif // PREPROCESSORINPUTCONDITIONDATAITEM_H
