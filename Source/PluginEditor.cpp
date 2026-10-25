#include "PluginEditor.h"

namespace
{
    const juce::Colour bg (0xff071018), panel (0xff0b1821), text (0xffeaf2f8), green (0xff16d878), red (0xffff3447), blue (0xff22aaff);
}

GaelRackFxAudioProcessorEditor::GaelRackFxAudioProcessorEditor (GaelRackFxAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (1000, 700);
    title.setText ("GaëlRack Fx", juce::dontSendNotification); title.setJustificationType (juce::Justification::centred); title.setColour (juce::Label::textColourId, text); title.setFont (juce::Font (juce::FontOptions (42.0f, juce::Font::bold)));
    subtitle.setText ("VOCAL EFFECT PLUGIN  •  LE SON DE MADAGASCAR", juce::dontSendNotification); subtitle.setJustificationType (juce::Justification::centred); subtitle.setColour (juce::Label::textColourId, green); subtitle.setFont (juce::Font (juce::FontOptions (15.0f)));
    presetsLabel.setText ("PRÉRÉGLAGES", juce::dontSendNotification); presetsLabel.setColour (juce::Label::textColourId, text);
    micLabel.setText ("MICROPHONE", juce::dontSendNotification); micLabel.setColour (juce::Label::textColourId, text);
    for (auto* c : { static_cast<juce::Component*>(&title), &subtitle, &presetsLabel, &micLabel, &presets, &microphone, &power, &input, &mix, &output, &low, &mid, &high, &compression, &saturation, &air, &afro }) addAndMakeVisible (c);
    presets.addItemList ({"Femme", "Homme", "Trap", "Afrobeat", "Salegy", "Slow", "Clean"}, 1);
    presets.onChange = [this] { processor.setCurrentProgram (presets.getSelectedItemIndex()); };
    microphone.addItemList ({"AKG P120", "AT2020", "BM800"}, 1); microphone.setSelectedId (1);
    power.setClickingTogglesState (true); power.setToggleState (false, juce::dontSendNotification); power.setColour (juce::TextButton::buttonColourId, green.darker (0.7f)); power.setColour (juce::TextButton::textColourOffId, text); power.setColour (juce::TextButton::buttonOnColourId, red.darker (0.7f));
    bypassAttachment = std::make_unique<ButtonAttachment> (processor.parameters, "bypass", power);
    power.onClick = [this] { power.setButtonText (power.getToggleState() ? "BYPASS" : "ON"); };
    styleSlider (input, green); styleSlider (mix, red); styleSlider (output, green);
    styleSlider (low, blue); styleSlider (mid, green); styleSlider (high, red); styleSlider (compression, red); styleSlider (saturation, juce::Colour (0xffffa52f)); styleSlider (air, juce::Colour (0xffb34dff)); styleSlider (afro, green);
    auto addAttachment = [this] (juce::Slider& s, const juce::String& id) { attachments.push_back (std::make_unique<SliderAttachment> (processor.parameters, id, s)); };
    addAttachment (input, "input"); addAttachment (mix, "mix"); addAttachment (output, "output"); addAttachment (low, "low"); addAttachment (mid, "mid"); addAttachment (high, "high"); addAttachment (compression, "compression"); addAttachment (saturation, "saturation"); addAttachment (air, "air"); addAttachment (afro, "afro");
}

void GaelRackFxAudioProcessorEditor::styleSlider (juce::Slider& s, juce::Colour accent)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
    s.setColour (juce::Slider::rotarySliderFillColourId, accent); s.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff273744));
    s.setColour (juce::Slider::thumbColourId, text); s.setColour (juce::Slider::textBoxTextColourId, text); s.setColour (juce::Slider::textBoxBackgroundColourId, bg); s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void GaelRackFxAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (bg);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff11232d), 0.0f, 0.0f, bg, 0.0f, 700.0f, false)); g.fillAll ();
    g.setColour (panel); g.fillRoundedRectangle (18.0f, 145.0f, 964.0f, 145.0f, 14.0f); g.fillRoundedRectangle (18.0f, 305.0f, 964.0f, 245.0f, 14.0f); g.fillRoundedRectangle (18.0f, 565.0f, 964.0f, 115.0f, 14.0f);
    g.setColour (juce::Colour (0xff29414f)); g.drawRoundedRectangle (18.0f, 145.0f, 964.0f, 145.0f, 14.0f, 1.0f); g.drawRoundedRectangle (18.0f, 305.0f, 964.0f, 245.0f, 14.0f, 1.0f); g.drawRoundedRectangle (18.0f, 565.0f, 964.0f, 115.0f, 14.0f, 1.0f);
    // Stylised Madagascar silhouette and Malagasy flag motif in the header.
    juce::Path island; island.startNewSubPath (900, 18); island.lineTo (920, 30); island.lineTo (925, 52); island.lineTo (916, 70); island.lineTo (923, 90); island.lineTo (908, 120); island.lineTo (896, 132); island.lineTo (884, 105); island.lineTo (889, 82); island.lineTo (879, 61); island.lineTo (888, 42); island.closeSubPath();
    g.setColour (red.withAlpha (0.9f)); g.fillPath (island); auto lowerIsland = island; juce::Path lower; lower.startNewSubPath (888, 78); lower.lineTo (916, 78); lower.lineTo (908, 120); lower.lineTo (896, 132); lower.lineTo (884, 105); lower.closeSubPath(); g.setColour (green.withAlpha (0.95f)); g.fillPath (lower);
    g.setColour (red); g.fillRoundedRectangle (35.0f, 18.0f, 12.0f, 28.0f, 2.0f); g.setColour (juce::Colours::white); g.fillRoundedRectangle (47.0f, 18.0f, 12.0f, 28.0f, 2.0f); g.setColour (green); g.fillRoundedRectangle (59.0f, 18.0f, 12.0f, 28.0f, 2.0f);
    g.setColour (text.withAlpha (0.7f)); g.setFont (juce::Font (juce::FontOptions (12.0f))); g.drawText ("AUTHENTIC SOUND  •  MADE IN MADAGASCAR", 82, 22, 360, 24, juce::Justification::centredLeft);
    g.setColour (text); g.setFont (juce::Font (juce::FontOptions (14.0f))); g.drawText ("INPUT", 150, 153, 150, 24, juce::Justification::centred); g.drawText ("MIX", 425, 153, 150, 24, juce::Justification::centred); g.drawText ("OUTPUT", 700, 153, 150, 24, juce::Justification::centred);
    const juce::String labels[] = {"EQ LOW", "EQ MID", "EQ HIGH", "COMPRESSION", "SATURATION", "AIR", "AFRO"};
    const int xs[] = {32, 166, 300, 434, 568, 702, 836};
    for (int i = 0; i < 7; ++i) { g.setColour (juce::Colour (0xff172833)); g.fillRoundedRectangle ((float) xs[i], 316.0f, 126.0f, 222.0f, 9.0f); g.setColour (text); g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold))); g.drawText (labels[i], xs[i], 321, 126, 22, juce::Justification::centred); }
    g.setColour (text.withAlpha (0.6f)); g.setFont (juce::Font (juce::FontOptions (11.0f))); g.drawText ("Le son de Madagascar", 370, 109, 260, 22, juce::Justification::centred);
}

void GaelRackFxAudioProcessorEditor::resized()
{
    title.setBounds (210, 38, 580, 56); subtitle.setBounds (270, 91, 460, 24); presetsLabel.setBounds (36, 570, 150, 22); micLabel.setBounds (650, 570, 150, 22);
    input.setBounds (165, 174, 120, 105); mix.setBounds (440, 174, 120, 105); output.setBounds (715, 174, 120, 105);
    low.setBounds (38, 350, 112, 88); mid.setBounds (172, 350, 112, 88); high.setBounds (306, 350, 112, 88); compression.setBounds (440, 350, 112, 88); saturation.setBounds (574, 350, 112, 88); air.setBounds (708, 350, 112, 88); afro.setBounds (842, 350, 112, 88);
    presets.setBounds (35, 600, 430, 35); microphone.setBounds (650, 600, 210, 35); power.setBounds (880, 590, 80, 55);
}
