#include "PluginEditor.h"

//==============================================================================
GainMonoAudioProcessorEditor::GainMonoAudioProcessorEditor (GainMonoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&alphaLAF);

    //==========================================================================
    // Preset box
    for (int i = 0; i < (int)GainMonoAudioProcessor::getPresets().size(); ++i)
        presetBox.addItem (GainMonoAudioProcessor::getPresets()[i].name, i + 1);
    presetBox.setSelectedId (audioProcessor.getCurrentProgram() + 1,
                             juce::dontSendNotification);
    presetBox.onChange = [this]
    {
        audioProcessor.setCurrentProgram (presetBox.getSelectedId() - 1);
    };
    addAndMakeVisible (presetBox);

    //==========================================================================
    // Bypass
    bypassToggle.setName ("bypass");
    addAndMakeVisible (bypassToggle);
    bypassAtt = std::make_unique<BA> (audioProcessor.apvts, ParamIDs::bypass, bypassToggle);

    //==========================================================================
    // Monitor
    modeBox.addItem ("Stereo",    1);
    modeBox.addItem ("Solo Mid",  2);
    modeBox.addItem ("Solo Side", 3);
    modeBox.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (modeBox);
    modeAtt = std::make_unique<CA> (audioProcessor.apvts, ParamIDs::mode, modeBox);

    sumLawBox.addItem ("-6 dB  (Voltage)", 1);
    sumLawBox.addItem ("-3 dB  (Power)",   2);
    sumLawBox.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (sumLawBox);
    sumLawAtt = std::make_unique<CA> (audioProcessor.apvts, ParamIDs::sumLaw, sumLawBox);

    gainSlider.setSliderStyle (juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 16);
    gainSlider.setTextValueSuffix (" dB");
    addAndMakeVisible (gainSlider);
    gainAtt = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::gainDb, gainSlider);

    sideGainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    sideGainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 16);
    sideGainSlider.setTextValueSuffix (" dB");
    addAndMakeVisible (sideGainSlider);
    sideGainAtt = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::sideGain, sideGainSlider);

    //==========================================================================
    // Side Filters
    sideHPFToggle.setName ("eqon");
    addAndMakeVisible (sideHPFToggle);
    sideHPFOnAtt = std::make_unique<BA> (audioProcessor.apvts, ParamIDs::sideHPFOn, sideHPFToggle);

    sideHPFSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    sideHPFSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 16);
    sideHPFSlider.setTextValueSuffix (" Hz");
    addAndMakeVisible (sideHPFSlider);
    sideHPFAtt = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::sideHPF, sideHPFSlider);

    sideLPFToggle.setName ("eqon");
    addAndMakeVisible (sideLPFToggle);
    sideLPFOnAtt = std::make_unique<BA> (audioProcessor.apvts, ParamIDs::sideLPFOn, sideLPFToggle);

    sideLPFSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    sideLPFSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 16);
    sideLPFSlider.setTextValueSuffix (" Hz");
    addAndMakeVisible (sideLPFSlider);
    sideLPFAtt = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::sideLPF, sideLPFSlider);

    //==========================================================================
    // Bass Focus (legacy)
    bassMonoToggle.setName ("eqon");
    addAndMakeVisible (bassMonoToggle);
    bassMonoAtt = std::make_unique<BA> (audioProcessor.apvts, ParamIDs::bassMono, bassMonoToggle);

    bassFreqSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    bassFreqSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 16);
    bassFreqSlider.setTextValueSuffix (" Hz");
    addAndMakeVisible (bassFreqSlider);
    bassFreqAtt = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::bassFreq, bassFreqSlider);

    //==========================================================================
    // M/S EQ Bands
    for (int i = 0; i < 3; ++i)
    {
        eqRows[i].init (i);
        addAndMakeVisible (eqRows[i]);

        eqOnAtt    [i] = std::make_unique<BA> (audioProcessor.apvts, ParamIDs::eqBandOn    [i], eqRows[i].onToggle);
        eqTypeAtt  [i] = std::make_unique<CA> (audioProcessor.apvts, ParamIDs::eqBandType  [i], eqRows[i].typeBox);
        eqFreqAtt  [i] = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::eqBandFreq  [i], eqRows[i].freqSlider);
        eqGainAtt  [i] = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::eqBandGain  [i], eqRows[i].gainSlider);
        eqQAtt     [i] = std::make_unique<SA> (audioProcessor.apvts, ParamIDs::eqBandQ     [i], eqRows[i].qSlider);
        eqTargetAtt[i] = std::make_unique<CA> (audioProcessor.apvts, ParamIDs::eqBandTarget[i], eqRows[i].targetBox);
    }

    //==========================================================================
    // Window size: wider to accommodate EQ matrix
    setSize (620, 520);
    startTimerHz (24);
}

GainMonoAudioProcessorEditor::~GainMonoAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void GainMonoAudioProcessorEditor::drawSectionPanel (juce::Graphics& g,
                                                      juce::Rectangle<int> bounds,
                                                      const juce::String& title,
                                                      juce::Colour accentOverride)
{
    const juce::Colour accent = accentOverride.isTransparent()
                              ? juce::Colour (AlphaLAF::colAccentDim)
                              : accentOverride;

    g.setColour (juce::Colour (AlphaLAF::colPanel));
    g.fillRoundedRectangle (bounds.toFloat(), 6.0f);
    g.setColour (juce::Colour (AlphaLAF::colBorder));
    g.drawRoundedRectangle (bounds.toFloat(), 6.0f, 1.0f);

    auto bar = bounds.removeFromTop (20).toFloat();
    g.setColour (accent.withAlpha (0.5f));
    g.fillRoundedRectangle (bar, 6.0f);
    g.fillRect (bar.getX(), bar.getBottom() - 6.0f, bar.getWidth(), 6.0f);

    g.setColour (juce::Colour (AlphaLAF::colAccentGlow));
    g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
    g.drawText (title, bar.toNearestInt(), juce::Justification::centred, false);
}

//==============================================================================
void GainMonoAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (AlphaLAF::colBg));

    // Subtle scanlines
    g.setColour (juce::Colours::black.withAlpha (0.07f));
    for (int y = 0; y < getHeight(); y += 2)
        g.drawHorizontalLine (y, 0.0f, (float)getWidth());

    // Plugin title
    g.setColour (juce::Colour (AlphaLAF::colAccent));
    g.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    g.drawText ("Balancer Adjust", headerRect, juce::Justification::centredLeft, false);

    // Version / brand
    g.setColour (juce::Colour (AlphaLAF::colTextDim));
    g.setFont (juce::FontOptions (9.5f));
    g.drawText ("v3.0  |  AlphaAudio  |  William Ashley Music",
                headerRect, juce::Justification::centredRight, false);

    // Section panels
    drawSectionPanel (g, monitorSection,    "MONITOR / GAIN");
    drawSectionPanel (g, sideFilterSection, "SIDE FILTERS",
                      juce::Colour (AlphaLAF::colSide).darker (0.3f));
    drawSectionPanel (g, bassSection,       "BASS FOCUS",
                      juce::Colour (AlphaLAF::colMid).darker (0.3f));
    drawSectionPanel (g, eqSection,         "M/S EQ MATRIX");

    // ── Monitor section labels ─────────────────────────────────────────────
    {
        auto inner = monitorSection.reduced (8).withTrimmedTop (20);
        g.setColour (juce::Colour (AlphaLAF::colTextDim));
        g.setFont (juce::FontOptions (9.5f));

        // Gain label (below vertical fader)
        auto gainR  = inner.removeFromLeft (gainSlider.getWidth());
        g.drawText ("Gain", gainR.removeFromBottom (12), juce::Justification::centred, false);

        inner.removeFromLeft (6);
        // Side Gain label
        auto sgArea = juce::Rectangle<int> (sideGainSlider.getBounds());
        g.drawText ("Width", sgArea.removeFromBottom (12), juce::Justification::centred, false);
    }

    // ── Side filter labels ─────────────────────────────────────────────────
    {
        g.setColour (juce::Colour (AlphaLAF::colTextDim));
        g.setFont (juce::FontOptions (9.5f));
        auto hpfKnob = sideHPFSlider.getBounds().toFloat();
        g.drawText ("HPF Freq", hpfKnob.removeFromBottom (12).toNearestInt(),
                    juce::Justification::centred, false);
        auto lpfKnob = sideLPFSlider.getBounds().toFloat();
        g.drawText ("LPF Freq", lpfKnob.removeFromBottom (12).toNearestInt(),
                    juce::Justification::centred, false);
    }

    // ── Bass Focus label ───────────────────────────────────────────────────
    {
        g.setColour (juce::Colour (AlphaLAF::colTextDim));
        g.setFont (juce::FontOptions (9.5f));
        auto bfKnob = bassFreqSlider.getBounds().toFloat();
        g.drawText ("Crossover", bfKnob.removeFromBottom (12).toNearestInt(),
                    juce::Justification::centred, false);
    }

    // ── EQ column headers ─────────────────────────────────────────────────
    {
        if (!eqRows[0].isVisible()) return;

        auto row0 = eqRows[0].getBounds();

        // Replicate the same column positions as EQBandRow::resized()
        auto r    = row0.reduced (2, 0);
        int  onW  = 20, typeW = 62, freqW = 58, gainW = 50, qW = 44, tgtW = 62;
        int  gap4 = 4,  gap2 = 2;

        auto onR   = r.removeFromLeft (onW);    juce::ignoreUnused (onR);
        r.removeFromLeft (gap4);
        auto typeR = r.removeFromLeft (typeW);
        r.removeFromLeft (gap4);
        auto freqR = r.removeFromLeft (freqW);
        r.removeFromLeft (gap2);
        auto gainR = r.removeFromLeft (gainW);
        r.removeFromLeft (gap2);
        auto qR    = r.removeFromLeft (qW);
        r.removeFromLeft (gap4);
        auto tgtR  = r.removeFromLeft (tgtW);

        const int hdrY = eqSection.getY() + 20 + 2;
        const int hdrH = 12;

        g.setColour (juce::Colour (AlphaLAF::colTextDim));
        g.setFont (juce::FontOptions (9.0f));
        g.drawText ("Type",   typeR.withY (hdrY).withHeight (hdrH), juce::Justification::centred, false);
        g.drawText ("Freq",   freqR.withY (hdrY).withHeight (hdrH), juce::Justification::centred, false);
        g.drawText ("Gain",   gainR.withY (hdrY).withHeight (hdrH), juce::Justification::centred, false);
        g.drawText ("Q",      qR   .withY (hdrY).withHeight (hdrH), juce::Justification::centred, false);
        g.drawText ("Target", tgtR .withY (hdrY).withHeight (hdrH), juce::Justification::centred, false);

        // Band labels on the left
        const int bandH = eqRows[0].getHeight();
        for (int i = 0; i < 3; ++i)
        {
            const int bandY = eqRows[i].getY() + bandH / 2 - 6;
            g.setColour (juce::Colour (AlphaLAF::colAccentDim));
            g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
            g.drawText (juce::String (i + 1),
                        eqSection.getX() + 3, bandY, 12, 12,
                        juce::Justification::centred, false);
        }

        // Alternate row tint
        for (int i = 0; i < 3; ++i)
        {
            if (i % 2 == 1)
            {
                g.setColour (juce::Colours::black.withAlpha (0.12f));
                g.fillRect (eqRows[i].getBounds().withLeft (eqSection.getX() + 1)
                                                 .withRight (eqSection.getRight() - 1));
            }
        }
    }
}

//==============================================================================
void GainMonoAudioProcessorEditor::resized()
{
    const int pad    = 10;
    const int margin = 7;

    auto area = getLocalBounds().reduced (pad);

    //── Top bar: header + preset box + bypass ──────────────────────────────
    auto topBar = area.removeFromTop (26);
    headerRect = topBar.removeFromLeft (topBar.getWidth() / 2);
    bypassToggle.setBounds (topBar.removeFromRight (80).withSizeKeepingCentre (80, 22));
    presetBox   .setBounds (topBar.reduced (2, 2));
    area.removeFromTop (margin);

    //── Upper row: Monitor | Side Filters | Bass Focus ──────────────────────
    const int upperH   = 130;
    auto upperRow      = area.removeFromTop (upperH);
    area.removeFromTop (margin);

    const int monW  = 200;
    const int sfW   = 180;
    const int bassW = upperRow.getWidth() - monW - sfW - margin * 2;

    monitorSection    = upperRow.removeFromLeft (monW);
    upperRow.removeFromLeft (margin);
    sideFilterSection = upperRow.removeFromLeft (sfW);
    upperRow.removeFromLeft (margin);
    bassSection       = upperRow;

    //── Monitor section internals ───────────────────────────────────────────
    {
        auto inner = monitorSection.reduced (8).withTrimmedTop (22);

        // Left: vertical gain fader (full height)
        auto gainCol = inner.removeFromLeft (36);
        gainSlider.setBounds (gainCol.withTrimmedBottom (14));  // 14px for label
        inner.removeFromLeft (6);

        // Right side: dropdowns + side gain knob
        const int boxH = 22;
        auto right = inner;

        modeBox   .setBounds (right.removeFromTop (boxH));
        right.removeFromTop (5);
        sumLawBox .setBounds (right.removeFromTop (boxH));
        right.removeFromTop (8);

        // Side gain knob
        const int sgSize = juce::jmin (right.getWidth(), right.getHeight() - 12);
        sideGainSlider.setBounds (right.removeFromTop (sgSize)
                                       .withWidth (sgSize)
                                       .withX (right.getX() + (right.getWidth() - sgSize) / 2
                                               + monitorSection.getX() + 8 + 36 + 6));
    }

    //── Side Filter section internals ───────────────────────────────────────
    {
        auto inner = sideFilterSection.reduced (8).withTrimmedTop (22);

        const int knobSize = 52;
        const int toggleH  = 18;

        // HPF side
        auto hpfCol = inner.removeFromLeft (inner.getWidth() / 2 - 2);
        sideHPFToggle.setBounds (hpfCol.removeFromTop (toggleH));
        hpfCol.removeFromTop (4);
        sideHPFSlider.setBounds (hpfCol.removeFromTop (knobSize + 16)
                                        .withWidth (knobSize)
                                        .withX (hpfCol.getX() + (hpfCol.getWidth() - knobSize) / 2));

        inner.removeFromLeft (4); // gap

        // LPF side
        auto lpfCol = inner;
        sideLPFToggle.setBounds (lpfCol.removeFromTop (toggleH));
        lpfCol.removeFromTop (4);
        sideLPFSlider.setBounds (lpfCol.removeFromTop (knobSize + 16)
                                        .withWidth (knobSize)
                                        .withX (lpfCol.getX() + (lpfCol.getWidth() - knobSize) / 2));
    }

    //── Bass Focus section internals ────────────────────────────────────────
    {
        auto inner = bassSection.reduced (8).withTrimmedTop (22);

        const int knobSize = 52;
        const int toggleH  = 18;

        bassMonoToggle.setBounds (inner.removeFromTop (toggleH));
        inner.removeFromTop (4);
        bassFreqSlider.setBounds (inner.removeFromTop (knobSize + 16)
                                       .withWidth (knobSize)
                                       .withX (inner.getX() + (inner.getWidth() - knobSize) / 2));
    }

    //── M/S EQ Matrix ───────────────────────────────────────────────────────
    eqSection = area;

    {
        auto inner = eqSection.reduced (6).withTrimmedTop (20 + 14); // title bar + column headers

        // Each row: knob height + textbox = ~68px, with small gaps
        const int rowH  = 68;
        const int rowGap = 3;

        for (int i = 0; i < 3; ++i)
        {
            eqRows[i].setBounds (inner.removeFromTop (rowH));
            inner.removeFromTop (rowGap);
        }
    }
}

//==============================================================================
void GainMonoAudioProcessorEditor::timerCallback()
{
    // Sync preset box if host changed the program externally
    const int hostProg = audioProcessor.getCurrentProgram();
    if (presetBox.getSelectedId() != hostProg + 1)
        presetBox.setSelectedId (hostProg + 1, juce::dontSendNotification);

    repaint();
}
