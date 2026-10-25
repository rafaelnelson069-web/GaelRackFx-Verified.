#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    juce::StringArray presetNames { "Femme", "Homme", "Trap", "Afrobeat", "Salegy", "Slow", "Clean" };
}

GaelRackFxAudioProcessor::GaelRackFxAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout GaelRackFxAudioProcessor::createParameterLayout()
{
    using APF = juce::AudioParameterFloat;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back (std::make_unique<juce::AudioParameterBool> ("bypass", "Bypass", false));
    p.push_back (std::make_unique<APF> ("input", "Input", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f, "dB"));
    p.push_back (std::make_unique<APF> ("mix", "Mix", juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 100.0f, "%"));
    p.push_back (std::make_unique<APF> ("output", "Output", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f, "dB"));
    p.push_back (std::make_unique<APF> ("low", "EQ Low", juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 0.0f, "dB"));
    p.push_back (std::make_unique<APF> ("mid", "EQ Mid", juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 0.0f, "dB"));
    p.push_back (std::make_unique<APF> ("high", "EQ High", juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 0.0f, "dB"));
    p.push_back (std::make_unique<APF> ("compression", "Compression", juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 25.0f, "%"));
    p.push_back (std::make_unique<APF> ("saturation", "Saturation", juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 0.0f, "%"));
    p.push_back (std::make_unique<APF> ("air", "Air", juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 15.0f, "%"));
    p.push_back (std::make_unique<APF> ("afro", "Afro", juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 0.0f, "%"));
    return { p.begin(), p.end() };
}

void GaelRackFxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), static_cast<juce::uint32> (getTotalNumOutputChannels()) };
    lowShelfL.reset(); lowShelfR.reset(); highShelfL.reset(); highShelfR.reset();
    lowShelfL.prepare (spec); lowShelfR.prepare (spec); highShelfL.prepare (spec); highShelfR.prepare (spec);
    compressor.prepare (spec); limiter.prepare (spec);
    compressor.setThreshold (-18.0f); compressor.setRatio (2.5f); compressor.setAttack (10.0f); compressor.setRelease (100.0f);
    limiter.setThreshold (0.95f); limiter.setRelease (80.0f);
    auto low = juce::dsp::IIR::Coefficients<float>::makeLowShelf (sampleRate, 140.0f, 0.707f, 1.0f);
    auto high = juce::dsp::IIR::Coefficients<float>::makeHighShelf (sampleRate, 9000.0f, 0.707f, 1.0f);
    *lowShelfL.coefficients = *low; *lowShelfR.coefficients = *low;
    *highShelfL.coefficients = *high; *highShelfR.coefficients = *high;
}

void GaelRackFxAudioProcessor::releaseResources() {}

bool GaelRackFxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo()) return false;
    return out == layouts.getMainInputChannelSet();
}

void GaelRackFxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ignoreUnused (midi);
    juce::ScopedNoDenormals noDenormals;
    if (parameters.getRawParameterValue ("bypass")->load() >= 0.5f)
        return;
    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();
    juce::AudioBuffer<float> dry (channels, samples);
    dry.makeCopyOf (buffer, true);
    const auto inputDb = parameters.getRawParameterValue ("input")->load();
    const auto outputDb = parameters.getRawParameterValue ("output")->load();
    const auto mix = parameters.getRawParameterValue ("mix")->load() / 100.0f;
    const auto low = parameters.getRawParameterValue ("low")->load();
    const auto mid = parameters.getRawParameterValue ("mid")->load();
    const auto high = parameters.getRawParameterValue ("high")->load();
    const auto comp = parameters.getRawParameterValue ("compression")->load();
    const auto sat = parameters.getRawParameterValue ("saturation")->load() / 100.0f;
    const auto air = parameters.getRawParameterValue ("air")->load() / 100.0f;
    const auto afro = parameters.getRawParameterValue ("afro")->load() / 100.0f;
    buffer.applyGain (juce::Decibels::decibelsToGain (inputDb));
    auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf (getSampleRate(), 140.0f, 0.707f, juce::Decibels::decibelsToGain (low));
    auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf (getSampleRate(), 9000.0f, 0.707f, juce::Decibels::decibelsToGain (high + air * 4.0f));
    *lowShelfL.coefficients = *lowCoeffs; *lowShelfR.coefficients = *lowCoeffs;
    *highShelfL.coefficients = *highCoeffs; *highShelfR.coefficients = *highCoeffs;
    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        auto& lowFilter = (ch == 0 ? lowShelfL : lowShelfR);
        auto& highFilter = (ch == 0 ? highShelfL : highShelfR);
        for (int i = 0; i < samples; ++i)
        {
            float x = data[i];
            x = lowFilter.processSample (x);
            x = highFilter.processSample (x);
            const float midGain = juce::Decibels::decibelsToGain (mid * 0.45f);
            x *= midGain;
            if (sat > 0.0001f) x = (1.0f - sat * 0.45f) * x + sat * 0.45f * std::tanh (x * (1.0f + sat * 5.0f));
            if (afro > 0.0001f) x = (1.0f - afro * 0.12f) * x + afro * 0.12f * std::tanh (x * 1.8f);
            data[i] = x;
        }
    }
    compressor.setRatio (1.0f + comp * 0.055f);
    compressor.setThreshold (-12.0f - comp * 0.12f);
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    compressor.process (context);
    limiter.process (context);
    buffer.applyGain (juce::Decibels::decibelsToGain (outputDb));
    for (int ch = 0; ch < channels; ++ch)
    {
        auto* wet = buffer.getWritePointer (ch); const auto* original = dry.getReadPointer (ch);
        for (int i = 0; i < samples; ++i) wet[i] = original[i] * (1.0f - mix) + wet[i] * mix;
    }
}

juce::AudioProcessorEditor* GaelRackFxAudioProcessor::createEditor() { return new GaelRackFxAudioProcessorEditor (*this); }
void GaelRackFxAudioProcessor::setCurrentProgram (int index)
{
    currentProgram = juce::jlimit (0, presetNames.size() - 1, index);
    const float presets[7][7] = { {0,1,2,20,0,20,0}, {2,0,1,35,4,10,0}, {1,2,3,55,25,25,15}, {2,1,2,40,12,28,35}, {3,1,2,35,15,18,50}, {1,0,1,22,5,35,5}, {0,0,0,0,0,0,0} };
    const juce::String ids[] = {"low","mid","high","compression","saturation","air","afro"};
    for (int i = 0; i < 7; ++i) if (auto* p = parameters.getParameter (ids[i])) p->setValueNotifyingHost (p->convertTo0to1 (presets[currentProgram][i]));
}
const juce::String GaelRackFxAudioProcessor::getProgramName (int index) { return presetNames[juce::jlimit (0, presetNames.size() - 1, index)]; }
void GaelRackFxAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState(); std::unique_ptr<juce::XmlElement> xml (state.createXml()); copyXmlToBinary (*xml, destData);
}
void GaelRackFxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType())) parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new GaelRackFxAudioProcessor(); }
