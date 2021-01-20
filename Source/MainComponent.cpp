#include "MainComponent.h"

static std::string base64Encode(const std::string& source)
{
    auto* b64 = BIO_new (BIO_f_base64());
    BIO_set_flags (b64, BIO_FLAGS_BASE64_NO_NL);

    auto* sink = BIO_new (BIO_s_mem());
    BIO_push (b64, sink);
    BIO_write (b64, source.data(), (int) source.size());
    BIO_flush (b64);

    char* encoded = nullptr;
    const auto len = BIO_get_mem_data (sink, &encoded);
    return std::string (encoded, len);
}

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    const auto c = getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId);
    g.fillAll (c);

    const std::string source = "test";
    const auto result = base64Encode (source);
    jassert (r == "dGVzdA==");

    juce::String message;
    message
        << "From: " << source << juce::newLine
        << "To: " << result << juce::newLine;

    g.setColour (c.contrasting());

    const auto b = getLocalBounds().reduced (16);
    g.drawMultiLineText (message, b.getX(), b.getY(), b.getWidth());
}

void MainComponent::resized()
{
}
