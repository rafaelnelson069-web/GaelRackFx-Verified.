#pragma once
#include <JuceHeader.h>

class GaelRackFxAudioProcessor final : public juce::AudioProcessor
{
public:
    GaelRackFxAudioProcessor();
    ~GaelRackFxAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 7; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    int currentProgram = 0;
    juce::dsp::IIR::Filter<float> lowShelfL, lowShelfR, highShelfL, highShelfR;
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Limiter<float> limiter;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GaelRackFxAudioProcessor)
};
