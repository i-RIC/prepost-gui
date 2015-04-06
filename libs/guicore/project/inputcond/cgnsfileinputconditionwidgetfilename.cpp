#include "cgnsfileinputconditionwidgetfilename.h"
#include "cgnsfileinputconditioncontainerstring.h"
#include "../../solverdef/solverdefinitiontranslator.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextCodec>
#include <QMessageBox>

QString CgnsFileInputConditionWidgetFilename::defaultFolder;

bool CgnsFileInputConditionWidgetFilenameEdit::checkContent()
{
	QString txt = text();
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	bool ok = asciiCodec->canEncode(txt);
	if (! ok){
		QMessageBox::warning(this, tr("Warning"), tr("File name has to consist of only English characters."));
		setText("");
	}
	return ok;
}

void CgnsFileInputConditionWidgetFilenameEdit::focusOutEvent(QFocusEvent* e)
{
	checkContent();

	emit valueChanged(text());
	QLineEdit::focusOutEvent(e);
}

CgnsFileInputConditionWidgetFilename::CgnsFileInputConditionWidgetFilename(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, CgnsFileInputConditionContainerString* cont) : CgnsFileInputConditionWidget(defnode){
	m_container = cont;
	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));

	m_edit = new CgnsFileInputConditionWidgetFilenameEdit(this);
	QPushButton* button = new QPushButton(QString(tr("...")), this);
	if (defnode.toElement().attribute("valueType") == "filename_all"){
		m_all = true;
	}else{
		m_all = false;
	}
	button->setFixedWidth(30);
	button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	// Should setup QLineEdit and a button;
	QHBoxLayout * l = new QHBoxLayout(0);
	l->setMargin(CgnsFileInputConditionWidget::margin);
	l->addWidget(m_edit, 10);
	l->addWidget(button, 0);
	setLayout(l);
	setValue(cont->value());
	connect(m_edit, SIGNAL(textChanged(QString)), this, SLOT(getWidgetValue(const QString &)));
	connect(button, SIGNAL(clicked()), this, SLOT(openFileDialog()));
}
void CgnsFileInputConditionWidgetFilename::openFileDialog(){
	QString originalFile = QDir::fromNativeSeparators(m_edit->text());
	QString dir;
	if (! originalFile.isEmpty()){
		QFileInfo finfo(originalFile);
		if (finfo.isAbsolute()){
			dir = finfo.absolutePath();
		}
	}
	if (! QFile::exists(dir)){
		dir = defaultFolder;
	}
	if (m_all){
		QString fname = QFileDialog::getSaveFileName(this, tr("Select File"), dir);
		if (! fname.isEmpty()){
			m_edit->setText(QDir::toNativeSeparators(fname));
			m_edit->checkContent();
			QFileInfo finfo(fname);
			defaultFolder = finfo.absolutePath();
		}
	}else{
		QString fname = QFileDialog::getOpenFileName(this, tr("Select File"), dir);
		if (! fname.isEmpty()){
			m_edit->setText(QDir::toNativeSeparators(fname));
			m_edit->checkContent();
			QFileInfo finfo(fname);
			defaultFolder = finfo.absolutePath();
		}
	}
}

void CgnsFileInputConditionWidgetFilename::getWidgetValue(const QString& newvalue){
	m_container->setValue(newvalue);
}
