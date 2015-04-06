#ifndef SOLVERDEFINITIONTRANSLATIONUPDATEWIZARD_H
#define SOLVERDEFINITIONTRANSLATIONUPDATEWIZARD_H

#include <QWizard>
#include <QList>

class QLabel;
class QComboBox;
class QRadioButton;
class QCheckBox;
class QTextEdit;
class SolverDefinitionList;
class GridCreatingConditionCreator;

class IntroductionPage : public QWizardPage
{
	Q_OBJECT
public:
	IntroductionPage(QWidget* parent = 0);
private:
	QLabel* m_label;
};

class SettingPage : public QWizardPage
{
	Q_OBJECT
public:
	SettingPage(QWidget* parent = 0);
	void init(SolverDefinitionList* list, const QList<GridCreatingConditionCreator*>& gclist);
	QList<QLocale> selectedLanguages();
	void initializePage();
private:
	QRadioButton* m_solverRadioButton;
	QRadioButton* m_gridGenRadioButton;
	QComboBox* m_solverCombobox;
	QComboBox* m_gridGenComboBox;
	QList<QCheckBox*> m_languageCheckboxes;
};

class ConfirmPage : public QWizardPage
{
	Q_OBJECT
public:
	ConfirmPage(QWidget* parent = 0);
	void initializePage();
private:
	QTextEdit* m_textEdit;
};

class SolverDefinitionTranslationUpdateWizard : public QWizard
{
	Q_OBJECT
public:
	SolverDefinitionTranslationUpdateWizard(QWidget* parent);
	void init(SolverDefinitionList* list, const QList<GridCreatingConditionCreator*>& gclist);
	SettingPage* settingPage(){return dynamic_cast<SettingPage*>(page(m_settingPageId));}
	SolverDefinitionList* solverList(){return m_solverList;}
	const QList<GridCreatingConditionCreator*>& gridCreatorList(){return m_gridCreatorList;}
	void accept();
	static QList<QLocale> supportedLanguages();
private:
	SolverDefinitionList* m_solverList;
	QList<GridCreatingConditionCreator*> m_gridCreatorList;
	int m_settingPageId;
};

#endif // SOLVERDEFINITIONTRANSLATIONUPDATEWIZARD_H
