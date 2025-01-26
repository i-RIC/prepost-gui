#ifndef BOUNDARYCONDITIONDIALOG_H
#define BOUNDARYCONDITIONDIALOG_H

#include <QDialog>
#include <guicore/project/inputcond/inputconditioncontainerstring.h>
#include <guicore/project/inputcond/inputconditioncontainerinteger.h>

#include <yaml-cpp/yaml.h>

class QAbstractButton;
class SolverDefinition;
class QDomElement;
class QLocale;
class iRICMainWindowI;

namespace Ui
{
	class BoundaryConditionDialog;
}

namespace iRICLib {
	class H5CgnsConditionGroup;
} // namespace iRICLib

class InputConditionContainerSet;
class InputConditionPage;
class InputConditionWidgetSet;
class iRICMainWindow;
class OpacityContainer;
class PreProcessorBCDataItem;

class QTextStream;

class BoundaryConditionDialog : public QDialog
{
	Q_OBJECT

public:
	BoundaryConditionDialog(PreProcessorBCDataItem* dataitem, iRICMainWindowI* mw, QWidget* parent);
	~BoundaryConditionDialog();

	void setup(SolverDefinition* def, const QDomElement& elem, const QLocale& locale);
	int load(const iRICLib::H5CgnsConditionGroup& group);
	int save(iRICLib::H5CgnsConditionGroup* group);
	void importFromYaml(const YAML::Node& node, const QDir& dir);
	void exportToYaml(QTextStream* stream, const QDir& dir, const QString& lineHeader = "");
	void setFileNamePrefix(const QString& prefix);

	void setType(const QString& type);

	QString caption() const;
	void setCaption(const QString& caption);

	OpacityContainer opacity() const;
	void setOpacity(const OpacityContainer& opacity);

	QColor color() const;
	void setColor(const QColor& color);

	int pointSize() const;
	void setPointSize(int size);

	bool showName() const;
	void setShowName(bool showName);

	void setEditMode();
	void setPropertyMode();

	QSize pageSizeHint() const;

public slots:
	void accept() override;

private slots:
	void setModified();

private:
	void reset();

	InputConditionContainerSet* m_containerSet;
	InputConditionWidgetSet* m_widgetSet;
	InputConditionPage* m_page;
	PreProcessorBCDataItem* m_dataItem;
	bool m_modified;

	QString m_caption;

	InputConditionContainerString m_captionContainer;
	InputConditionContainerString m_colorContainer;
	InputConditionContainerInteger m_opacityContainer;
	InputConditionContainerInteger m_showNameContainer;
	InputConditionContainerInteger m_pointSizeContainer;

	Ui::BoundaryConditionDialog* ui;

public:
	friend class InputConditionContainerSet;
};

#endif // BOUNDARYCONDITIONDIALOG_H
