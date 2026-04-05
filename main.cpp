#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>

int main()
{
    // ================= WINDOW =================
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Light Puzzle");

    // ================= PLAYER SETUP =================
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("assets/Top_Down_Survivor/flashlight/idle/survivor-idle_flashlight_0.png"))
        std::cout << "Error loading player\n";

    sf::Sprite player(playerTexture);
    player.setScale({0.6f, 0.6f});
    player.setPosition({100.f, 100.f});
    float speed = 0.25f;

    // ================= GUARD SETUP =================
    sf::Texture guardTexture;
    guardTexture.loadFromFile("assets/Top_Down_Survivor/shotgun/idle/survivor-idle_shotgun_0.png");

    sf::Sprite guard(guardTexture);
    guard.setScale({0.6f, 0.6f});
    guard.setPosition({500.f, 300.f});

    float guardSpeed = 0.1f;
    bool moveRight = true;

    // ================= KEY SETUP =================
    sf::Texture keyTexture;
    keyTexture.loadFromFile("assets/Keys/Keys-1.png");

    sf::Sprite key(keyTexture);
    key.setScale({2.f, 2.f});
    key.setPosition({600.f, 100.f});
    bool hasKey = false;

    // ================= WALLS (BOUNDARY SYSTEM) =================

    // TOP
    sf::RectangleShape wallTop({800.f, 20.f});
    wallTop.setPosition({0.f, 0.f});

    // BOTTOM
    sf::RectangleShape wallBottom({800.f, 20.f});
    wallBottom.setPosition({0.f, 580.f});

    // LEFT
    sf::RectangleShape wallLeft({20.f, 600.f});
    wallLeft.setPosition({0.f, 0.f});

    // RIGHT WALL (3 PARTS)
    sf::RectangleShape wallRightTop({20.f, 200.f});
    wallRightTop.setPosition({780.f, 0.f});

    sf::RectangleShape wallRightMiddle({20.f, 200.f}); // DOOR AREA
    wallRightMiddle.setPosition({780.f, 200.f});

    sf::RectangleShape wallRightBottom({20.f, 200.f});
    wallRightBottom.setPosition({780.f, 400.f});

    // COLORS
    wallTop.setFillColor(sf::Color(100, 100, 100));
    wallBottom.setFillColor(sf::Color(100, 100, 100));
    wallLeft.setFillColor(sf::Color(100, 100, 100));
    wallRightTop.setFillColor(sf::Color(100, 100, 100));
    wallRightMiddle.setFillColor(sf::Color(100, 100, 100)); // door color
    wallRightBottom.setFillColor(sf::Color(100, 100, 100));

    // ================= DOOR STATE =================
    bool doorOpen = false;

    // ================= LIGHT SYSTEM =================
    bool lightOn = true;

    sf::RectangleShape darkness({800.f, 600.f});
    darkness.setFillColor(sf::Color(0, 0, 0, 200));

    sf::CircleShape light(100.f);
    light.setFillColor(sf::Color(255, 255, 255, 100));
    light.setOrigin({100.f, 100.f});

    // ================= GAME STATE =================
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

        // ================= PLAYER MOVEMENT =================
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) player.move({0.f, -speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) player.move({0.f, speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) player.move({-speed, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) player.move({speed, 0.f});

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) player.move({0.f, -speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) player.move({0.f, speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) player.move({-speed, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) player.move({speed, 0.f});

        // ================= DOOR LOGIC =================
        if (hasKey)
            doorOpen = true;

        // ================= WALL COLLISION =================

        // TOP
        if (player.getGlobalBounds().findIntersection(wallTop.getGlobalBounds()).has_value())
            player.move({0.f, speed});

        // BOTTOM
        if (player.getGlobalBounds().findIntersection(wallBottom.getGlobalBounds()).has_value())
            player.move({0.f, -speed});

        // LEFT
        if (player.getGlobalBounds().findIntersection(wallLeft.getGlobalBounds()).has_value())
            player.move({speed, 0.f});

        // RIGHT (TOP + BOTTOM always block)
        if (player.getGlobalBounds().findIntersection(wallRightTop.getGlobalBounds()).has_value())
            player.move({-speed, 0.f});

        if (player.getGlobalBounds().findIntersection(wallRightBottom.getGlobalBounds()).has_value())
            player.move({-speed, 0.f});

        // RIGHT MIDDLE (DOOR BLOCK only when closed)
        if (!doorOpen)
        {
            if (player.getGlobalBounds().findIntersection(wallRightMiddle.getGlobalBounds()).has_value())
                player.move({-speed, 0.f});
        }

        // ================= GUARD MOVEMENT =================
        guard.move({moveRight ? guardSpeed : -guardSpeed, 0.f});
        if (guard.getPosition().x > 700) moveRight = false;
        if (guard.getPosition().x < 100) moveRight = true;

        // ================= GUARD COLLISION =================
        if (player.getGlobalBounds().findIntersection(guard.getGlobalBounds()).has_value() && lightOn)
            gameOver = true;

        // ================= KEY PICKUP =================
        if (!hasKey && player.getGlobalBounds().findIntersection(key.getGlobalBounds()).has_value())
        {
            hasKey = true;
            key.setPosition({-100.f, -100.f});
        }

        // ================= WIN CONDITION =================
        if (doorOpen && player.getPosition().x > 800)
            win = true;

        // ================= LIGHT FOLLOW =================
        auto pb = player.getGlobalBounds();
        light.setPosition(pb.position + pb.size / 2.f);

        // ================= DRAW =================
        window.clear();

        window.draw(player);
        window.draw(guard);

        if (!hasKey)
            window.draw(key);

        // walls
        window.draw(wallTop);
        window.draw(wallBottom);
        window.draw(wallLeft);
        window.draw(wallRightTop);
        window.draw(wallRightBottom);

        // draw door ONLY when closed
        if (!doorOpen)
            window.draw(wallRightMiddle);

        if (!lightOn)
        {
            window.draw(darkness);
            window.draw(light);
        }

        window.display();
    }

    return 0;
}