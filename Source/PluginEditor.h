#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// AlphaAudio Look & Feel — dark charcoal / amber-gold brand palette
//==============================================================================
class AlphaLAF : public juce::LookAndFeel_V4
{
public:
    // ── Palette ────────────────────────────────────────────────────────────
    static constexpr uint32_t colBg         = 0xff1a1a20u;   // Deep charcoal
    static constexpr uint32_t colPanel      = 0xff22222au;   // Panel bg
    static constexpr uint32_t colPanelAlt   = 0xff1e1e26u;   // EQ row alternate
    static constexpr uint32_t colBorder     = 0xff38383fu;   // Subtle border
    static constexpr uint32_t colAccent     = 0xffcf9b3au;   // Amber gold
    static constexpr uint32_t colAccentDim  = 0xff8a6420u;   // Dim amber header
    static constexpr uint32_t colAccentGlow = 0xffffe08au;   // Bright highlight
    static constexpr uint32_t colText       = 0xffe8e0ccu;   // Warm off-white
    static constexpr uint32_t colTextDim    = 0xff706858u;   // Dim label
    static constexpr uint32_t colThumb      = 0xffe0c060u;   // Knob/thumb
    static constexpr uint32_t colTrack      = 0xff35353eu;   // Slider track
    static constexpr uint32_t colMid        = 0xff4a8fc4u;   // Mid channel indicator (blue)
    static constexpr uint32_t colSide       = 0xff59b88cu;   // Side channel indicator (teal)

    AlphaLAF()
    {
        setColour (juce::ComboBox::backgroundColourId,     juce::Colour (colPanel));
        setColour (juce::ComboBox::outlineColourId,        juce::Colour (colBorder));
        setColour (juce::ComboBox::textColourId,           juce::Colour (colText));
        setColour (juce::ComboBox::arrowColourId,          juce::Colour (colAccent));
        setColour (juce::PopupMenu::backgroundColourId,    juce::Colour (colPanel));
        setColour (juce::PopupMenu::textColourId,          juce::Colour (colText));
        setColour (juce::PopupMenu::highlightedBackgroundColourId,
                   juce::Colour (colAccent).withAlpha (0.4f));

        setColour (juce::ToggleButton::textColourId,       juce::Colour (colText));
        setColour (juce::ToggleButton::tickColourId,       juce::Colour (colAccent));
        setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (colTextDim));

        setColour (juce::Slider::thumbColourId,            juce::Colour (colThumb));
        setColour (juce::Slider::trackColourId,            juce::Colour (colTrack));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (colAccent));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (colTrack));
        setColour (juce::Slider::backgroundColourId,       juce::Colour (colTrack));
        setColour (juce::Slider::textBoxTextColourId,      juce::Colour (colText));
        setColour (juce::Slider::textBoxBackgroundColourId,juce::Colour (colBg));
        setColour (juce::Slider::textBoxOutlineColourId,   juce::Colour (colBorder));
        setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (colAccent).withAlpha (0.5f));

        setColour (juce::Label::textColourId,              juce::Colour (colText));
    }

    //==========================================================================
    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float startAngle, float endAngle,
                           juce::Slider& /*s*/) override
    {
        const float radius  = juce::jmin (width / 2.0f, height / 2.0f) - 4.0f;
        const float cx      = x + width  * 0.5f;
        const float cy      = y + height * 0.5f;

        juce::Path track;
        track.addArc (cx - radius, cy - radius, radius * 2.0f, radius * 2.0f,
                      startAngle, endAngle, true);
        g.setColour (juce::Colour (colTrack));
        g.strokePath (track, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));

        const float angle = startAngle + sliderPos * (endAngle - startAngle);
        juce::Path value;
        value.addArc (cx - radius, cy - radius, radius * 2.0f, radius * 2.0f,
                      startAngle, angle, true);
        g.setColour (juce::Colour (colAccent));
        g.strokePath (value, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));

        const float kr = radius * 0.65f;
        g.setColour (juce::Colour (0xff2a2a34));
        g.fillEllipse (cx - kr, cy - kr, kr * 2.0f, kr * 2.0f);
        g.setColour (juce::Colour (colBorder));
        g.drawEllipse (cx - kr, cy - kr, kr * 2.0f, kr * 2.0f, 1.2f);

        const float dotR = 2.5f;
        const float dotD = kr * 0.62f;
        g.setColour (juce::Colour (colAccentGlow));
        g.fillEllipse (cx + dotD * std::sin (angle) - dotR,
                       cy - dotD * std::cos (angle) - dotR, dotR * 2.0f, dotR * 2.0f);
    }

    //==========================================================================
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float /*minPos*/, float /*maxPos*/,
                           const juce::Slider::SliderStyle style, juce::Slider& /*s*/) override
    {
        const bool vert    = (style == juce::Slider::LinearVertical);
        const float trackW = 4.0f;
        const float thumbW = 14.0f;
        const float thumbH = 22.0f;
        const float cx     = x + width  * 0.5f;
        const float cy     = y + height * 0.5f;

        if (vert)
        {
            g.setColour (juce::Colour (colTrack));
            g.fillRoundedRectangle (cx - trackW * 0.5f, (float)y, trackW, (float)height, 2.0f);

            const float zero = juce::jmap (0.5f, 0.0f, 1.0f, (float)(y + height), (float)y);
            const float top  = juce::jmin (sliderPos, zero);
            const float bot  = juce::jmax (sliderPos, zero);
            g.setColour (juce::Colour (colAccent));
            g.fillRoundedRectangle (cx - trackW * 0.5f, top, trackW, bot - top, 2.0f);

            g.setColour (juce::Colour (colThumb));
            g.fillRoundedRectangle (cx - thumbW * 0.5f, sliderPos - thumbH * 0.5f, thumbW, thumbH, 4.0f);
            g.setColour (juce::Colour (colBorder));
            g.drawRoundedRectangle (cx - thumbW * 0.5f, sliderPos - thumbH * 0.5f, thumbW, thumbH, 4.0f, 1.0f);
        }
        else
        {
            g.setColour (juce::Colour (colTrack));
            g.fillRoundedRectangle ((float)x, cy - trackW * 0.5f, (float)width, trackW, 2.0f);
            g.setColour (juce::Colour (colAccent));
            g.fillRoundedRectangle ((float)x, cy - trackW * 0.5f, sliderPos - (float)x, trackW, 2.0f);
            g.setColour (juce::Colour (colThumb));
            g.fillRoundedRectangle (sliderPos - thumbH * 0.5f, cy - thumbW * 0.5f, thumbH, thumbW, 4.0f);
            g.setColour (juce::Colour (colBorder));
            g.drawRoundedRectangle (sliderPos - thumbH * 0.5f, cy - thumbW * 0.5f, thumbH, thumbW, 4.0f, 1.0f);
        }
    }

    //==========================================================================
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& btn,
                           bool highlighted, bool /*down*/) override
    {
        const float ledR  = 5.5f;
        const float ledX  = 7.0f;
        const float ledY  = btn.getHeight() * 0.5f;
        const bool  on    = btn.getToggleState();
        const bool  isRed = (btn.getName() == "bypass");

        juce::Colour ledCol = on
            ? (isRed ? juce::Colour (0xffee4444u) : juce::Colour (colAccent))
            : juce::Colour (colTrack);
        if (highlighted) ledCol = ledCol.brighter (0.2f);

        if (on)
        {
            g.setColour (ledCol.withAlpha (0.22f));
            g.fillEllipse (ledX - ledR * 1.8f, ledY - ledR * 1.8f, ledR * 3.6f, ledR * 3.6f);
        }
        g.setColour (ledCol);
        g.fillEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);
        g.setColour (juce::Colour (colBorder));
        g.drawEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f, 1.0f);

        g.setColour (on ? juce::Colour (colText) : juce::Colour (colTextDim));
        g.setFont (juce::FontOptions (11.0f));
        g.drawText (btn.getButtonText(),
                    (int)(ledX + ledR + 5), 0,
                    btn.getWidth() - (int)(ledX + ledR + 5), btn.getHeight(),
                    juce::Justification::centredLeft, false);
    }

    //==========================================================================
    void drawComboBox (juce::Graphics& g, int width, int height,
                       bool /*isButtonDown*/, int /*buttonX*/, int /*buttonY*/,
                       int /*buttonW*/, int /*buttonH*/, juce::ComboBox& box) override
    {
        g.setColour (juce::Colour (colPanel));
        g.fillRoundedRectangle (0.0f, 0.0f, (float)width, (float)height, 3.0f);
        g.setColour (juce::Colour (colBorder));
        g.drawRoundedRectangle (0.5f, 0.5f, (float)width - 1.0f, (float)height - 1.0f, 3.0f, 1.0f);

        // Arrow
        const float arrowX = width - 14.0f;
        const float arrowY = height * 0.5f;
        juce::Path arrow;
        arrow.addTriangle (arrowX, arrowY - 3.0f, arrowX + 6.0f, arrowY - 3.0f, arrowX + 3.0f, arrowY + 3.0f);
        g.setColour (juce::Colour (colAccent));
        g.fillPath (arrow);

        juce::ignoreUnused (box);
    }
};

//==============================================================================
// EQ Band Row — one horizontal strip with all controls for one band
//==============================================================================
class EQBandRow : public juce::Component
{
public:
    EQBandRow() = default;

    // All members are public so the editor can lay them out directly
    juce::ToggleButton onToggle   { "" };
    juce::ComboBox     typeBox;
    juce::Slider       freqSlider;
    juce::Slider       gainSlider;
    juce::Slider       qSlider;
    juce::ComboBox     targetBox;

    void init (int bandIndex)
    {
        bandIdx = bandIndex;

        typeBox.addItem ("Bell",     1);
        typeBox.addItem ("HPF",      2);
        typeBox.addItem ("LPF",      3);
        typeBox.addItem ("Hi Shelf", 4);
        typeBox.addItem ("Lo Shelf", 5);
        typeBox.setSelectedId (1, juce::dontSendNotification);
        addAndMakeVisible (typeBox);

        freqSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        freqSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 14);
        freqSlider.setTextValueSuffix (" Hz");
        addAndMakeVisible (freqSlider);

        gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 14);
        gainSlider.setTextValueSuffix (" dB");
        addAndMakeVisible (gainSlider);

        qSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        qSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 42, 14);
        addAndMakeVisible (qSlider);

        targetBox.addItem ("Stereo", 1);
        targetBox.addItem ("Mid",    2);
        targetBox.addItem ("Side",   3);
        targetBox.setSelectedId (1, juce::dontSendNotification);
        addAndMakeVisible (targetBox);

        onToggle.setName ("eqon");
        addAndMakeVisible (onToggle);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (2, 0);

        // Layout: [ON 20] | [Type 62] | [Freq 58] | [Gain 50] | [Q 44] | [Target 62]
        onToggle .setBounds (r.removeFromLeft (20).withSizeKeepingCentre (20, 20));
        r.removeFromLeft (4);
        typeBox  .setBounds (r.removeFromLeft (62).withSizeKeepingCentre (62, 22));
        r.removeFromLeft (4);
        freqSlider.setBounds (r.removeFromLeft (58));
        r.removeFromLeft (2);
        gainSlider.setBounds (r.removeFromLeft (50));
        r.removeFromLeft (2);
        qSlider   .setBounds (r.removeFromLeft (44));
        r.removeFromLeft (4);
        targetBox .setBounds (r.removeFromLeft (62).withSizeKeepingCentre (62, 22));
    }

private:
    int bandIdx = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQBandRow)
};

//==============================================================================
// Main Editor
//==============================================================================
class GainMonoAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    explicit GainMonoAudioProcessorEditor (GainMonoAudioProcessor&);
    ~GainMonoAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized () override;
    void timerCallback () override;

private:
    void drawSectionPanel (juce::Graphics&, juce::Rectangle<int> bounds,
                           const juce::String& title,
                           juce::Colour accentOverride = juce::Colour (0u));

    //==========================================================================
    GainMonoAudioProcessor& audioProcessor;
    AlphaLAF alphaLAF;

    // ── Top bar ──────────────────────────────────────────────────────────────
    juce::ToggleButton bypassToggle  { "BYPASS" };
    juce::ComboBox     presetBox;

    // ── Monitor section ───────────────────────────────────────────────────────
    juce::ComboBox     modeBox;
    juce::ComboBox     sumLawBox;
    juce::Slider       gainSlider;
    juce::Slider       sideGainSlider;

    // ── Side Filter section ───────────────────────────────────────────────────
    juce::ToggleButton sideHPFToggle  { "HPF" };
    juce::Slider       sideHPFSlider;
    juce::ToggleButton sideLPFToggle  { "LPF" };
    juce::Slider       sideLPFSlider;

    // ── Bass Focus (legacy) ───────────────────────────────────────────────────
    juce::ToggleButton bassMonoToggle { "Mono Bass" };
    juce::Slider       bassFreqSlider;

    // ── M/S EQ Bands ─────────────────────────────────────────────────────────
    EQBandRow eqRows[3];

    // ── APVTS Attachments ────────────────────────────────────────────────────
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SA> gainAtt, sideGainAtt, sideHPFAtt, sideLPFAtt, bassFreqAtt;
    std::unique_ptr<BA> bypassAtt, sideHPFOnAtt, sideLPFOnAtt, bassMonoAtt;
    std::unique_ptr<CA> modeAtt, sumLawAtt;

    std::unique_ptr<BA> eqOnAtt    [3];
    std::unique_ptr<CA> eqTypeAtt  [3];
    std::unique_ptr<SA> eqFreqAtt  [3];
    std::unique_ptr<SA> eqGainAtt  [3];
    std::unique_ptr<SA> eqQAtt     [3];
    std::unique_ptr<CA> eqTargetAtt[3];

    // Layout rects (set in resized, used in paint)
    juce::Rectangle<int> headerRect;
    juce::Rectangle<int> monitorSection;
    juce::Rectangle<int> sideFilterSection;
    juce::Rectangle<int> bassSection;
    juce::Rectangle<int> eqSection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainMonoAudioProcessorEditor)
};
