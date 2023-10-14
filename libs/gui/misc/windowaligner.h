#ifndef WINDOWALIGNER_H
#define WINDOWALIGNER_H

class QMdiArea;

class WindowAligner
{
public:
	WindowAligner(QMdiArea* area);

	void align();

private:
	QMdiArea* m_mdiArea;;

	class Setting;
	class SettingDialog;
};

#endif // WINDOWALIGNER_H
