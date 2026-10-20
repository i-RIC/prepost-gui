#ifndef MODALDIALOGRESPONDER_H
#define MODALDIALOGRESPONDER_H

#include <QObject>
#include <QStringList>
#include <QTimer>

#include <functional>

class QWidget;

/// Watches modal dialogs while tests run, records their texts, and closes them automatically.
/**
 * Tests can register a handler to answer a specific dialog (for example, to select an option).
 * When no handler accepts the dialog, the default answer is used:
 * - QMessageBox asking whether to save: "No" / "Discard"
 * - Other QMessageBox: "Yes", "OK" or the default button
 * - QColorDialog / QFileDialog: rejected
 * - Other dialogs: accepted
 */
class ModalDialogResponder : public QObject
{
	Q_OBJECT

public:
	/// Returns true when the dialog was handled.
	using Handler = std::function<bool(QWidget* dialog)>;

	explicit ModalDialogResponder(QObject* parent = nullptr);

	void setHandler(const Handler& handler);
	void clearHandler();

	/// Records of the dialogs shown, in the format "ClassName: text"
	const QStringList& records() const;
	void clearRecords();

private slots:
	void check();

private:
	void respondDefault(QWidget* dialog);

	QTimer m_timer;
	Handler m_handler;
	QStringList m_records;
	QWidget* m_lastDialog;
};

#endif // MODALDIALOGRESPONDER_H
