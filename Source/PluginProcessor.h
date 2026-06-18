#pragma once
#include <JuceHeader.h>

//==============================================================================
// Parameter IDs — single source of truth for processor and editor.
//==============================================================================
namespace ParamIDs
{
    // Master
    static constexpr auto gainDb = "gainDb";
    static constexpr auto bypass = "bypass";

    // Monitor
    static constexpr auto mode = "mode";       // 0=Stereo, 1=Solo Mid, 2=Solo Side
    static constexpr auto sumLaw = "sumLaw";     // 0=-6dB, 1=-3dB

    // Side Gain (width control)
    static constexpr auto sideGain = "sideGain";   // dB, ±12

    // Side filter (replaces Bass Focus HPF/LPF)
    static constexpr auto sideHPF = "sideHPF";    // Hz
    static constexpr auto sideLPF = "sideLPF";    // Hz
    static constexpr auto sideHPFOn = "sideHPFOn";
    static constexpr auto sideLPFOn = "sideLPFOn";

    // Bass Focus mono toggle (legacy / elliptical quick-toggle)
    static constexpr auto bassMono = "bassMono";
    static constexpr auto bassFreq = "bassFreq";

    // M/S EQ Bands (3 bands)
    // Band index 0,1,2 → suffix "0","1","2"
    // Per band: eqType (0=Bell,1=HPF,2=LPF,3=HiShelf,4=LoShelf)
    //           eqFreq, eqGain, eqQ, eqTarget (0=Stereo,1=Mid,2=Side)
    static constexpr const char* eqBandType[3] = { "eqType0",   "eqType1",   "eqType2" };
    static constexpr const char* eqBandFreq[3] = { "eqFreq0",   "eqFreq1",   "eqFreq2" };
    static constexpr const char* eqBandGain[3] = { "eqGain0",   "eqGain1",   "eqGain2" };
    static constexpr const char* eqBandQ[3] = { "eqQ0",      "eqQ1",      "eqQ2" };
    static constexpr const char* eqBandTarget[3] = { "eqTarget0", "eqTarget1", "eqTarget2" };
    static constexpr const char* eqBandOn[3] = { "eqOn0",     "eqOn1",     "eqOn2" };
}

//==============================================================================
// EQ filter type and routing target enums (shared with editor)
//==============================================================================
enum class EQType { Bell = 0, HPF, LPF, HiShelf, LoShelf };
enum class EQTarget { Stereo = 0, Mid, Side };

//==============================================================================
// Preset definitions
//==============================================================================
struct BalancerPreset
{
    const char* name;
    const char* xmlState; // Full APVTS XML string
};

//==============================================================================
class GainMonoAudioProcessor : public juce::AudioProcessor,
    private juce::AudioProcessorValueTreeState::Listener
{
public:
    GainMonoAudioProcessor();
    ~GainMonoAudioProcessor() override;

    //==========================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==========================================================================
    const juce::String getName() const override { return "Balancer Adjust"; }
    bool acceptsMidi()   const override { return false; }
    bool producesMidi()  const override { return false; }
    bool isMidiEffect()  const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int  getNumPrograms()  override { return (int)getPresets().size(); }
    int  getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    //==========================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    //==========================================================================
    juce::AudioProcessorValueTreeState apvts;

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // Preset access for editor
    static const std::vector<BalancerPreset>& getPresets();

private:
    //==========================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    // Filter rebuild helpers
    void rebuildEQBand(int band);
    void rebuildSideHPF();
    void rebuildSideLPF();
    void rebuildBassFilter();

    // Inline DSP helpers
    float processEQSample(int band, float sample);

    //==========================================================================
    juce::LinearSmoothedValue<float> smoothedGain;
    juce::LinearSmoothedValue<float> smoothedSideGain;

    // Side HPF / LPF (explicit)
    using IIRFilter = juce::dsp::IIR::Filter<float>;
    IIRFilter sideHPFFilter;
    IIRFilter sideLPFFilter;

    // Bass Focus elliptical LPF (legacy)
    IIRFilter lpfSide;

    // M/S EQ: dual filters per band (one for Mid, one for Side — same coefficients,
    // separate state so M and S are processed independently).
    IIRFilter eqMid[3];
    IIRFilter eqSide[3];

    double currentSampleRate = 44100.0;
    int    currentPreset = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainMonoAudioProcessor)
};