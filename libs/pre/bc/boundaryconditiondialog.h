#ifndef BOUNDARYCONDITIONDIALOG_H
#define BOUNDARYCONDITIONDIALOG_H

#include <QDialog>
#include <guicore/project/inputcond/cgnsfileinputconditioncontainerstring.h>
#include <guicore/project/inputcond/cgnsfileinputconditioncontainerinteger.h>

class QAbstractButton;
class SolverDefinition;
class QDomElement;
class QLocale;
class iRICMainWindowInterface;

namespace Ui {
	class BoundaryConditionDialog;
}

class PreProcessorBCDataItem;
class CgnsFileInputConditionContainerSet;
class CgnsFileInputConditionWidgetSet;
class iRICMainWindow;

class BoundaryConditionDialog : public QDialog {
	Q_OBJECT
public:
	/// Constructor
	BoundaryConditionDialog(PreProcessorBCDataItem* dataitem, iRICMainWindowInterface* mw, QWidget *parent = nullptr);
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
	void accept();

private slots:
	void setModified();

private:
	void reset();

	Ui::BoundaryConditionDialog *ui;
	CgnsFileInputConditionContainerSet* m_containerSet;
	CgnsFileInputConditionWidgetSet* m_widgetSet;
	PreProcessorBCDataItem* m_dataItem;
	bool m_modified;

	QString m_caption;

	CgnsFileInputConditionContainerString m_captionContainer;
	CgnsFileInputConditionContainerString m_colorContainer;
	CgnsFileInputConditionContainerInteger m_opacityContainer;
	CgnsFileInputConditionContainerInteger m_showNameContainer;
	CgnsFileInputConditionContainerInteger m_pointSizeContainer;
public:
	friend class CgnsFileInputConditionContainerSet;
};

#endif // BOUNDARYCONDITIONDIALOG_H
