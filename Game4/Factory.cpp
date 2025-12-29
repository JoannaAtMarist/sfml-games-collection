#include <cstdlib> // rand()
#include "Factory.hpp"

#include "BeeProjectileUpdate.hpp"
#include "BeeProjectileGraphics.hpp"
#include "ButterflyGraphics.hpp"
#include "ForegroundGraphics.hpp"
#include "MushroomPerilUpdate.hpp"
#include "RoadGraphics.hpp"
#include "RoadUpdate.hpp"
#include "TreeClimbUpdate.hpp"
#include "TreeGraphics.hpp"

#include "CameraGraphics.hpp"
#include "CameraUpdate.hpp"
#include "InputDispatcher.hpp"
#include "LevelUpdate.hpp"
#include "MenuGraphics.hpp"
#include "MenuUpdate.hpp"
#include "PlatformGraphics.hpp"
#include "PlatformUpdate.hpp"
#include "PlayerGraphics.hpp"
#include "PlayerUpdate.hpp"
#include "PositionUpdate.hpp"

#include "TextureHolder.hpp"

using namespace sf;
using namespace std;

// A 128x64 crop from the road texture.
const sf::IntRect RoadPlatformRect(0, 0, 128, 64);

// Helper struct to return both parts of a platform
struct PlatformCreation
{
    GameObject *object;
    std::shared_ptr<PlatformUpdate> update;
};

// Create a standard platform GameObject with Update and Graphics
PlatformCreation createPlatform(float x, float y, float width, float height,
                                const sf::IntRect *explicitRect = nullptr)
{
    auto g = new GameObject();

    auto update = std::make_shared<PlatformUpdate>();
    auto graphics = std::make_shared<PlatformGraphics>();

    g->addComponent(update);
    g->addComponent(graphics);

    // Initial position/size
    update->setPosition(x, y);
    update->setSize(width, height);

    graphics->assemble(update);

    return {g, update};
}

// Create a climbable tree GameObject at the specified X position
// The tree's base will rest on the specified roadTopY
/* Disable Trees for now
static GameObject *
createClimbableTree(float trunkCenterX, float roadTopY,
                    std::shared_ptr<LevelUpdate> levelUpdate,
                    std::shared_ptr<PlayerUpdate> playerUpdate)
{
    auto obj = new GameObject();

    float scale = 3.0f;
    int numTrunksHigh = 4;

    // 1. THE BASE (Roots) - Middle tree in the sprite sheet
    sf::IntRect rectBase(48, 0, 48, 256);

    // 2. THE COLUMN (Straight Trunk) - Left tree in the sprite sheet
    sf::IntRect rectColumn(0, 0, 48, 256);

    float baseW = rectBase.width * scale;
    float baseH = rectBase.height * scale;
    float colW = rectColumn.width * scale;
    float colH = rectColumn.height * scale;

    // --------------------------------------------------------
    // GRAPHICS GENERATION
    // --------------------------------------------------------

    // STARTING Y CALCULATION:
    // We want the BOTTOM of the sprite to be at 'roadTopY'.
    // SFML positions sprites by their Top-Left corner.
    // So, Top-Left Y = Ground Y - Sprite Height.

    // We add a tiny offset (+10) to bury the roots slightly in the grass
    float currentY = roadTopY - baseH + 10.f;

    // 1. Place the Base (Roots)
    auto posBase = std::make_shared<PositionUpdate>();
    posBase->setPosition(trunkCenterX - (baseW / 2.f), currentY);

    auto gfxBase =
        std::make_shared<TreeGraphics>("graphics/Trunk 02.png", rectBase);
    gfxBase->assemble(posBase);
    gfxBase->setScale(scale);

    obj->addComponent(posBase);
    obj->addComponent(gfxBase);

    // 2. Stack the Columns ABOVE the base
    for (int i = 0; i < numTrunksHigh; i++) {
        // Move UP by one column height
        // (Remember: In SFML, Y decreases as you go up)
        currentY -= colH;

        // Add a tiny overlap (+2) to hide seams
        float nextTopY = currentY + 2.f;

        auto posCol = std::make_shared<PositionUpdate>();
        posCol->setPosition(trunkCenterX - (colW / 2.f), nextTopY);

        auto gfxCol =
            std::make_shared<TreeGraphics>("graphics/Trunk 02.png", rectColumn);
        gfxCol->assemble(posCol);
        gfxCol->setScale(scale);

        obj->addComponent(posCol);
        obj->addComponent(gfxCol);
    }

    // --------------------------------------------------------
    // CLIMB LOGIC
    // --------------------------------------------------------
    auto climb = std::make_shared<TreeClimbUpdate>();
    climb->assemble(levelUpdate, playerUpdate);

    // Calculate total height of the tree for the hitbox
    // Top of tree (currentY) to Bottom of tree (roadTopY)
    float topOfClimb = currentY;
    float totalHeight = roadTopY - topOfClimb;

    // Make hitbox generous (80% of visual width)
    float climbWidth = colW * 0.8f;

    climb->setTrunkArea(trunkCenterX - (climbWidth / 2.f), topOfClimb,
                        climbWidth, totalHeight);

    obj->addComponent(climb);

    return obj;
}
*/

// Helper struct to return both parts of a bee
struct BeeCreation
{
    GameObject *object;
    std::shared_ptr<BeeProjectileUpdate> update;
};

// Create a bee projectile GameObject
GameObject *createBee(float x, float y, float vx, int damage, bool zigzag,
                      int level, std::shared_ptr<LevelUpdate> levelUpdate,
                      std::shared_ptr<PlayerUpdate> playerUpdate)
{
    auto g = new GameObject();
    auto upd = std::make_shared<BeeProjectileUpdate>(); // 1. Create it first
    auto gfx = std::make_shared<BeeProjectileGraphics>();

    upd->setActiveLevel(level); // 2. THEN set the level

    upd->setPosition(x, y);
    upd->setVelocity(vx, 0.f);
    upd->setDamage(damage);

    if (zigzag) {
        upd->enableZigZag(60.f, 6.f);
    }

    upd->assemble(levelUpdate, playerUpdate);
    gfx->assemble(upd);

    g->addComponent(upd);
    g->addComponent(gfx);

    return g;
}

// Create a mushroom GameObject at the specified X position
// The mushroom's base will rest on the specified roadTopY
GameObject *createMushroom(float x, float roadTopY, const std::string &texture,
                           std::shared_ptr<LevelUpdate> levelUpdate,
                           std::shared_ptr<PlayerUpdate> playerUpdate)
{
    auto g = new GameObject();
    auto peril = std::make_shared<MushroomPerilUpdate>();
    auto pos = std::make_shared<PositionUpdate>();
    auto gfx = std::make_shared<ForegroundGraphics>(texture);

    // Push mushroom down by 10 pixels to stop floating
    float yOffset = 10.f;
    float spriteY = roadTopY + yOffset;

    // 1) Static position used by ForegroundGraphics
    pos->setPosition(x, spriteY);

    // 2) Peril hurtbox based on same coordinates
    peril->setPosition(x, spriteY);
    peril->setAreaSize(80.f, 40.f);
    peril->assemble(levelUpdate, playerUpdate);

    // 3) ForegroundGraphics draws the mushroom at the PositionUpdate
    gfx->assemble(pos);

    g->addComponent(pos);
    g->addComponent(peril);
    g->addComponent(gfx);

    return g;
}

// FACTORY METHODS
Factory::Factory(RenderWindow *window) : m_Window(window)
{
    // Ensure TextureHolder exists
    static TextureHolder holder;
}

// Load a level with objects
void Factory::loadLevel(vector<GameObject *> &gameObjects,
                        InputDispatcher &inputDispatcher)
{
    cout << "[Factory] Loading level..." << endl;

    //. LEVEL OBJECT (must exist before player)
    GameObject *level = new GameObject();
    auto levelUpdate = std::make_shared<LevelUpdate>();

    // PASS THE WINDOW SIZE
    levelUpdate->setWindowSize(m_Window->getSize());

    level->addComponent(levelUpdate);

    gameObjects.push_back(level);

    //. Billina (the Player)
    std::cout << "Summoning Billina..." << std::endl;

    // Construct the GameObject on the heap from the start?
    GameObject *player = new GameObject();

    // --- PlayerUpdate ---
    auto playerUpdate = std::make_shared<PlayerUpdate>();
    playerUpdate->assemble(levelUpdate, nullptr);
    player->addComponent(playerUpdate);

    // Input registration
    inputDispatcher.registerNewInputReceiver(playerUpdate->getInputReceiver());

    // --- PlayerGraphics (Billina) ---
    auto playerGraphics = std::make_shared<PlayerGraphics>();
    playerGraphics->assemble(*m_Window, playerUpdate);
    player->addComponent(playerGraphics);

    // LevelUpdate learns about player
    levelUpdate->assemble(nullptr, playerUpdate);

    //. Create Camera (Replaces Background)
    std::cout << "Painting the sky..." << std::endl;

    GameObject *camera = new GameObject();
    auto cameraUpdate = std::make_shared<CameraUpdate>();
    auto cameraGraphics = std::make_shared<CameraGraphics>();

    cameraUpdate->assemble(levelUpdate, playerUpdate);
    cameraGraphics->assemble(cameraUpdate);

    // Give camera access to level info so it can hide the forest
    cameraGraphics->assemble(levelUpdate);

    camera->addComponent(cameraUpdate);
    camera->addComponent(cameraGraphics);

    gameObjects.push_back(camera);

    //. Road Layer
    {
        std::cout << "Building the road..." << std::endl;

        float tileW = 64;
        float tileH = 64;

        float screenW = m_Window->getSize().x;
        float screenH = m_Window->getSize().y;

        float roadWidth = screenW * 3;
        int numTiles = roadWidth / tileW + 2;
        float roadY = screenH - tileH;

        // FULL ROAD GAMEOBJECT
        GameObject *road = new GameObject();

        // Update (collision)
        auto roadUpdate = std::make_shared<RoadUpdate>();
        roadUpdate->setPosition(0, roadY);
        roadUpdate->setSize(roadWidth, tileH);

        // roadUpdate learns about player
        roadUpdate->assemble(levelUpdate, playerUpdate);

        road->addComponent(roadUpdate);

        // Graphics
        auto gfx = std::make_shared<RoadGraphics>(
            "graphics/oneTile.png", numTiles, tileW, tileH, roadY);
        road->addComponent(gfx);

        gameObjects.push_back(road);
    }

    //. Climbable Trees (behind platforms, on the road)
    {
        /* Disable Trees for now
        std::cout << "Planting climbable trees..." << std::endl;

        float screenH = m_Window->getSize().y;
        float roadTopY = screenH - 64.f; // road tile height is 64

        // Pick some world X positions along the road
        std::vector<float> treeXs = {450.f, 900.f, 1450.f};

        for (float x : treeXs) {
            GameObject *tree =
                createClimbableTree(x, roadTopY, levelUpdate, playerUpdate);
            gameObjects.push_back(tree);
        }
        */
    }

    //. Platforms
    std::cout << "Building the platforms..." << std::endl;

    const int platformCount = 20; // how many platforms in the path
    const float tileW = 64.f;
    const float tileH = 64.f;

    // --- SETUP SPECIAL INDICES ---
    // We want them separated so we don't accidentally stack effects.

    // 1. Flight (Butterfly) - somewhere early-ish
    int flightIndex = 4;

    // 2. Disappearing (Turquoise Mushroom) - Middle
    int disappearIndex = 8;

    // 3. Deadly (Red Mushroom) - Late
    int deadlyIndex = 12;

    for (int i = 0; i < platformCount; ++i) {
        // Initial dummy placement
        auto created = createPlatform(0.f, 0.f, tileW, tileH);
        created.update->assemble(nullptr, playerUpdate);

        // --- A) FLIGHT PLATFORM ---
        if (i == flightIndex) {
            created.update->setGrantsFlight(true, 4.f);
            auto marker =
                std::make_shared<ButterflyGraphics>("graphics/butterfly.png");
            marker->assemble(created.update);
            created.object->addComponent(marker);
        }

        // --- B) DISAPPEARING PLATFORM (Visual: Turquoise Mushroom) ---
        else if (i == disappearIndex) {
            created.update->setDisappearing(true, 1.5f); // Vanish after 1.5s

            // Visual Debug Marker
            auto marker = std::make_shared<ForegroundGraphics>(
                "graphics/mushrooms/3turquoise.png");
            marker->assemble(created.update); // Attach to platform
            created.object->addComponent(marker);
        }

        // --- C) DEADLY PLATFORM (Visual: Red Mushroom) ---
        else if (i == deadlyIndex) {
            // Pass '1' as the second argument to make it Level 1 Only
            created.update->setDeadly(true, 1);

            // Visual Marker (Red Mushroom) - Set this to Level 1 only too
            auto marker = std::make_shared<ForegroundGraphics>(
                "graphics/mushrooms/2purple-red.png");
            marker->assemble(created.update);
            marker->assemble(levelUpdate);
            marker->setVisibleLevel(1); // Hide visual in L2
            created.object->addComponent(marker);
        }

        // D) DECORATION (Flowers - LEVEL 2 ONLY)
        else if (rand() % 2 == 0) {
            std::string flowerFile;
            int r = rand() % 6;
            if (r == 0)
                flowerFile = "graphics/flowers/hollyhock.png";
            else if (r == 1)
                flowerFile = "graphics/flowers/foxglove.png";
            else if (r == 2)
                flowerFile = "graphics/flowers/daisy2.png";
            else if (r == 3)
                flowerFile = "graphics/flowers/gladilolus.png";
            else if (r == 4)
                flowerFile = "graphics/flowers/iris.png";
            else
                flowerFile = "graphics/flowers/tulip.png";

            auto flowerGfx = std::make_shared<ForegroundGraphics>(flowerFile);

            // Connect to platform for position
            flowerGfx->assemble(created.update);
            // Connect to LevelUpdate for Level Logic
            flowerGfx->assemble(levelUpdate);

            flowerGfx->setAlpha(150);

            // Only show in Level 2
            flowerGfx->setVisibleLevel(2);

            created.object->addComponent(flowerGfx);
        }

        gameObjects.push_back(created.object);
        levelUpdate->addPlatform(created.update.get());
    }

    // Initialize the platform layout
    levelUpdate->positionLevelAtStart();

    cout << "[Factory] Level loaded with multiple platforms!\n";

    //. Projectiles: Bees
    std::cout << "Summoning bees..." << std::endl;

    // Level 1 Bees
    gameObjects.push_back(createBee(300.f, 150.f, -150.f, 1, false, 1,
                                    levelUpdate, playerUpdate));
    gameObjects.push_back(createBee(600.f, 380.f, -150.f, 1, false, 1,
                                    levelUpdate, playerUpdate));

    // Level 2 Swarm
    float swarmStartX = 1400.f;
    for (int i = 0; i < 3; ++i) {
        // Pass '2' for Level 2
        gameObjects.push_back(createBee(swarmStartX, 350.f + i * 25.f, -260.f,
                                        2, true, 2, levelUpdate, playerUpdate));
    }

    //. Foreground: Flowers
    std::cout << "Planting flowers..." << std::endl;

    auto addFlower = [&](float x, float y, std::string file,
                         sf::Uint8 alpha = 220) {
        auto obj = new GameObject();

        auto pos = std::make_shared<PositionUpdate>();
        pos->setPosition(x, y);

        auto gfx = std::make_shared<ForegroundGraphics>(file);
        gfx->assemble(pos);
        gfx->setAlpha(alpha);

        // Opacity
        gfx->setAlpha(180);

        obj->addComponent(pos);
        obj->addComponent(gfx);

        gameObjects.push_back(obj);
    };

    //. Foreground: Mushrooms (non-lethal static platform perils)
    float screenH = m_Window->getSize().y;
    float screenW = m_Window->getSize().x;
    float roadTopY = screenH - tileH; // tileH is 64.f from above
    float roadWidth = screenW * 3.f;  // same width used for the road

    std::cout << "Planting mushrooms..." << std::endl;

    // mushroom textures to choose from
    std::vector<std::string> mushTextures = {
        "graphics/mushrooms/1green.png",
        "graphics/mushrooms/1orange.png",
        "graphics/mushrooms/2green.png",
        "graphics/mushrooms/2purple-dark.png",
        "graphics/mushrooms/2purple-red.png",
        "graphics/mushrooms/3rose.png",
        "graphics/mushrooms/3turquoise.png",
        "graphics/mushrooms/4brown.png"};

    int numMushrooms = 15; // how many you want on the road
    float margin = 80.f;   // keep them away from absolute edges

    // --- LEVEL 1: MUSHROOMS (Safe-ish) ---
    for (int i = 0; i < 15; ++i) {
        float x = 100.f + (rand() % 3000); // Random X

        // Create components manually so we can set the Level
        auto g = new GameObject();
        auto pos = std::make_shared<PositionUpdate>();
        auto peril = std::make_shared<MushroomPerilUpdate>();
        std::string randomTex = mushTextures[rand() % mushTextures.size()];
        auto gfx = std::make_shared<ForegroundGraphics>(randomTex);

        float yOffset = 10.f;
        float spriteY = roadTopY + yOffset;

        pos->setPosition(x, spriteY);

        // PERIL SETUP
        peril->setPosition(x, spriteY);
        peril->setAreaSize(80.f, 40.f);
        peril->assemble(levelUpdate, playerUpdate);
        peril->setDamage(1);      // Low Damage
        peril->setActiveLevel(1); // LEVEL 1 ONLY

        // GRAPHICS SETUP
        gfx->assemble(pos);
        gfx->assemble(levelUpdate);
        gfx->setVisibleLevel(1); // LEVEL 1 ONLY

        g->addComponent(pos);
        g->addComponent(peril);
        g->addComponent(gfx);
        gameObjects.push_back(g);
    }

    // --- LEVEL 2: POPPIES & FLOWERS MIX ---
    int numFloraL2 = 20;

    for (int i = 0; i < numFloraL2; ++i) {
        float x = 100.f + (rand() % 3000);

        // 50/50 Chance: Deadly Poppy or Safe Flower
        bool isPoppy = (rand() % 2 == 0);

        auto g = new GameObject();
        auto pos = std::make_shared<PositionUpdate>();
        float yOffset = 10.f;
        float spriteY = roadTopY + yOffset;
        pos->setPosition(x, spriteY);

        if (isPoppy) {
            // --- DEADLY POPPY ---
            auto peril = std::make_shared<MushroomPerilUpdate>();
            auto gfx = std::make_shared<ForegroundGraphics>(
                "graphics/flowers/poppy.png");

            peril->setPosition(x, spriteY);
            peril->setAreaSize(60.f, 40.f);
            peril->assemble(levelUpdate, playerUpdate);
            peril->setDamage(100);
            peril->setActiveLevel(2);

            gfx->assemble(pos);
            gfx->assemble(levelUpdate);
            gfx->setVisibleLevel(2);

            g->addComponent(peril);
            g->addComponent(gfx);
        } else {
            // --- SAFE DECORATIVE FLOWER ---
            std::string safeTex = "graphics/flowers/daisy.png";
            if (rand() % 2 == 0)
                safeTex = "graphics/flowers/tulip.png";

            auto gfx = std::make_shared<ForegroundGraphics>(safeTex);

            gfx->assemble(pos);
            gfx->assemble(levelUpdate);
            gfx->setVisibleLevel(2);

            g->addComponent(gfx);
        }

        g->addComponent(pos);
        gameObjects.push_back(g);
    }

    //. Create the Menu (Handles Start/Pause + Health HUD)
    GameObject *menu = new GameObject();

    // Update component (handles paused / game-over state)
    auto menuUpdate = std::make_shared<MenuUpdate>(m_Window);

    // Graphics component (renders text + hearts)
    auto menuGraphics = std::make_shared<MenuGraphics>();

    // Attach components to the menu GameObject
    menu->addComponent(menuUpdate);
    menu->addComponent(menuGraphics);

    // Wire it to Level and Player for pause / game-over logic
    menuUpdate->assemble(levelUpdate, playerUpdate);

    // Wire graphics to BOTH the menu state and the player (for hearts)
    menuGraphics->assemble(menuUpdate);
    menuGraphics->assemble(playerUpdate);

    // Make sure ENTER goes to MenuUpdate
    inputDispatcher.registerNewInputReceiver(menuUpdate->getInputReceiver());

    // Finally add menu object to the world
    gameObjects.push_back(menu);

    //. PUSH
    // Create Player (Logic happens earlier, but PUSH happens LAST)
    // This ensures Billina is drawn on top of the sky, road, and platforms.
    gameObjects.push_back(player);
}