#ifndef WINDOWWITHOBJECTBROWSERINTERFACE_H
#define WINDOWWITHOBJECTBROWSERINTERFACE_H

class ObjectBrowser;

class WindowWithObjectBrowserInterface
{
public:
	virtual ObjectBrowser* objectBrowser() const = 0;
};

#endif // WINDOWWITHOBJECTBROWSERINTERFACE_H
