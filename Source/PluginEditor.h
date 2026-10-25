#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <vector>

class GaelRackFxAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit GaelRackFxAudioProcessorEditor (GaelRackFxAudioProcessor&);
    ~GaelRackFxAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    GaelRackFxAudioProcessor& processor;
    juce::Label title, subtitle, presetsLabel, micLabel;
    juce::ComboBox presets, microphone;
    juce::TextButton power { "ON" };
    juce::Slider input, mix, output, low, mid, high, compression, saturation, air, afro;
    std::vector<std::unique_ptr<SliderAttachment>> attachments;
    std::unique_ptr<ButtonAttachment> bypassAttachment;
    void styleSlider (juce::Slider&, juce::Colour);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GaelRackFxAudioProcessorEditor)
};
