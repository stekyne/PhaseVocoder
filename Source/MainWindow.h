#ifndef __MAINWINDOW_H_BE62E5DE__
#define __MAINWINDOW_H_BE62E5DE__

#include "../JuceLibraryCode/JuceHeader.h"
class AudioManager;

class MainAppWindow :   public DocumentWindow
{
public:
    MainAppWindow (AudioManager* audioManager);
    ~MainAppWindow ();

    void closeButtonPressed ();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainAppWindow)
};

#endif
