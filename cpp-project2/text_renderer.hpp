#include "spdlog/sinks/stdout_color_sinks.h"
#include <unordered_map>
#include <memory>
#include <vector>

#include <spdlog/spdlog.h>

class TextMesh;
class ShaderProgram;

class AssetLoader;
class Texture;

struct GlyphData {
    float u0;
    float v0;
    float u1;
    float v1;
    float u2;
    float v2;
    float u3;
    float v3;
};

struct Text {
    std::unique_ptr<TextMesh> textMesh;
    int x;
    int y;
    float fontSize;
};

class TextRenderer {
public:
    TextRenderer(AssetLoader* assetLoader, int width, int height);
    ~TextRenderer();

    Text* createText(const std::string& name, int x, int y, float fontSize);
    void deleteText(Text* text);

    void render();

private:
    void loadAtlas();
    void loadShaders();
    void renderText(Text* text);

    const int width = 18;
    const int glyphWidth = 7; // includes padding
    const int glyphHeight = 9; // includes apdding

    const int atlasWidth = 128;
    const int atlasHeight = 64;

    AssetLoader* assetLoader;
    Texture* atlasTexture;

    std::unordered_map<char, GlyphData> glyphs;

    std::vector<std::unique_ptr<Text>> textObjects;

    std::unique_ptr<ShaderProgram> textShader;


    int windowWidth;
    int windowHeight;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("text_renderer");
};