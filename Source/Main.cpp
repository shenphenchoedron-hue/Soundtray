#include "MainComponent.h"

class SoundtrayApplication final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override    { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override          { return false; }

    void initialise (const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow> (getApplicationName());
    }

    void shutdown() override { mainWindow = nullptr; }

    void systemRequestedQuit() override { quit(); }

    class MainWindow final : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (const juce::String& name)
            : DocumentWindow (name, juce::Colour (0xffeef2f8), DocumentWindow::allButtons)
        {
           #if JUCE_MAC
            // Native title bar on macOS (traffic lights, minimise, full screen).
            setUsingNativeTitleBar (true);
           #else
            // Custom, app-drawn title bar on Windows / Linux - see MainComponent.
            setUsingNativeTitleBar (false);
            setTitleBarHeight (0);
           #endif

            setContentOwned (new MainComponent(), true);
            setResizable (true, false);
            setResizeLimits (420, 480, 4000, 4000);
            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

       #if ! JUCE_MAC
        // Keep minimise/close available to the OS (taskbar etc.) even without a drawn title bar.
        int getDesktopWindowStyleFlags() const override
        {
            return DocumentWindow::getDesktopWindowStyleFlags()
                 | juce::ComponentPeer::windowHasMinimiseButton
                 | juce::ComponentPeer::windowHasMaximiseButton
                 | juce::ComponentPeer::windowHasCloseButton;
        }
       #endif

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (SoundtrayApplication)
