#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Factory Presets
// Each preset is a minimal APVTS XML patch. Parameters not listed stay at default.
//==============================================================================
static const char* kPresetDefault = R"(
<PARAMS gainDb="0.0" bypass="0" mode="0" sumLaw="0" sideGain="0.0"
        sideHPFOn="0" sideHPF="80.0" sideLPFOn="0" sideLPF="16000.0"
        bassMono="0" bassFreq="120.0"
        eqOn0="0" eqType0="0" eqFreq0="1000.0" eqGain0="0.0" eqQ0="0.707" eqTarget0="0"
        eqOn1="0" eqType1="0" eqFreq1="1000.0" eqGain1="0.0" eqQ1="0.707" eqTarget1="0"
        eqOn2="0" eqType2="0" eqFreq2="1000.0" eqGain2="0.0" eqQ2="0.707" eqTarget2="0"/>
)";

// 1. EDM / Synthwave Mud Cleanup
static const char* kPresetEDM = R"(
<PARAMS gainDb="0.0" bypass="0" mode="0" sumLaw="0" sideGain="0.0"
        sideHPFOn="1" sideHPF="120.0" sideLPFOn="0" sideLPF="16000.0"
        bassMono="0" bassFreq="120.0"
        eqOn0="1" eqType0="0" eqFreq0="300.0" eqGain0="-2.0" eqQ0="1.0" eqTarget0="2"
        eqOn1="0" eqType1="0" eqFreq1="1000.0" eqGain1="0.0" eqQ1="0.707" eqTarget1="0"
        eqOn2="0" eqType2="0" eqFreq2="1000.0" eqGain2="0.0" eqQ2="0.707" eqTarget2="0"/>
)";

// 2. Vocal Clarity / De-Boxing
static const char* kPresetVocal = R"(
<PARAMS gainDb="0.0" bypass="0" mode="0" sumLaw="0" sideGain="0.0"
        sideHPFOn="0" sideHPF="80.0" sideLPFOn="0" sideLPF="16000.0"
        bassMono="0" bassFreq="120.0"
        eqOn0="1" eqType0="0" eqFreq0="300.0" eqGain0="-1.5" eqQ0="1.2" eqTarget0="1"
        eqOn1="0" eqType1="0" eqFreq1="1000.0" eqGain1="0.0" eqQ1="0.707" eqTarget1="0"
        eqOn2="0" eqType2="0" eqFreq2="1000.0" eqGain2="0.0" eqQ2="0.707" eqTarget2="0"/>
)";

// 3. Reverb Tamer / De-Clouding
static const char* kPresetReverb = R"(
<PARAMS gainDb="0.0" bypass="0" mode="0" sumLaw="0" sideGain="0.0"
        sideHPFOn="1" sideHPF="250.0" sideLPFOn="0" sideLPF="16000.0"
        bassMono="0" bassFreq="120.0"
        eqOn0="1" eqType0="3" eqFreq0="6000.0" eqGain0="-2.0" eqQ0="0.707" eqTarget0="2"
        eqOn1="0" eqType1="0" eqFreq1="1000.0" eqGain1="0.0" eqQ1="0.707" eqTarget1="0"
        eqOn2="0" eqType2="0" eqFreq2="1000.0" eqGain2="0.0" eqQ2="0.707" eqTarget2="0"/>
)";

// 4. Vinyl Mastering Prep
static const char* kPresetVinyl = R"(
<PARAMS gainDb="0.0" bypass="0" mode="1" sumLaw="0" sideGain="0.0"
        sideHPFOn="1" sideHPF="150.0" sideLPFOn="1" sideLPF="16000.0"
        bassMono="0" bassFreq="120.0"
        eqOn0="0" eqType0="0" eqFreq0="1000.0" eqGain0="0.0" eqQ0="0.707" eqTarget0="0"
        eqOn1="0" eqType1="0" eqFreq1="1000.0" eqGain1="0.0" eqQ1="0.707" eqTarget1="0"
        eqOn2="0" eqType2="0" eqFreq2="1000.0" eqGain2="0.0" eqQ2="0.707" eqTarget2="0"/>
)";

// 5. Safe Width Booster
static const char* kPresetWidth = R"(
<PARAMS gainDb="0.0" bypass="0" mode="0" sumLaw="0" sideGain="1.5"
        sideHPFOn="1" sideHPF="100.0" sideLPFOn="0" sideLPF="16000.0"
        bassMono="0" bassFreq="120.0"
        eqOn0="1" eqType0="0" eqFreq0="400.0" eqGain0="-1.5" eqQ0="1.0" eqTarget0="2"
        eqOn1="0" eqType1="0" eqFreq1="1000.0" eqGain1="0.0" eqQ1="0.707" eqTarget1="0"
        eqOn2="0" eqType2="0" eqFreq2="1000.0" eqGain2="0.0" eqQ2="0.707" eqTarget2="0"/>
)";

const std::vector<BalancerPreset>& GainMonoAudioProcessor::getPresets()
{
    static const std::vector<BalancerPreset> presets =
    {
        { "Default",                    kPresetDefault },
        { "EDM / Synthwave Mud Cleanup",kPresetEDM     },
        { "Vocal Clarity / De-Boxing",  kPresetVocal   },
        { "Reverb Tamer / De-Clouding", kPresetReverb  },
        { "Vinyl Mastering Prep",       kPresetVinyl   },
        { "Safe Width Booster",         kPresetWidth   },
    };
    return presets;
}

//==============================================================================
// Parameter Layout
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GainMonoAudioProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    // Master
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::gainDb, "Gain (dB)",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.01f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::bypass, "Bypass", false));

    // Monitor
    p.push_back(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs::mode, "Monitor Mode",
        juce::StringArray{ "Stereo", "Solo Mid", "Solo Side" }, 0));

    p.push_back(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs::sumLaw, "Summing Law",
        juce::StringArray{ "-6dB (Voltage)", "-3dB (Power)" }, 0));

    // Side Gain
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::sideGain, "Side Gain (dB)",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));

    // Side HPF / LPF
    p.push_back(std::make_unique<juce::AudioParameterBool>(ParamIDs::sideHPFOn, "Side HPF On", false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::sideHPF, "Side HPF (Hz)",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.4f), 80.0f));

    p.push_back(std::make_unique<juce::AudioParameterBool>(ParamIDs::sideLPFOn, "Side LPF On", false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::sideLPF, "Side LPF (Hz)",
        juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f, 0.4f), 16000.0f));

    // Bass Focus (legacy quick toggle)
    p.push_back(std::make_unique<juce::AudioParameterBool>(ParamIDs::bassMono, "Bass Mono", false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::bassFreq, "Bass Freq (Hz)",
        juce::NormalisableRange<float>(20.0f, 300.0f, 1.0f, 0.5f), 120.0f));

    // 3 M/S EQ Bands
    for (int i = 0; i < 3; ++i)
    {
        p.push_back(std::make_unique<juce::AudioParameterBool>(
            ParamIDs::eqBandOn[i], juce::String("EQ") + juce::String(i) + " On", false));

        p.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::eqBandType[i], juce::String("EQ") + juce::String(i) + " Type",
            juce::StringArray{ "Bell", "HPF", "LPF", "Hi Shelf", "Lo Shelf" }, 0));

        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::eqBandFreq[i], juce::String("EQ") + juce::String(i) + " Freq",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 1000.0f));

        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::eqBandGain[i], juce::String("EQ") + juce::String(i) + " Gain",
            juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));

        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::eqBandQ[i], juce::String("EQ") + juce::String(i) + " Q",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 0.707f));

        p.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::eqBandTarget[i], juce::String("EQ") + juce::String(i) + " Target",
            juce::StringArray{ "Stereo", "Mid", "Side" }, 0));
    }

    return { p.begin(), p.end() };
}

//==============================================================================
GainMonoAudioProcessor::GainMonoAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "PARAMS", createLayout())
{
    smoothedGain.reset(44100.0, 0.02);
    smoothedSideGain.reset(44100.0, 0.02);

    // Register listeners for filter parameters
    apvts.addParameterListener(ParamIDs::sideHPF, this);
    apvts.addParameterListener(ParamIDs::sideLPF, this);
    apvts.addParameterListener(ParamIDs::bassFreq, this);

    for (int i = 0; i < 3; ++i)
    {
        apvts.addParameterListener(ParamIDs::eqBandType[i], this);
        apvts.addParameterListener(ParamIDs::eqBandFreq[i], this);
        apvts.addParameterListener(ParamIDs::eqBandGain[i], this);
        apvts.addParameterListener(ParamIDs::eqBandQ[i], this);
    }
}

GainMonoAudioProcessor::~GainMonoAudioProcessor()
{
    apvts.removeParameterListener(ParamIDs::sideHPF, this);
    apvts.removeParameterListener(ParamIDs::sideLPF, this);
    apvts.removeParameterListener(ParamIDs::bassFreq, this);

    for (int i = 0; i < 3; ++i)
    {
        apvts.removeParameterListener(ParamIDs::eqBandType[i], this);
        apvts.removeParameterListener(ParamIDs::eqBandFreq[i], this);
        apvts.removeParameterListener(ParamIDs::eqBandGain[i], this);
        apvts.removeParameterListener(ParamIDs::eqBandQ[i], this);
    }
}

//==============================================================================
void GainMonoAudioProcessor::parameterChanged(const juce::String& id, float /*newValue*/)
{
    if (id == ParamIDs::sideHPF) { rebuildSideHPF();    return; }
    if (id == ParamIDs::sideLPF) { rebuildSideLPF();    return; }
    if (id == ParamIDs::bassFreq) { rebuildBassFilter(); return; }

    for (int i = 0; i < 3; ++i)
        if (id == ParamIDs::eqBandType[i] || id == ParamIDs::eqBandFreq[i] ||
            id == ParamIDs::eqBandGain[i] || id == ParamIDs::eqBandQ[i])
        {
            rebuildEQBand(i);
            return;
        }
}

//==============================================================================
// Filter coefficient builders
//==============================================================================
void GainMonoAudioProcessor::rebuildSideHPF()
{
    const float fc = apvts.getRawParameterValue(ParamIDs::sideHPF)->load();
    auto c = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, fc, 0.707f);
    *sideHPFFilter.coefficients = *c;
}

void GainMonoAudioProcessor::rebuildSideLPF()
{
    const float fc = apvts.getRawParameterValue(ParamIDs::sideLPF)->load();
    auto c = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, fc, 0.707f);
    *sideLPFFilter.coefficients = *c;
}

void GainMonoAudioProcessor::rebuildBassFilter()
{
    const float fc = apvts.getRawParameterValue(ParamIDs::bassFreq)->load();
    auto c = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, fc);
    *lpfSide.coefficients = *c;
}

void GainMonoAudioProcessor::rebuildEQBand(int i)
{
    const int   type = static_cast<int> (apvts.getRawParameterValue(ParamIDs::eqBandType[i])->load());
    const float freq = apvts.getRawParameterValue(ParamIDs::eqBandFreq[i])->load();
    const float gain = apvts.getRawParameterValue(ParamIDs::eqBandGain[i])->load();
    const float q = apvts.getRawParameterValue(ParamIDs::eqBandQ[i])->load();
    const float sr = (float)currentSampleRate;

    juce::dsp::IIR::Coefficients<float>::Ptr c;

    switch (static_cast<EQType> (type))
    {
    case EQType::Bell:
        c = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, freq, q,
            juce::Decibels::decibelsToGain(gain));
        break;
    case EQType::HPF:
        c = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, freq, q);
        break;
    case EQType::LPF:
        c = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, freq, q);
        break;
    case EQType::HiShelf:
        c = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sr, freq, q,
            juce::Decibels::decibelsToGain(gain));
        break;
    case EQType::LoShelf:
        c = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sr, freq, q,
            juce::Decibels::decibelsToGain(gain));
        break;
    default:
        c = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, freq, q,
            juce::Decibels::decibelsToGain(gain));
        break;
    }

    // Both Mid and Side filters share coefficients, separate state
    *eqMid[i].coefficients = *c;
    *eqSide[i].coefficients = *c;
}

//==============================================================================
bool GainMonoAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& in = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();
    if (in.isDisabled() || out.isDisabled()) return false;
    if (in != juce::AudioChannelSet::stereo() && in != juce::AudioChannelSet::mono()) return false;
    if (out != juce::AudioChannelSet::stereo() && out != juce::AudioChannelSet::mono()) return false;
    return true;
}

//==============================================================================
void GainMonoAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;
    smoothedGain.reset(sampleRate, 0.02);
    smoothedSideGain.reset(sampleRate, 0.02);

    rebuildSideHPF();
    rebuildSideLPF();
    rebuildBassFilter();
    for (int i = 0; i < 3; ++i) rebuildEQBand(i);

    sideHPFFilter.reset();
    sideLPFFilter.reset();
    lpfSide.reset();
    for (int i = 0; i < 3; ++i) { eqMid[i].reset(); eqSide[i].reset(); }
}

//==============================================================================
void GainMonoAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midi)
{
    juce::ignoreUnused(midi);
    const int numCh = buffer.getNumChannels();
    const int numSamp = buffer.getNumSamples();

    if (numCh < 2)
    {
        // Pure mono fallback
        auto* d = buffer.getWritePointer(0);
        smoothedGain.setTargetValue(
            juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamIDs::gainDb)->load()));
        for (int n = 0; n < numSamp; ++n)
            d[n] *= smoothedGain.getNextValue();
        return;
    }

    if (apvts.getRawParameterValue(ParamIDs::bypass)->load() > 0.5f)
        return;

    // Read all params once per block
    const int   mode = static_cast<int> (apvts.getRawParameterValue(ParamIDs::mode)->load());
    const int   sumLaw = static_cast<int> (apvts.getRawParameterValue(ParamIDs::sumLaw)->load());
    const bool  sideHPFOn = apvts.getRawParameterValue(ParamIDs::sideHPFOn)->load() > 0.5f;
    const bool  sideLPFOn = apvts.getRawParameterValue(ParamIDs::sideLPFOn)->load() > 0.5f;
    const bool  bassMono = apvts.getRawParameterValue(ParamIDs::bassMono)->load() > 0.5f;

    // EQ band on/target per block
    bool       eqOn[3];
    EQTarget   eqTarget[3];
    for (int i = 0; i < 3; ++i)
    {
        eqOn[i] = apvts.getRawParameterValue(ParamIDs::eqBandOn[i])->load() > 0.5f;
        eqTarget[i] = static_cast<EQTarget> (
            static_cast<int> (apvts.getRawParameterValue(ParamIDs::eqBandTarget[i])->load()));
    }

    const float targetGain = juce::Decibels::decibelsToGain(
        apvts.getRawParameterValue(ParamIDs::gainDb)->load());
    smoothedGain.setTargetValue(targetGain);

    const float targetSideGain = juce::Decibels::decibelsToGain(
        apvts.getRawParameterValue(ParamIDs::sideGain)->load());
    smoothedSideGain.setTargetValue(targetSideGain);

    const float monoScale = (sumLaw == 0) ? 0.5f : 0.70710678118f;

    auto* L = buffer.getWritePointer(0);
    auto* R = buffer.getWritePointer(1);

    for (int n = 0; n < numSamp; ++n)
    {
        float mid = (L[n] + R[n]) * 0.5f;
        float side = (L[n] - R[n]) * 0.5f;

        //─── 1. Bass Focus elliptical quick-toggle ────────────────────────
        if (bassMono)
        {
            float lowSide = lpfSide.processSample(side);
            float highSide = side - lowSide;
            side = highSide; // Mono-ise low end by discarding low-side content
        }

        //─── 2. Side HPF / LPF ───────────────────────────────────────────
        if (sideHPFOn) side = sideHPFFilter.processSample(side);
        if (sideLPFOn) side = sideLPFFilter.processSample(side);

        //─── 3. M/S EQ Bands ─────────────────────────────────────────────
        for (int i = 0; i < 3; ++i)
        {
            if (!eqOn[i]) continue;

            switch (eqTarget[i])
            {
            case EQTarget::Mid:
                mid = eqMid[i].processSample(mid);
                break;
            case EQTarget::Side:
                side = eqSide[i].processSample(side);
                break;
            case EQTarget::Stereo:
            default:
                mid = eqMid[i].processSample(mid);
                side = eqSide[i].processSample(side);
                break;
            }
        }

        //─── 4. Side Gain (width) ─────────────────────────────────────────
        side *= smoothedSideGain.getNextValue();

        //─── 5. Reconvert to L/R ─────────────────────────────────────────
        float outL = mid + side;
        float outR = mid - side;

        //─── 6. Monitor Mode ──────────────────────────────────────────────
        if (mode == 1)          // Solo Mid
        {
            outL = outR = (outL + outR) * monoScale;
        }
        else if (mode == 2)     // Solo Side
        {
            outL = outR = (outL - outR) * monoScale;
        }

        //─── 7. Master Gain ───────────────────────────────────────────────
        const float g = smoothedGain.getNextValue();
        L[n] = outL * g;
        R[n] = outR * g;
    }
}

//==============================================================================
// Preset Program management
//==============================================================================
void GainMonoAudioProcessor::setCurrentProgram(int index)
{
    const auto& presets = getPresets();
    if (index < 0 || index >= (int)presets.size()) return;

    currentPreset = index;

    // Parse the XML attribute list and apply to APVTS
    juce::String xml(presets[index].xmlState);
    // Wrap in a document root so XmlDocument parses it
    auto doc = juce::XmlDocument::parse("<root>" + xml.trim() + "</root>");
    if (doc == nullptr) return;

    auto* elem = doc->getFirstChildElement();
    if (elem == nullptr) return;

    // Walk attributes and set each APVTS parameter
    for (int a = 0; a < elem->getNumAttributes(); ++a)
    {
        const juce::String name = elem->getAttributeName(a);
        const juce::String value = elem->getAttributeValue(a);

        if (auto* param = apvts.getParameter(name))
            param->setValueNotifyingHost(
                param->getNormalisableRange().convertTo0to1(value.getFloatValue()));
    }
}

const juce::String GainMonoAudioProcessor::getProgramName(int index)
{
    const auto& presets = getPresets();
    if (index < 0 || index >= (int)presets.size()) return {};
    return presets[index].name;
}

//==============================================================================
void GainMonoAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void GainMonoAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, (size_t)sizeInBytes);
    if (tree.isValid()) apvts.replaceState(tree);
}

//==============================================================================
juce::AudioProcessorEditor* GainMonoAudioProcessor::createEditor()
{
    return new GainMonoAudioProcessorEditor(*this);
}