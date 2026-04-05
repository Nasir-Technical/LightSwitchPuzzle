#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Light Puzzle");

    // ================= PLAYER =================
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("assets/Top_Down_Survivor/flashlight/idle/survivor-idle_flashlight_0.png"))
        std::cout << "Error loading player\n";

    sf::Sprite player(playerTexture);
    player.setScale({0.6f, 0.6f});
    player.setPosition({100.f, 100.f});
    float speed = 0.25f;

    // ================= TILESET =================
    // sf::Texture tileTexture;
    // if (!tileTexture.loadFromFile("assets/warped_top_down_tech_lab_extension.png"))
    //     std::cout << "Error loading tileset\n";

    // sf::Sprite tile(tileTexture);
    // int TILE_SIZE = 32;
    // tile.setScale({2.f, 2.f});

    // ================= GUARD =================
    sf::Texture guardTexture;
    if (!guardTexture.loadFromFile("assets/Top_Down_Survivor/shotgun/idle/survivor-idle_shotgun_0.png")) // Update this path to your guard's image
        std::cout << "Error loading guard\n";

    sf::Sprite guard(guardTexture);
    guard.setScale({0.6f, 0.6f});
    guard.setPosition({500.f, 300.f});

    float guardSpeed = 0.1f;
    bool moveRight = true;

    // ================= KEY =================
    sf::Texture keyTexture;
    if (!keyTexture.loadFromFile("assets/Keys/Keys-1.png"))
        std::cout << "Error loading key\n";

    sf::Sprite key(keyTexture);
    key.setScale({2.f, 2.f});
    key.setPosition({600.f, 100.f});
    bool hasKey = false;

    // ================= DOOR =================
    sf::Texture doorTexture;
    if (!doorTexture.loadFromFile("assets/doors.png"))
        std::cout << "Error loading door texture\n";

    sf::Sprite door(doorTexture);
    door.setPosition({700.f, 500.f});

    // Assuming each frame in doors.png is 32x32 pixels
    sf::IntRect doorFrame(0, 0, 32, 32);
    door.setTextureRect(doorFrame);
    // ================= LIGHT =================
    bool lightOn = true;

    sf::RectangleShape darkness({800.f, 600.f});
    darkness.setFillColor(sf::Color(0, 0, 0, 200));

    sf::CircleShape light(100.f);
    light.setFillColor(sf::Color(255, 255, 255, 100));
    light.setOrigin({100.f, 100.f});

    // ================= GAME STATES =================
    bool gameStarted = false;
    bool gameOver = false;
    bool win = false;

    // ================= FONT =================
    sf::Font font;
    if (!font.openFromFile("Super Youth.ttf"))
        return -1;

    sf::Text text(font);
    text.setCharacterSize(40);

    // ================= TIMER =================
    sf::Clock clock;

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // ================= START SCREEN =================
        if (!gameStarted)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                gameStarted = true;
                clock.restart();
            }

            window.clear();
            text.setString("Press SPACE to Start");

            auto rect = text.getLocalBounds();
            text.setOrigin(rect.position + rect.size / 2.f);
            text.setPosition({400.f, 300.f});

            window.draw(text);
            window.display();
            continue;
        }

        // ================= GAME OVER / WIN =================
        if (gameOver || win)
        {
            window.clear();
            text.setString(win ? "YOU WIN!" : "GAME OVER");

            auto rect = text.getLocalBounds();
            text.setOrigin(rect.position + rect.size / 2.f);
            text.setPosition({400.f, 300.f});

            window.draw(text);
            window.display();
            continue;
        }

        // ================= INPUT =================
        lightOn = !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);

        // ================= PLAYER MOVE =================
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            player.move({0.f, -speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            player.move({0.f, speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            player.move({-speed, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            player.move({speed, 0.f});

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            player.move({0.f, -speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            player.move({0.f, speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            player.move({-speed, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            player.move({speed, 0.f});

        // ================= GUARD MOVE =================
        guard.move({moveRight ? guardSpeed : -guardSpeed, 0.f});
        if (guard.getPosition().x > 700)
            moveRight = false;
        if (guard.getPosition().x < 100)
            moveRight = true;

        // ================= COLLISION =================
        if (player.getGlobalBounds().findIntersection(guard.getGlobalBounds()).has_value() && lightOn)
            gameOver = true;

        // ================= KEY PICKUP =================
        if (!hasKey && player.getGlobalBounds().findIntersection(key.getGlobalBounds()).has_value())
        {
            hasKey = true;
            key.setPosition({-100.f, -100.f});
        }

        // ================= DOOR =================
        if (hasKey && player.getGlobalBounds().findIntersection(door.getGlobalBounds()).has_value())
            win = true;

        // ================= TIMER =================
        int time = clock.getElapsedTime().asSeconds();
        sf::Text timerText(font);
        timerText.setCharacterSize(20);
        timerText.setPosition({10.f, 10.f});
        timerText.setString("Time: " + std::to_string(time));

        // ================= LIGHT FOLLOW =================
        auto pb = player.getGlobalBounds();
        light.setPosition(pb.position + pb.size / 2.f);

        // ================= DRAW =================
        window.clear();

        // for (int x = 0; x < 800; x += TILE_SIZE * 2)
        // {
        //     for (int y = 0; y < 600; y += TILE_SIZE * 2)
        //     {
        //         if (x == 0 || x >= 800 - TILE_SIZE * 2 || y == 0 || y >= 600 - TILE_SIZE * 2)
        //             tile.setTextureRect(sf::IntRect({32,0}, {32,32}));
        //         else
        //             tile.setTextureRect(sf::IntRect({0,0}, {32,32}));

        //         tile.setPosition({(float)x, (float)y});
        //         window.draw(tile);
        //     }
        // }

        window.draw(player);
        window.draw(guard);

        if (!hasKey)
            window.draw(key);

        window.draw(door);
        window.draw(timerText);

        if (!lightOn)
        {
            window.draw(darkness);
            window.draw(light);
        }

        window.display();
    }

    return 0;
}