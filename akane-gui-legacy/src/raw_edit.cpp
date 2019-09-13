
/*/
static Canvas::SharedPtr canvas = nullptr;

if (canvas == nullptr)
{
    auto file = fopen("d:/test2.raw.txt", "rb");

    int width  = 0;
    int height = 0;
    fscanf(file, "%d %d", &width, &height);

    canvas = std::make_shared<Canvas>(width, height);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float r, g, b;
            fscanf(file, "%f %f %f", &r, &g, &b);
            canvas->Append(r, g, b, x, y);
        }
    }

    InitializeDisplayTexture(width, height, true);
}

static float gamma                                          = 1.f;
static char tone_mapping_name[100]                          = "ACES";
static std::function<Spectrum(const Spectrum&)> tone_mapper = ToneMap_Aces;
ImGui::Begin("raw edit");

if (ImGui::BeginCombo("tone mapper", tone_mapping_name))
{
    if (ImGui::Selectable("ACES", true))
    {
        strcpy(tone_mapping_name, "ACES");
        tone_mapper = ToneMap_Aces;
    }
    if (ImGui::Selectable("Reinhard"))
    {
        strcpy(tone_mapping_name, "Reinhard");
        tone_mapper = ToneMap_Reinhard;
    }
    if (ImGui::Selectable("None"))
    {
        strcpy(tone_mapping_name, "None");
        tone_mapper = [](const Spectrum& s) { return s; };
    }

    ImGui::EndCombo();
}

ImGui::SliderFloat("gamma", &gamma, 0.1f, 4.f);

ImGui::End();

UpdateDisplayTexture([&](int x, int y) {
    auto spectrum = canvas->GetSpectrum(x, y, 1.);
    return SpectrumToRGB(GammaCorrect(tone_mapper(spectrum), gamma));
});

ImGui::Image(RetriveDisplayTexture(),
             {1.f * CurrentState.ResolutionWidth, 1.f * CurrentState.ResolutionHeight});
//*/
