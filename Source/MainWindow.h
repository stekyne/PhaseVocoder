#ifndef __MAINWINDOW_H_BE62E5DE__
#define __MAINWINDOW_H_BE62E5DE__

#include "../JuceLibraryCode/JuceHeader.h"

class MainAppWindow :   public DocumentWindow
{
public:
    MainAppWindow ();
    ~MainAppWindow ();

    void closeButtonPressed ();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainAppWindow)
};

#endif
