#ifndef CENTEREDCHECKBOX_H
#define CENTEREDCHECKBOX_H

#include "../guibase_global.h"

#include <QCheckBox>
#include <QWidget>

/// Checkbox that is aligned to center.
/**
 * The interface is basically the same to QCheckbox.
 */
class GUIBASEDLL_EXPORT CenteredCheckBox : public QWidget
{
	Q_OBJECT

public:
	explicit CenteredCheckBox(QWidget* parent = nullptr);
	QIcon icon() const {return m_checkBox->icon();}
	QSize iconSize() const {return m_checkBox->iconSize();}
	bool isCheckable() const {return m_checkBox->isCheckable();}
	bool isChecked() const {return m_checkBox->isChecked();}
	bool isDown() const {return m_checkBox->isDown();}
	void setAutoExclusive(bool e) {m_checkBox->setAutoExclusive(e);}
	void setAutoRepeat(bool r) {m_checkBox->setAutoRepeat(r);}
	void setAutoRepeatDelay(int delay) {m_checkBox->setAutoRepeatDelay(delay);}
	void setAutoRepeatInterval(int interval) {m_checkBox->setAutoRepeatInterval(interval);}
	void setCheckable(bool checkable) {m_checkBox->setCheckable(checkable);}
	void setDown(bool down) {m_checkBox->setDown(down);}
	void setIcon(const QIcon& icon) {m_checkBox->setIcon(icon);}
	void setShortcut(const QKeySequence& key) {m_checkBox->setShortcut(key);}
	void setText(const QString& text) {m_checkBox->setText(text);}
	QKeySequence shortcut() const {return m_checkBox->shortcut();}
	QString text() const {return m_checkBox->text();}

signals:
	void clicked(bool checked = false);
	void pressed();
	void released();
	void toggled(bool checked);
	void customContextMenuRequested(const QPoint& pos);
	void destroyed(QObject*);

public slots:
	void animateClick(int msec = 100) {m_checkBox->animateClick(msec);}
	void click() {m_checkBox->click();}
	void setChecked(bool checked) {m_checkBox->setChecked(checked);}
	void setIconSize(const QSize& size) {m_checkBox->setIconSize(size);}
	void toggle() {m_checkBox->toggle();}

private:
	QCheckBox* m_checkBox;
};

#endif // CENTEREDCHECKBOX_H
