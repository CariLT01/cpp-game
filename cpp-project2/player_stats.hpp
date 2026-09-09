#include <vector>

class TextRenderer;
struct Text;

class PlayerStats {
public:
    PlayerStats(TextRenderer* textRenderer);
    ~PlayerStats();

    void incrementShardCount();

    void update();

private:

    

    unsigned int shardCount = 0;
    bool pendingUpdates = true;

    TextRenderer* textRenderer;
    
    std::vector<Text*> texts;
};