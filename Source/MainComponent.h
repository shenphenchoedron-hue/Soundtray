#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <random>
#include <vector>

//==============================================================================
/** A button whose appearance is drawn by a lambda. */
class IconButton final : public juce::Button
{
public:
    using Drawer = std::function<void (juce::Graphics&, juce::Rectangle<float>, bool over, bool down)>;

    IconButton (const juce::String& name, Drawer d) : Button (name), drawer (std::move (d))
    {
        setMouseCursor (juce::MouseCursor::PointingHandCursor);
    }

    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        if (drawer)
            drawer (g, getLocalBounds().toFloat(), over, down);
    }

private:
    Drawer drawer;
};

//==============================================================================
class SoundtrayLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    SoundtrayLookAndFeel();

    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h,
                           float sliderPos, float minPos, float maxPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;

    void drawScrollbar (juce::Graphics&, juce::ScrollBar&, int x, int y, int w, int h,
                        bool isVertical, int thumbStart, int thumbSize,
                        bool isMouseOver, bool isMouseDown) override;

    int getDefaultScrollbarWidth() override { return 8; }
};

//==============================================================================
class MainComponent final : public juce::Component,
                            private juce::ListBoxModel,
                            private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;

private:
    enum class RepeatMode { off, all, one };

    // ListBoxModel
    int  getNumRows() override;
    void paintListBoxItem (int row, juce::Graphics&, int width, int height, bool selected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    void returnKeyPressed (int lastRowSelected) override;

    // Timer
    void timerCallback() override;

    // Playlist / playback
    void chooseFolder();
    void loadFolder (const juce::File& folder);
    void scanDurations();
    bool loadTrack (int index);
    void playIndex (int index);
    void togglePlayPause();
    void next (bool automatic);
    void previous();
    void stopPlayback();
    void rebuildOrder();
    int  orderPositionOf (int index) const;
    void updateButtons();
    void saveSettings();

    juce::String nowPlayingTitle() const;
    static juce::String formatTime (double seconds);

    // UI helpers
    juce::Rectangle<int> headerArea() const { return getLocalBounds().removeFromTop (headerHeight); }
    juce::Component* window() { return getTopLevelComponent(); }

    SoundtrayLookAndFeel lnf;
    juce::TooltipWindow tooltipWindow { this, 600 };

    std::unique_ptr<IconButton> minimiseButton, maximiseButton, closeButton;
    std::unique_ptr<IconButton> openButton, shuffleButton, prevButton, playButton, nextButton, repeatButton;
    juce::ListBox listBox { "playlist", this };
    juce::Slider progress, volume;

    juce::ComponentDragger dragger;
    juce::Rectangle<int> infoArea, nowPlayingArea, timeArea, dividerArea, speakerArea, separatorArea;
    int headerHeight = 0;

    // Audio
    juce::AudioDeviceManager deviceManager;
    juce::AudioFormatManager formatManager;
    juce::AudioSourcePlayer sourcePlayer;
    juce::AudioTransportSource transport;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::TimeSliceThread readAheadThread { "Soundtray read-ahead" };

    // Playlist state
    juce::File currentFolder;
    juce::Array<juce::File> tracks;
    std::vector<double> durations;     // seconds, < 0 = unknown
    std::vector<int> order;            // playback order (shuffled when shuffle is on)
    int currentIndex = -1;
    bool wantPlaying = false;
    bool shuffle = false;
    RepeatMode repeat = RepeatMode::off;
    std::mt19937 rng { std::random_device{}() };
    int animationTick = 0;

    // Background duration scanning
    class DurationScanner;
    std::unique_ptr<DurationScanner> scanner;
    int scanGeneration = 0;

    std::unique_ptr<juce::PropertiesFile> settings;
    std::unique_ptr<juce::FileChooser> chooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
