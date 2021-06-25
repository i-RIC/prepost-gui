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
	PreProcessorInputConditionDataItem(GraphicsWindowDataItem* parent);
	~PreProcessorInputConditionDataItem();

	int loadFromCgnsFile() override;
	int saveToCgnsFile() override;
	int updateCgnsFileOtherThanGrids() override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	ProjectData* projectData() const;
	void handleStandardItemDoubleClicked() override;
	void checkImportSourceUpdate();

	bool importInputCondition(const QString& filename);
	bool exportInputCondition(const QString& filename);
	bool setupCgnsFilesIfNeeded(QString* cgnsFileForGrid, bool *updated);
	bool isSet() const;
	bool isSetupCorrectly() const;

public slots:
	void setModified(bool modified = true);
	void showDialog(bool readonly = false);

private:
	bool m_isSet;
	InputConditionDialog* m_dialog;
};

#endif // PREPROCESSORINPUTCONDITIONDATAITEM_H
