#ifndef WINDOWWITHOBJECTBROWSERI_H
#define WINDOWWITHOBJECTBROWSERI_H

class ObjectBrowser;

class WindowWithObjectBrowserI
{
public:
	virtual ObjectBrowser* objectBrowser() const = 0;
};

#endif // WINDOWWITHOBJECTBROWSERI_H
