#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerstring.h"
#include "inputconditionwidgetstring.h"
#include "inputconditionwidgettooltip.h"

#include <guibase/widget/asciionlylineedit.h>
#include <guibase/widget/asciionlytextedit.h>
#include <misc/informationdialog.h>

#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextCodec>
#include <QVBoxLayout>

InputConditionWidgetString::InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, const QString& folderName) :
	InputConditionWidget(defnode),
	m_container {cont},
	m_sourceComboBox {nullptr}
{
	auto layout = new QVBoxLayout(this);
	layout->setMargin(InputConditionWidget::margin);
	if (defnode.toElement().attribute("multiline") == "true") {
		AsciiOnlyTextEdit* te = new AsciiOnlyTextEdit(this);
		connect(te, SIGNAL(textChanged()), this, SLOT(handleEditingFinished()));

		setupComboBoxIfNeeded(defnode, t, cont, folderName, layout);

		layout->addWidget(te, 1);
		m_textEdit = te;
	}	else {
		AsciiOnlyLineEdit* te = new AsciiOnlyLineEdit(this);
		connect(te, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
		layout->addWidget(te, 1);
		m_textEdit = te;
	}
	setLayout(layout);

	setValue(cont->value());

	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));
}

void InputConditionWidgetString::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(0, tt);
}

void InputConditionWidgetString::setValue(const QString& newvalue)
{
	if (newvalue == m_textEdit->text()) {return;}

	m_textEdit->setText(newvalue);
}

void InputConditionWidgetString::handleEditingFinished()
{
	m_container->setValue(m_textEdit->text());
}

void InputConditionWidgetString::setupComboBoxIfNeeded(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, const QString& folderName, QLayout* l)
{
	QDir folder(folderName);

	std::vector<QString> captions;
	const auto cn = defnode.childNodes();
	for (int i = 0; i < cn.size(); ++i) {
		auto child = cn.at(i);
		if (child.nodeName() != "Source") {continue;}
		auto elem = child.toElement();

		captions.push_back(t.translate(elem.attribute("caption")));
		auto fname = folder.absoluteFilePath(elem.attribute("filename"));
		m_sourceFileNames.push_back(fname);
		if (elem.attribute("default") == "true") {
			auto utf8 = QTextCodec::codecForName("utf-8");
			QFile f(fname);
			f.open(QFile::ReadOnly);
			auto content = utf8->toUnicode(f.readAll());
			cont->setDefaultValue(content);
			cont->setValue(content);
		}
	}

	if (captions.size() > 0) {
		m_sourceComboBox = new QComboBox(this);
		m_sourceComboBox->addItem(tr("(Select template to import)"));

		for (int i = 0; i < captions.size(); ++i) {
			m_sourceComboBox->addItem(captions.at(i));
		}
		connect<void (QComboBox::*)(int)>(m_sourceComboBox, &QComboBox::currentIndexChanged, this, &InputConditionWidgetString::applyTemplate);

		auto l2 = new QHBoxLayout(this);
		l2->addWidget(m_sourceComboBox);
		l2->addStretch(1);
		l->addItem(l2);
	}
}

void InputConditionWidgetString::applyTemplate(int index)
{
	int ret = QMessageBox::warning(this, tr("Warning"), tr("The content in the text area is overwritten with the template"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (ret == QMessageBox::Ok) {
		auto fname = m_sourceFileNames.at(index - 1);
		auto utf8 = QTextCodec::codecForName("utf-8");
		QFile f(fname);
		f.open(QFile::ReadOnly);
		auto content = utf8->toUnicode(f.readAll());
		m_textEdit->setText(content);
	}

	m_sourceComboBox->blockSignals(true);
	m_sourceComboBox->setCurrentIndex(0);
	m_sourceComboBox->blockSignals(false);
}
