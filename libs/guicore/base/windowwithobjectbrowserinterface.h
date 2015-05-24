#ifndef WINDOWWITHOBJECTBROWSERINTERFACE_H
#define WINDOWWITHOBJECTBROWSERINTERFACE_H

class ObjectBrowser;

class WindowWithObjectBrowserInterface
{
public:
	virtual ObjectBrowser* objectBrowser() = 0;
};

#endif // WINDOWWITHOBJECTBROWSERINTERFACE_H
