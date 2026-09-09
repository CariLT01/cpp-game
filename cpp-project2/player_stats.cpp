#include "player_stats.hpp"
#include "text_renderer.hpp"


PlayerStats::PlayerStats(TextRenderer* textRenderer) : textRenderer(textRenderer){

}

PlayerStats::~PlayerStats() {

}

void PlayerStats::incrementShardCount() {
    shardCount++;
    pendingUpdates = true;
}

void PlayerStats::update() {
    if (!pendingUpdates) return;

    for (auto& t  : texts) {
        textRenderer->deleteText(t);
    }
    texts.clear();
    
    Text* shardText = textRenderer->createText(std::format("Shards: {}", shardCount), 0, 0,  2.0f);
    texts.push_back(shardText);

    pendingUpdates = false;
}