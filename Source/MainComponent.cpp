#include "MainComponent.h"

#include <numeric>

//==============================================================================
namespace Palette
{
    const juce::Colour background   { 0xffeef2f8 };
    const juce::Colour line         { 0xffdce1ea };
    const juce::Colour text         { 0xff141a26 };
    const juce::Colour textSoft     { 0xff4a5262 };
    const juce::Colour textMuted    { 0xff6b7383 };
    const juce::Colour blue         { 0xff1a66f5 };
    const juce::Colour blueHover    { 0xff0f56dc };
    const juce::Colour orange       { 0xffff9a1f };
    const juce::Colour track        { 0xffd5d9e0 };
    const juce::Colour rowWhite     { 0xffffffff };
    const juce::Colour rowBlue      { 0xffe4ecfb };
    const juce::Colour rowPlaying   { 0xfffdebc0 };
    const juce::Colour grey         { 0xffc6cad2 };
    const juce::Colour icon         { 0xff1f2430 };
}

static juce::Font font (float size, bool bold = false)
{
    return juce::Font (juce::FontOptions (size, bold ? juce::Font::bold : juce::Font::plain));
}

static const juce::String openFolderText = juce::String::fromUTF8 ("\xc3\x85" "bn mappe");

//==============================================================================
namespace Icons
{
    using R = juce::Rectangle<float>;

    static void logo (juce::Graphics& g, R r)
    {
        auto s = juce::jmin (r.getWidth(), r.getHeight());
        r = r.withSizeKeepingCentre (s, s);
        auto px = [&] (float fx) { return r.getX() + fx * s; };
        auto py = [&] (float fy) { return r.getY() + fy * s; };

        // Tray
        juce::Path tray;
        tray.startNewSubPath (px (0.02f), py (0.50f));
        tray.lineTo (px (0.30f), py (0.50f));
        tray.quadraticTo (px (0.50f), py (0.68f), px (0.70f), py (0.50f));
        tray.lineTo (px (0.98f), py (0.50f));
        tray.lineTo (px (0.86f), py (0.96f));
        tray.lineTo (px (0.14f), py (0.96f));
        tray.closeSubPath();
        g.setColour (Palette::blue);
        g.fillPath (juce::Path (tray).createPathWithRoundedCorners (s * 0.08f));

        // Note
        g.setColour (Palette::orange);
        g.fillEllipse (px (0.26f), py (0.34f), s * 0.22f, s * 0.18f);
        g.fillRect (px (0.42f), py (0.02f), s * 0.07f, s * 0.42f);
        juce::Path flag;
        flag.startNewSubPath (px (0.42f), py (0.02f));
        flag.lineTo (px (0.66f), py (0.14f));
        flag.lineTo (px (0.66f), py (0.26f));
        flag.lineTo (px (0.49f), py (0.18f));
        flag.closeSubPath();
        g.fillPath (flag);
    }

    static void folder (juce::Graphics& g, R r, juce::Colour c)
    {
        juce::Path p;
        auto w = r.getWidth(), h = r.getHeight();
        p.startNewSubPath (r.getX(), r.getY() + h * 0.10f);
        p.lineTo (r.getX() + w * 0.38f, r.getY() + h * 0.10f);
        p.lineTo (r.getX() + w * 0.50f, r.getY() + h * 0.26f);
        p.lineTo (r.getRight(), r.getY() + h * 0.26f);
        p.lineTo (r.getRight(), r.getBottom());
        p.lineTo (r.getX(), r.getBottom());
        p.closeSubPath();
        g.setColour (c);
        g.fillPath (p.createPathWithRoundedCorners (w * 0.12f));
    }

    static void skip (juce::Graphics& g, R r, juce::Colour c, bool forward)
    {
        auto s = juce::jmin (r.getWidth(), r.getHeight()) * 0.46f;
        auto b = r.withSizeKeepingCentre (s, s);
        auto bar = s * 0.20f;
        juce::Path tri;
        if (forward)
        {
            tri.addTriangle (b.getX(), b.getY(), b.getRight() - bar, b.getCentreY(), b.getX(), b.getBottom());
            g.setColour (c);
            g.fillPath (tri.createPathWithRoundedCorners (s * 0.08f));
            g.fillRoundedRectangle (b.getRight() - bar, b.getY(), bar, s, bar * 0.25f);
        }
        else
        {
            tri.addTriangle (b.getRight(), b.getY(), b.getX() + bar, b.getCentreY(), b.getRight(), b.getBottom());
            g.setColour (c);
            g.fillPath (tri.createPathWithRoundedCorners (s * 0.08f));
            g.fillRoundedRectangle (b.getX(), b.getY(), bar, s, bar * 0.25f);
        }
    }

    static void arrowHead (juce::Path& p, juce::Point<float> tip, float angle, float size)
    {
        juce::Path head;
        head.addTriangle (0.0f, 0.0f, -size, -size * 0.6f, -size, size * 0.6f);
        head.applyTransform (juce::AffineTransform::rotation (angle).translated (tip));
        p.addPath (head);
    }

    static void shuffle (juce::Graphics& g, R r, juce::Colour c)
    {
        auto s = juce::jmin (r.getWidth(), r.getHeight()) * 0.48f;
        auto b = r.withSizeKeepingCentre (s, s * 0.8f);
        auto x0 = b.getX(), x1 = b.getRight() - s * 0.12f, y0 = b.getY(), y1 = b.getBottom();
        auto stroke = juce::PathStrokeType (s * 0.11f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        juce::Path a, bb;
        a.startNewSubPath (x0, y1);
        a.cubicTo (x0 + s * 0.5f, y1, x1 - s * 0.5f, y0, x1, y0);
        bb.startNewSubPath (x0, y0);
        bb.cubicTo (x0 + s * 0.5f, y0, x1 - s * 0.5f, y1, x1, y1);

        juce::Path heads;
        arrowHead (heads, { b.getRight(), y0 }, 0.0f, s * 0.28f);
        arrowHead (heads, { b.getRight(), y1 }, 0.0f, s * 0.28f);

        g.setColour (c);
        g.strokePath (a, stroke);
        g.strokePath (bb, stroke);
        g.fillPath (heads);
    }

    static void repeat (juce::Graphics& g, R r, juce::Colour c, bool one)
    {
        auto s = juce::jmin (r.getWidth(), r.getHeight()) * 0.48f;
        auto b = r.withSizeKeepingCentre (s, s * 0.72f);
        auto stroke = juce::PathStrokeType (s * 0.11f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        auto rad = s * 0.2f;

        juce::Path top, bottom;
        top.startNewSubPath (b.getX(), b.getCentreY());
        top.lineTo (b.getX(), b.getY() + rad);
        top.quadraticTo (b.getX(), b.getY(), b.getX() + rad, b.getY());
        top.lineTo (b.getRight() - s * 0.2f, b.getY());

        bottom.startNewSubPath (b.getRight(), b.getCentreY());
        bottom.lineTo (b.getRight(), b.getBottom() - rad);
        bottom.quadraticTo (b.getRight(), b.getBottom(), b.getRight() - rad, b.getBottom());
        bottom.lineTo (b.getX() + s * 0.2f, b.getBottom());

        juce::Path heads;
        arrowHead (heads, { b.getRight() + s * 0.05f, b.getY() }, 0.0f, s * 0.28f);
        arrowHead (heads, { b.getX() - s * 0.05f, b.getBottom() }, juce::MathConstants<float>::pi, s * 0.28f);

        g.setColour (c);
        g.strokePath (top, stroke);
        g.strokePath (bottom, stroke);
        g.fillPath (heads);

        if (one)
        {
            auto badge = juce::Rectangle<float> (s * 0.52f, s * 0.52f).withCentre ({ b.getRight() + s * 0.12f, b.getY() - s * 0.12f });
            g.setColour (c);
            g.fillEllipse (badge);
            g.setColour (juce::Colours::white);
            g.setFont (font (s * 0.42f, true));
            g.drawText ("1", badge, juce::Justification::centred, false);
        }
    }

    static void speaker (juce::Graphics& g, R r, juce::Colour c)
    {
        auto s = juce::jmin (r.getWidth(), r.getHeight()) * 0.8f;
        auto b = r.withSizeKeepingCentre (s, s);
        juce::Path p;
        p.startNewSubPath (b.getX(), b.getY() + s * 0.34f);
        p.lineTo (b.getX() + s * 0.22f, b.getY() + s * 0.34f);
        p.lineTo (b.getX() + s * 0.52f, b.getY() + s * 0.08f);
        p.lineTo (b.getX() + s * 0.52f, b.getY() + s * 0.92f);
        p.lineTo (b.getX() + s * 0.22f, b.getY() + s * 0.66f);
        p.lineTo (b.getX(), b.getY() + s * 0.66f);
        p.closeSubPath();
        g.setColour (c);
        g.fillPath (p.createPathWithRoundedCorners (s * 0.06f));

        juce::Path wave;
        wave.addCentredArc (b.getX() + s * 0.56f, b.getCentreY(), s * 0.22f, s * 0.22f, 0.0f,
                            juce::MathConstants<float>::pi * 0.22f, juce::MathConstants<float>::pi * 0.78f, true);
        g.strokePath (wave, juce::PathStrokeType (s * 0.08f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    static void bars (juce::Graphics& g, R r, juce::Colour c, int tick, bool animate)
    {
        const float base[] = { 0.45f, 0.75f, 1.0f };
        auto w = r.getWidth() / 5.0f;
        g.setColour (c);
        for (int i = 0; i < 3; ++i)
        {
            auto f = base[i];
            if (animate)
                f = 0.35f + 0.65f * (0.5f + 0.5f * std::sin ((float) tick * 0.55f + (float) i * 1.9f));
            auto h = r.getHeight() * f;
            g.fillRoundedRectangle (r.getX() + (float) i * w * 2.0f, r.getBottom() - h, w, h, w * 0.5f);
        }
    }
}

//==============================================================================
SoundtrayLookAndFeel::SoundtrayLookAndFeel()
{
    setColour (juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::ListBox::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::ScrollBar::thumbColourId, Palette::grey);
    setColour (juce::ResizableWindow::backgroundColourId, Palette::background);
}

void SoundtrayLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                                             float sliderPos, float, float,
                                             juce::Slider::SliderStyle, juce::Slider& slider)
{
    const auto thumbRadius = (float) slider.getProperties().getWithDefault ("thumbRadius", 9.0);
    const auto trackHeight = 6.0f;
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h);
    auto trackArea = bounds.withSizeKeepingCentre (bounds.getWidth(), trackHeight);

    g.setColour (Palette::track);
    g.fillRoundedRectangle (trackArea, trackHeight * 0.5f);

    g.setColour (Palette::orange);
    g.fillRoundedRectangle (trackArea.withRight (juce::jmax (trackArea.getX() + trackHeight, sliderPos)), trackHeight * 0.5f);

    if (slider.isEnabled())
    {
        auto thumb = juce::Rectangle<float> (thumbRadius * 2.0f, thumbRadius * 2.0f)
                         .withCentre ({ sliderPos, bounds.getCentreY() });
        g.setColour (juce::Colours::black.withAlpha (0.12f));
        g.fillEllipse (thumb.translated (0.0f, 1.0f).expanded (1.0f));
        g.setColour (juce::Colours::white);
        g.fillEllipse (thumb);
        g.setColour (Palette::orange);
        g.fillEllipse (thumb.reduced (2.5f));
    }
}

void SoundtrayLookAndFeel::drawScrollbar (juce::Graphics& g, juce::ScrollBar&, int x, int y, int w, int h,
                                          bool isVertical, int thumbStart, int thumbSize, bool over, bool down)
{
    auto thumb = isVertical ? juce::Rectangle<int> (x, thumbStart, w, thumbSize)
                            : juce::Rectangle<int> (thumbStart, y, thumbSize, h);
    g.setColour (Palette::grey.withAlpha (over || down ? 1.0f : 0.7f));
    g.fillRoundedRectangle (thumb.toFloat().reduced (2.0f), 3.0f);
}

//==============================================================================
class MainComponent::DurationScanner final : public juce::Thread
{
public:
    DurationScanner (MainComponent& o, juce::Array<juce::File> f, int gen)
        : Thread ("Soundtray duration scan"), owner (&o), files (std::move (f)), generation (gen)
    {
        formats.registerBasicFormats();
    }

    ~DurationScanner() override { stopThread (4000); }

    void run() override
    {
        for (int i = 0; i < files.size() && ! threadShouldExit(); ++i)
        {
            double seconds = 0.0;
            if (std::unique_ptr<juce::AudioFormatReader> r { formats.createReaderFor (files[i]) })
                if (r->sampleRate > 0)
                    seconds = (double) r->lengthInSamples / r->sampleRate;

            juce::MessageManager::callAsync ([safe = owner, i, seconds, gen = generation]
            {
                if (auto* mc = safe.getComponent())
                {
                    if (mc->scanGeneration == gen && i < (int) mc->durations.size() && mc->durations[(size_t) i] < 0)
                    {
                        mc->durations[(size_t) i] = seconds;
                        mc->listBox.repaintRow (i);
                    }
                }
            });
        }
    }

private:
    juce::Component::SafePointer<MainComponent> owner;
    juce::Array<juce::File> files;
    int generation;
    juce::AudioFormatManager formats;
};

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel (&lnf);
    setWantsKeyboardFocus (true);

   #if JUCE_MAC
    headerHeight = 60;
   #else
    headerHeight = 64;
    auto windowButton = [] (const juce::String& name, int kind)
    {
        return std::make_unique<IconButton> (name, [kind] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool down)
        {
            if (over || down)
            {
                g.setColour (kind == 2 ? juce::Colour (0xffe5484d) : juce::Colours::black.withAlpha (down ? 0.10f : 0.06f));
                g.fillRoundedRectangle (r.reduced (4.0f, 12.0f), 6.0f);
            }

            auto c = (kind == 2 && over) ? juce::Colours::white : Palette::icon;
            auto b = r.withSizeKeepingCentre (14.0f, 14.0f);
            g.setColour (c);
            if (kind == 0)       g.drawLine (b.getX(), b.getCentreY(), b.getRight(), b.getCentreY(), 1.8f);
            else if (kind == 1)  g.drawRoundedRectangle (b.reduced (1.0f), 1.5f, 1.6f);
            else
            {
                g.drawLine (b.getX(), b.getY(), b.getRight(), b.getBottom(), 1.8f);
                g.drawLine (b.getRight(), b.getY(), b.getX(), b.getBottom(), 1.8f);
            }
        });
    };

    minimiseButton = windowButton ("Minimer", 0);
    maximiseButton = windowButton ("Maksimer", 1);
    closeButton    = windowButton ("Luk", 2);

    minimiseButton->onClick = [this] { if (auto* w = dynamic_cast<juce::DocumentWindow*> (window())) w->setMinimised (true); };
    maximiseButton->onClick = [this] { if (auto* w = dynamic_cast<juce::DocumentWindow*> (window())) w->setFullScreen (! w->isFullScreen()); };
    closeButton->onClick    = []     { juce::JUCEApplication::getInstance()->systemRequestedQuit(); };

    for (auto* b : { minimiseButton.get(), maximiseButton.get(), closeButton.get() })
        addAndMakeVisible (b);
   #endif

    // Open folder ---------------------------------------------------------
    openButton = std::make_unique<IconButton> (openFolderText, [] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool down)
    {
        g.setColour (down ? Palette::blueHover.darker (0.1f) : over ? Palette::blueHover : Palette::blue);
        g.fillRoundedRectangle (r, 12.0f);

        auto content = r.withSizeKeepingCentre (juce::jmin (r.getWidth() - 20.0f, 150.0f), r.getHeight());
        Icons::folder (g, content.removeFromLeft (26.0f).withSizeKeepingCentre (26.0f, 21.0f), juce::Colours::white);
        content.removeFromLeft (14.0f);
        g.setColour (juce::Colours::white);
        g.setFont (font (19.0f, true));
        g.drawText (openFolderText, content, juce::Justification::centredLeft, false);
    });
    openButton->onClick = [this] { chooseFolder(); };
    addAndMakeVisible (*openButton);

    // Playlist ----------------------------------------------------------------
    listBox.setRowHeight (57);
    listBox.setOutlineThickness (0);
    listBox.setColour (juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    listBox.getViewport()->setScrollBarThickness (8);
    addAndMakeVisible (listBox);

    // Progress & volume -------------------------------------------------------
    for (auto* s : { &progress, &volume })
    {
        s->setSliderStyle (juce::Slider::LinearHorizontal);
        s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s->setMouseCursor (juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible (*s);
    }

    progress.setRange (0.0, 1.0);
    progress.getProperties().set ("thumbRadius", 11.0);
    progress.onValueChange = [this]
    {
        if (readerSource != nullptr)
            transport.setPosition (progress.getValue());
    };

    volume.setRange (0.0, 1.0);
    volume.setValue (0.7, juce::dontSendNotification);
    volume.getProperties().set ("thumbRadius", 10.0);
    volume.onValueChange = [this]
    {
        auto v = (float) volume.getValue();
        transport.setGain (v * v);
    };

    // Transport ---------------------------------------------------------------
    shuffleButton = std::make_unique<IconButton> ("Bland", [this] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool)
    {
        if (shuffle)
        {
            g.setColour (Palette::blue.withAlpha (0.12f));
            g.fillEllipse (r.withSizeKeepingCentre (38.0f, 38.0f));
        }
        Icons::shuffle (g, r, shuffle ? Palette::blue : (over ? Palette::icon : Palette::textMuted));
    });
    shuffleButton->setTooltip ("Bland");
    shuffleButton->onClick = [this]
    {
        shuffle = ! shuffle;
        rebuildOrder();
        updateButtons();
        saveSettings();
    };

    repeatButton = std::make_unique<IconButton> ("Gentag", [this] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool)
    {
        auto active = repeat != RepeatMode::off;
        if (active)
        {
            g.setColour (Palette::blue.withAlpha (0.12f));
            g.fillEllipse (r.withSizeKeepingCentre (38.0f, 38.0f));
        }
        Icons::repeat (g, r, active ? Palette::blue : (over ? Palette::icon : Palette::textMuted), repeat == RepeatMode::one);
    });
    repeatButton->onClick = [this]
    {
        repeat = repeat == RepeatMode::off ? RepeatMode::all
               : repeat == RepeatMode::all ? RepeatMode::one
                                           : RepeatMode::off;
        updateButtons();
        saveSettings();
    };

    prevButton = std::make_unique<IconButton> ("Forrige", [] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool down)
    {
        Icons::skip (g, r, down ? Palette::blue : over ? Palette::icon.brighter (0.4f) : Palette::icon, false);
    });
    prevButton->onClick = [this] { previous(); };

    nextButton = std::make_unique<IconButton> (juce::String::fromUTF8 ("N\xc3\xa6ste"), [] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool down)
    {
        Icons::skip (g, r, down ? Palette::blue : over ? Palette::icon.brighter (0.4f) : Palette::icon, true);
    });
    nextButton->onClick = [this] { next (false); };

    playButton = std::make_unique<IconButton> ("Afspil", [this] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool down)
    {
        auto circle = r.withSizeKeepingCentre (juce::jmin (r.getWidth(), r.getHeight()), juce::jmin (r.getWidth(), r.getHeight()));
        g.setColour (juce::Colours::black.withAlpha (0.10f));
        g.fillEllipse (circle.reduced (2.0f).translated (0.0f, 2.0f));
        g.setColour (down ? Palette::blueHover.darker (0.1f) : over ? Palette::blueHover : Palette::blue);
        g.fillEllipse (circle.reduced (2.0f));

        g.setColour (juce::Colours::white);
        auto s = circle.getWidth() * 0.34f;
        auto b = circle.withSizeKeepingCentre (s, s);
        if (wantPlaying)
        {
            auto bw = s * 0.30f;
            g.fillRoundedRectangle (b.getX() + s * 0.06f, b.getY(), bw, s, bw * 0.3f);
            g.fillRoundedRectangle (b.getRight() - s * 0.06f - bw, b.getY(), bw, s, bw * 0.3f);
        }
        else
        {
            juce::Path tri;
            b = b.translated (s * 0.08f, 0.0f);
            tri.addTriangle (b.getX(), b.getY(), b.getRight(), b.getCentreY(), b.getX(), b.getBottom());
            g.fillPath (tri.createPathWithRoundedCorners (s * 0.1f));
        }
    });
    playButton->onClick = [this] { togglePlayPause(); };

    for (auto* b : { shuffleButton.get(), prevButton.get(), playButton.get(), nextButton.get(), repeatButton.get() })
    {
        b->setWantsKeyboardFocus (false);
        addAndMakeVisible (b);
    }
    openButton->setWantsKeyboardFocus (false);

    // Audio -------------------------------------------------------------------
    formatManager.registerBasicFormats();
    deviceManager.initialiseWithDefaultDevices (0, 2);
    sourcePlayer.setSource (&transport);
    deviceManager.addAudioCallback (&sourcePlayer);
    readAheadThread.startThread();

    // Settings ----------------------------------------------------------------
    juce::PropertiesFile::Options opts;
    opts.applicationName     = "Soundtray";
    opts.filenameSuffix      = ".settings";
    opts.folderName          = "Soundtray";
    opts.osxLibrarySubFolder = "Application Support";
    settings = std::make_unique<juce::PropertiesFile> (opts);

    volume.setValue (settings->getDoubleValue ("volume", 0.7), juce::sendNotificationSync);
    shuffle = settings->getBoolValue ("shuffle", false);
    repeat  = (RepeatMode) juce::jlimit (0, 2, settings->getIntValue ("repeat", 0));

    auto lastFolder = juce::File (settings->getValue ("folder"));
    if (lastFolder.isDirectory())
        loadFolder (lastFolder);

    updateButtons();
    setSize (575, 690);
    startTimerHz (30);
}

MainComponent::~MainComponent()
{
    stopTimer();
    saveSettings();
    scanner.reset();
    transport.setSource (nullptr);
    sourcePlayer.setSource (nullptr);
    deviceManager.removeAudioCallback (&sourcePlayer);
    readAheadThread.stopThread (2000);
    listBox.setModel (nullptr);
    setLookAndFeel (nullptr);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (Palette::background);

    // Header
    auto header = headerArea();
    g.setColour (Palette::line);
    g.fillRect (header.removeFromBottom (1));

    auto h = header.reduced (24, 0);
    Icons::logo (g, h.removeFromLeft (38).withSizeKeepingCentre (38, 38).toFloat());
    h.removeFromLeft (16);
    g.setColour (Palette::text);
    g.setFont (font (23.0f, true));
    g.drawText ("Soundtray", h, juce::Justification::centredLeft, false);

    // Folder info
    auto info = infoArea;
    auto deco = info.removeFromRight (90).toFloat();
    g.setColour (Palette::orange);
    g.fillEllipse (juce::Rectangle<float> (17.0f, 17.0f).withCentre ({ deco.getX() + 18.0f, deco.getCentreY() - 6.0f }));
    g.setColour (Palette::grey);
    g.fillRoundedRectangle (juce::Rectangle<float> (19.0f, 19.0f).withCentre ({ deco.getX() + 50.0f, deco.getCentreY() + 17.0f }), 3.0f);
    {
        juce::Path tri;
        auto c = juce::Point<float> (deco.getX() + 72.0f, deco.getCentreY() - 12.0f);
        tri.addTriangle (c.x, c.y - 12.0f, c.x + 11.0f, c.y + 9.0f, c.x - 12.0f, c.y + 6.0f);
        g.setColour (Palette::blue);
        g.fillPath (tri.createPathWithRoundedCorners (3.0f));
    }

    info.removeFromLeft (24);
    auto title = currentFolder.exists() ? currentFolder.getFileName() : juce::String ("Ingen mappe");
    if (title.isEmpty())
        title = currentFolder.getFullPathName();
    g.setColour (Palette::text);
    g.setFont (font (20.0f, true));
    g.drawText (title, info.removeFromTop (info.getHeight() / 2), juce::Justification::bottomLeft, true);

    g.setColour (Palette::textSoft);
    g.setFont (font (16.0f));
    auto n = tracks.size();
    auto count = currentFolder.exists() ? (juce::String (n) + (n == 1 ? " nummer" : " numre"))
                                        : juce::String::fromUTF8 ("V\xc3\xa6lg en mappe med musik");
    g.drawText (count, info.withTrimmedTop (4), juce::Justification::topLeft, true);

    // Empty playlist hint
    if (tracks.isEmpty())
    {
        g.setColour (Palette::textMuted);
        g.setFont (font (16.0f));
        auto msg = currentFolder.exists()
                       ? juce::String::fromUTF8 ("Der er ingen lydfiler i denne mappe.")
                       : juce::String::fromUTF8 ("Klik p\xc3\xa5 \xc2\xbb") + openFolderText
                             + juce::String::fromUTF8 ("\xc2\xab for at fylde playlisten.");
        g.drawFittedText (msg, listBox.getBounds().reduced (20), juce::Justification::centred, 3);
    }

    // Bottom panel
    g.setColour (Palette::line);
    g.fillRect (separatorArea);

    g.setColour (Palette::text);
    g.setFont (font (19.0f, true));
    g.drawText (nowPlayingTitle(), nowPlayingArea, juce::Justification::centredLeft, true);

    auto len = transport.getLengthInSeconds();
    g.setColour (Palette::textSoft);
    g.setFont (font (14.5f));
    g.drawText (formatTime (readerSource ? transport.getCurrentPosition() : 0.0), timeArea, juce::Justification::centredLeft, false);
    g.drawText (formatTime (readerSource ? len : 0.0), timeArea, juce::Justification::centredRight, false);

    g.setColour (Palette::line.darker (0.05f));
    g.fillRect (dividerArea);

    Icons::speaker (g, speakerArea.toFloat(), Palette::textMuted);
}

void MainComponent::resized()
{
    auto r = getLocalBounds();
    auto header = r.removeFromTop (headerHeight);

   #if ! JUCE_MAC
    header.removeFromRight (14);
    for (auto* b : { closeButton.get(), maximiseButton.get(), minimiseButton.get() })
    {
        b->setBounds (header.removeFromRight (48));
        header.removeFromRight (2);
    }
   #else
    juce::ignoreUnused (header);
   #endif

    r = r.reduced (24, 0);
    r.removeFromTop (20);

    auto topRow = r.removeFromTop (58);
    openButton->setBounds (topRow.removeFromLeft (juce::jmin (196, topRow.getWidth() / 2)).withTrimmedLeft (6));
    infoArea = topRow;

    r.removeFromTop (20);

    auto bottom = r.removeFromBottom (184);
    listBox.setBounds (r);

    bottom.removeFromTop (12);
    separatorArea = bottom.removeFromTop (1).reduced (10, 0);
    bottom = bottom.reduced (14, 0);
    bottom.removeFromTop (16);
    nowPlayingArea = bottom.removeFromTop (28);
    bottom.removeFromTop (4);
    progress.setBounds (bottom.removeFromTop (26).expanded (6, 0));
    timeArea = bottom.removeFromTop (20);

    auto controls = bottom.removeFromTop (78);

    auto vol = controls.removeFromRight (juce::jmin (180, controls.getWidth() / 3));
    speakerArea = vol.removeFromLeft (34);
    vol.removeFromLeft (8);
    volume.setBounds (vol.withSizeKeepingCentre (vol.getWidth(), 26).expanded (6, 0));

    dividerArea = controls.removeFromRight (24).withSizeKeepingCentre (1, 50);

    const int group = 44 + 8 + 50 + 18 + 70 + 18 + 50 + 8 + 44;
    auto g = controls.withSizeKeepingCentre (juce::jmin (group, controls.getWidth()), controls.getHeight());
    shuffleButton->setBounds (g.removeFromLeft (44));
    g.removeFromLeft (8);
    prevButton->setBounds (g.removeFromLeft (50));
    g.removeFromLeft (18);
    playButton->setBounds (g.removeFromLeft (70).withSizeKeepingCentre (70, 70));
    g.removeFromLeft (18);
    nextButton->setBounds (g.removeFromLeft (50));
    g.removeFromLeft (8);
    repeatButton->setBounds (g.removeFromLeft (44));
}

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress::spaceKey)                              { togglePlayPause(); return true; }
    if (key == juce::KeyPress (juce::KeyPress::rightKey, juce::ModifierKeys::commandModifier, 0)) { next (false); return true; }
    if (key == juce::KeyPress (juce::KeyPress::leftKey,  juce::ModifierKeys::commandModifier, 0)) { previous();   return true; }
    if (key == juce::KeyPress ('o', juce::ModifierKeys::commandModifier, 0))                      { chooseFolder(); return true; }
    return false;
}

void MainComponent::mouseDown (const juce::MouseEvent& e)
{
   #if ! JUCE_MAC
    if (headerArea().contains (e.getPosition()))
        if (auto* w = dynamic_cast<juce::ResizableWindow*> (window()); w != nullptr && ! w->isFullScreen())
            dragger.startDraggingComponent (w, e.getEventRelativeTo (w));
   #else
    juce::ignoreUnused (e);
   #endif
}

void MainComponent::mouseDrag (const juce::MouseEvent& e)
{
   #if ! JUCE_MAC
    if (headerArea().contains (e.getMouseDownPosition()))
        if (auto* w = dynamic_cast<juce::ResizableWindow*> (window()); w != nullptr && ! w->isFullScreen())
            dragger.dragComponent (w, e.getEventRelativeTo (w), nullptr);
   #else
    juce::ignoreUnused (e);
   #endif
}

void MainComponent::mouseDoubleClick (const juce::MouseEvent& e)
{
   #if ! JUCE_MAC
    if (headerArea().contains (e.getPosition()))
        if (auto* w = dynamic_cast<juce::DocumentWindow*> (window()))
            w->setFullScreen (! w->isFullScreen());
   #else
    juce::ignoreUnused (e);
   #endif
}

//==============================================================================
int MainComponent::getNumRows() { return tracks.size(); }

void MainComponent::paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (! juce::isPositiveAndBelow (row, tracks.size()))
        return;

    const bool isCurrent = row == currentIndex;
    auto r = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height).reduced (1.0f, 3.0f);

    auto fill = isCurrent ? Palette::rowPlaying : (row % 2 == 0 ? Palette::rowBlue : Palette::rowWhite);
    g.setColour (fill);
    g.fillRoundedRectangle (r, 10.0f);

    if (selected && ! isCurrent)
    {
        g.setColour (Palette::blue.withAlpha (0.45f));
        g.drawRoundedRectangle (r.reduced (0.75f), 10.0f, 1.5f);
    }

    auto content = r.toNearestInt().reduced (20, 0);

    g.setColour (Palette::textSoft);
    g.setFont (font (16.5f));
    g.drawText (juce::String (row + 1), content.removeFromLeft (42), juce::Justification::centredLeft, false);
    content.removeFromLeft (20);

    auto d = durations[(size_t) row];
    g.drawText (d >= 0 ? formatTime (d) : juce::String(), content.removeFromRight (60), juce::Justification::centredRight, false);

    if (isCurrent)
    {
        content.removeFromRight (14);
        Icons::bars (g, content.removeFromRight (24).withSizeKeepingCentre (22, 22).toFloat(),
                     Palette::orange, animationTick, wantPlaying);
    }
    content.removeFromRight (12);

    g.setColour (Palette::text);
    g.setFont (font (18.0f, true));
    g.drawText (tracks[row].getFileName(), content, juce::Justification::centredLeft, true);
}

void MainComponent::listBoxItemClicked (int row, const juce::MouseEvent&)
{
    if (juce::isPositiveAndBelow (row, tracks.size()))
        playIndex (row);
}

void MainComponent::returnKeyPressed (int lastRowSelected)
{
    if (juce::isPositiveAndBelow (lastRowSelected, tracks.size()))
        playIndex (lastRowSelected);
}

//==============================================================================
void MainComponent::timerCallback()
{
    if (readerSource != nullptr)
    {
        auto len = transport.getLengthInSeconds();
        if (len > 0 && std::abs (progress.getMaximum() - len) > 1.0e-6)
            progress.setRange (0.0, len, 0.0);

        if (! progress.isMouseButtonDown())
            progress.setValue (transport.getCurrentPosition(), juce::dontSendNotification);

        repaint (timeArea);

        if (wantPlaying && transport.hasStreamFinished())
            next (true);
    }

    if (wantPlaying && ++animationTick % 3 == 0 && currentIndex >= 0)
        listBox.repaintRow (currentIndex);
}

//==============================================================================
void MainComponent::chooseFolder()
{
    auto start = currentFolder.isDirectory() ? currentFolder
                                             : juce::File::getSpecialLocation (juce::File::userMusicDirectory);
    chooser = std::make_unique<juce::FileChooser> (juce::String::fromUTF8 ("V\xc3\xa6lg en mappe med lydfiler"), start);
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                          [this] (const juce::FileChooser& fc)
                          {
                              auto f = fc.getResult();
                              if (f.isDirectory())
                                  loadFolder (f);
                          });
}

void MainComponent::loadFolder (const juce::File& folder)
{
    stopPlayback();
    scanner.reset();

    currentFolder = folder;
    tracks.clear();

    juce::StringArray extensions;
    for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
        for (auto ext : formatManager.getKnownFormat (i)->getFileExtensions())
            extensions.addIfNotAlreadyThere (ext.toLowerCase());

    for (const auto& entry : juce::RangedDirectoryIterator (folder, false, "*", juce::File::findFiles))
        if (! entry.isHidden() && extensions.contains (entry.getFile().getFileExtension().toLowerCase()))
            tracks.add (entry.getFile());

    std::sort (tracks.begin(), tracks.end(), [] (const juce::File& a, const juce::File& b)
    {
        return a.getFileName().compareNatural (b.getFileName()) < 0;
    });

    durations.assign ((size_t) tracks.size(), -1.0);
    currentIndex = -1;
    rebuildOrder();

    listBox.updateContent();
    listBox.deselectAllRows();
    listBox.scrollToEnsureRowIsOnscreen (0);

    ++scanGeneration;
    scanner = std::make_unique<DurationScanner> (*this, tracks, scanGeneration);
    scanner->startThread (juce::Thread::Priority::low);

    saveSettings();
    updateButtons();
    repaint();
}

bool MainComponent::loadTrack (int index)
{
    transport.stop();
    transport.setSource (nullptr);
    readerSource.reset();

    if (! juce::isPositiveAndBelow (index, tracks.size()))
        return false;

    auto* reader = formatManager.createReaderFor (tracks[index]);
    if (reader == nullptr)
        return false;

    if (durations[(size_t) index] < 0 && reader->sampleRate > 0)
        durations[(size_t) index] = (double) reader->lengthInSamples / reader->sampleRate;

    auto sampleRate = reader->sampleRate;
    readerSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
    transport.setSource (readerSource.get(), 32768, &readAheadThread, sampleRate, 2);

    auto len = transport.getLengthInSeconds();
    progress.setRange (0.0, len > 0 ? len : 1.0, 0.0);
    progress.setValue (0.0, juce::dontSendNotification);
    return true;
}

void MainComponent::playIndex (int index)
{
    // Skip over unreadable files, but never loop forever.
    for (int attempts = 0; attempts < tracks.size(); ++attempts)
    {
        auto previous = currentIndex;
        currentIndex = index;
        listBox.repaintRow (previous);

        if (loadTrack (index))
        {
            wantPlaying = true;
            transport.start();
            listBox.selectRow (index, false, true);
            listBox.repaintRow (index);
            updateButtons();
            repaint();
            return;
        }

        auto pos = orderPositionOf (index);
        index = order[(size_t) ((pos + 1) % (int) order.size())];
    }

    stopPlayback();
}

void MainComponent::togglePlayPause()
{
    if (tracks.isEmpty())
    {
        chooseFolder();
        return;
    }

    if (readerSource == nullptr)
    {
        playIndex (currentIndex >= 0 ? currentIndex : order.front());
        return;
    }

    if (wantPlaying)
    {
        wantPlaying = false;
        transport.stop();
    }
    else
    {
        if (transport.hasStreamFinished())
            transport.setPosition (0.0);
        wantPlaying = true;
        transport.start();
    }

    updateButtons();
    listBox.repaintRow (currentIndex);
}

void MainComponent::next (bool automatic)
{
    if (tracks.isEmpty())
        return;

    if (automatic && repeat == RepeatMode::one && currentIndex >= 0)
    {
        playIndex (currentIndex);
        return;
    }

    auto pos = orderPositionOf (currentIndex) + 1;

    if (pos >= (int) order.size())
    {
        if (automatic && repeat == RepeatMode::off)
        {
            // End of the list: stop and rewind to the first track.
            stopPlayback();
            return;
        }

        if (shuffle)
        {
            auto last = currentIndex;
            rebuildOrder();
            if (order.size() > 1 && order.front() == last)
                std::swap (order.front(), order.back());
        }
        pos = 0;
    }

    playIndex (order[(size_t) pos]);
}

void MainComponent::previous()
{
    if (tracks.isEmpty())
        return;

    if (readerSource != nullptr && transport.getCurrentPosition() > 3.0)
    {
        transport.setPosition (0.0);
        return;
    }

    auto pos = orderPositionOf (currentIndex) - 1;
    if (pos < 0)
        pos = (int) order.size() - 1;

    playIndex (order[(size_t) pos]);
}

void MainComponent::stopPlayback()
{
    wantPlaying = false;
    transport.stop();
    transport.setSource (nullptr);
    readerSource.reset();
    progress.setValue (0.0, juce::dontSendNotification);

    auto old = currentIndex;
    currentIndex = -1;
    listBox.repaintRow (old);
    updateButtons();
    repaint();
}

void MainComponent::rebuildOrder()
{
    order.resize ((size_t) tracks.size());
    std::iota (order.begin(), order.end(), 0);

    if (shuffle && ! order.empty())
    {
        std::shuffle (order.begin(), order.end(), rng);

        // Keep the current song as the starting point of the new shuffled order.
        if (currentIndex >= 0)
        {
            auto it = std::find (order.begin(), order.end(), currentIndex);
            if (it != order.end())
                std::iter_swap (order.begin(), it);
        }
    }
}

int MainComponent::orderPositionOf (int index) const
{
    auto it = std::find (order.begin(), order.end(), index);
    return it == order.end() ? -1 : (int) std::distance (order.begin(), it);
}

void MainComponent::updateButtons()
{
    const bool hasTracks = ! tracks.isEmpty();
    prevButton->setEnabled (hasTracks);
    nextButton->setEnabled (hasTracks);
    progress.setEnabled (readerSource != nullptr);

    playButton->setTooltip (wantPlaying ? "Pause" : "Afspil");
    shuffleButton->setTooltip (shuffle ? "Bland: til" : "Bland: fra");
    repeatButton->setTooltip (repeat == RepeatMode::off ? "Gentag: fra"
                            : repeat == RepeatMode::all ? "Gentag: hele listen"
                                                        : "Gentag: denne sang");

    for (auto* b : { playButton.get(), shuffleButton.get(), repeatButton.get(), prevButton.get(), nextButton.get() })
        b->repaint();
}

void MainComponent::saveSettings()
{
    if (settings == nullptr)
        return;

    settings->setValue ("folder", currentFolder.getFullPathName());
    settings->setValue ("volume", volume.getValue());
    settings->setValue ("shuffle", shuffle);
    settings->setValue ("repeat", (int) repeat);
    settings->saveIfNeeded();
}

juce::String MainComponent::nowPlayingTitle() const
{
    if (juce::isPositiveAndBelow (currentIndex, tracks.size()))
        return tracks[currentIndex].getFileNameWithoutExtension();
    return juce::String::fromUTF8 ("Intet afspilles");
}

juce::String MainComponent::formatTime (double seconds)
{
    auto total = (int) std::floor (juce::jmax (0.0, seconds));
    auto h = total / 3600, m = (total / 60) % 60, s = total % 60;
    if (h > 0)
        return juce::String::formatted ("%d:%02d:%02d", h, m, s);
    return juce::String::formatted ("%d:%02d", m, s);
}
