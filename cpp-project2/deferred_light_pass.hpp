
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

class Framebuffer;
class ShaderProgram;

class DeferredLightPass {
public:
    DeferredLightPass(Framebuffer* framebuffer);
    ~DeferredLightPass();

    void execute(unsigned int lightTexture);

private:

    void init();

    Framebuffer* framebuffer;
    std::unique_ptr<ShaderProgram> deferredProgram;

    unsigned int vao;
    unsigned int vbo;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("deferred_light_pass");
};