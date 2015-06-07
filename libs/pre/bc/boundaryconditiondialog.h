#ifndef BOUNDARYCONDITIONDIALOG_H
#define BOUNDARYCONDITIONDIALOG_H

#include <QDialog>
#include <guicore/project/inputcond/inputconditioncontainerstring.h>
#include <guicore/project/inputcond/inputconditioncontainerinteger.h>

class QAbstractButton;
class SolverDefinition;
class QDomElement;
class QLocale;
class iRICMainWindowInterface;

namespace Ui
{
	class BoundaryConditionDialog;
}

class PreProcessorBCDataItem;
class InputConditionContainerSet;
class InputConditionWidgetSet;
class iRICMainWindow;

class BoundaryConditionDialog : public QDialog
{
	Q_OBJECT
public:
	/// Constructor
	BoundaryConditionDialog(PreProcessorBCDataItem* dataitem, iRICMainWindowInterface* mw, QWidget* parent = nullptr);
	/// Destructor
	~BoundaryConditionDialog();
	void setNameAndNumber(QString name, int number);
	/// Load data fron CGNS file.
	void load(const int fn);
	/// Save data into CGNS file.
	void save(const int fn);
	/// Setup the dialog.
	void setup(SolverDefinition* def, const QDomElement& elem, const QLocale& locale);

	void setType(const QString& type);
	void setCaption(const QString& caption);
	const QString caption() const;
	void setOpacityPercent(int opacity);
	int opacityPercent() const;
	QColor color();
	void setColor(const QColor& color);
	int pointSize() const;
	void setPointSize(int size);
	bool showName() const;
	void setShowName(bool showName);

	void setEditMode();
	void setPropertyMode();

public slots:
	void accept() override;

private slots:
	void setModified();

private:
	void reset();

	Ui::BoundaryConditionDialog* ui;
	InputConditionContainerSet* m_containerSet;
	InputConditionWidgetSet* m_widgetSet;
	PreProcessorBCDataItem* m_dataItem;
	bool m_modified;

	QString m_caption;

	InputConditionContainerString m_captionContainer;
	InputConditionContainerString m_colorContainer;
	InputConditionContainerInteger m_opacityContainer;
	InputConditionContainerInteger m_showNameContainer;
	InputConditionContainerInteger m_pointSizeContainer;
public:
	friend class InputConditionContainerSet;
};

#endif // BOUNDARYCONDITIONDIALOG_H
