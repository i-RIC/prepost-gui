#ifndef CLIPBOARDOPERATABLEWINDOWINTERFACE_H
#define CLIPBOARDOPERATABLEWINDOWINTERFACE_H

/// This class is the abstract class of windows those support
/// clipboard related operations, i.e. cut, copy, and paste.
class ClipboardOperatableWindowInterface
{
public:
	/// Returns true if this window support cut()
	virtual bool acceptCut() const {return false;}
	/// Cut operation
	virtual void cut() {}
	/// Returns true if this window support copy()
	virtual bool acceptCopy() const {return false;}
	/// Copy operation
	virtual void copy() const {}
	/// Returns true if this window support paste()
	virtual bool acceptPaste() const {return false;}
	/// Paste operation
	virtual void paste() {}
};

#endif // CLIPBOARDOPERATABLEWINDOWINTERFACE_H
