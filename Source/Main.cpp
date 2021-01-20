#include "MainComponent.h"

class JUCEAWSExampleApplication final : public juce::JUCEApplication
{
public:
    JUCEAWSExampleApplication()
    {
    }

    //==============================================================================
    const juce::String getApplicationName() override            { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override         { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override                  { return true; }
    void initialise (const juce::String&) override              { mainWindow.reset (new MainWindow (getApplicationName())); }
    void shutdown() override                                    { mainWindow = nullptr; }
    void systemRequestedQuit() override                         { quit(); }
    void anotherInstanceStarted (const juce::String&) override  { }

private:
    //==============================================================================
    class MainWindow final : public juce::DocumentWindow
    {
    public:
        MainWindow (const juce::String& name) :
            DocumentWindow (name, juce::Colours::darkgrey, DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JUCEAWSExampleApplication)
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (JUCEAWSExampleApplication)
