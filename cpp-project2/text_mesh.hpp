#include <string>
#include <unordered_map>
#include <memory>

#include <spdlog/spdlog.h>

class GlBuffer;
struct GlyphData;

class TextMesh {
public:
    TextMesh(spdlog::logger* logger, const std::string& text, std::unordered_map<char, GlyphData> glyphData, float fontSize);
    ~TextMesh();

    void render();

private:

    void createMesh();

    std::string text;
    std::unordered_map<char, GlyphData> glyphData;

    std::unique_ptr<GlBuffer> vbo;
    unsigned int vao = 0;

    int count = 0;

    float fontSize;

    bool vaoExists = false;

    spdlog::logger* logger;

    const int glyphWidth = 7; // includes padding
    const int glyphHeight = 9; // includes apdding

};