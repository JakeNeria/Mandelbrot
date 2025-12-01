#include <SFML/Graphics.hpp>
#include <iostream>
#include "ComplexPlane.h"

int main() {

    // Get desktop resolution
    unsigned int desktopWidth = sf::VideoMode::getDesktopMode().width;
    unsigned int desktopHeight = sf::VideoMode::getDesktopMode().height;

    // Divide by 2 for a smaller screen
    unsigned int screenWidth = desktopWidth / 2;
    unsigned int screenHeight = desktopHeight / 2;

    // Min size check for usability
    if (screenWidth < 640) screenWidth = 640;
    if (screenHeight < 480) screenHeight = 480;

    // Construct the RenderWindow
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Mandelbrot Viewer");
    window.setFramerateLimit(60);

    // Construct the ComplexPlane
    ComplexPlane complexPlane(screenWidth, screenHeight);

    // Construct the Font and Text objects
    sf::Font font;
    if (!font.loadFromFile("resources/arial.ttf")) {
        std::cerr << "Error loading font: arial.ttf. Text display will not work.\n";
        return 1;
    }

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color::White);
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(1.0f);
    infoText.setPosition(10.f, 10.f); // Top-left corner


    while (window.isOpen()) {
        // Handle Input segment
        sf::Event event;

        while (window.pollEvent(event)) {
            // Handle Event::Closed event
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle Event::MouseButtonPressed
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePixel = { event.mouseButton.x, event.mouseButton.y };

                // Left click will zoomIn and call setCenter
                if (event.mouseButton.button == sf::Mouse::Left) {
                    complexPlane.setCenter(mousePixel);
                    complexPlane.zoomIn();
                }
                // Right click will zoomOut and call setCenter
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    complexPlane.setCenter(mousePixel);
                    complexPlane.zoomOut();
                }
            }

            // Handle Event::MouseMoved
            if (event.type == sf::Event::MouseMoved) {
                // Call setMouseLocation on the ComplexPlane object
                sf::Vector2i mousePixel = { event.mouseMove.x, event.mouseMove.y };
                complexPlane.setMouseLocation(mousePixel);
            }
        }

        // Check if Keyboard::isKeyPressed(Keyboard::Escape)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }


        // Call updateRender on the ComplexPlane object (handles CALCULATING/DISPLAYING state)
        complexPlane.updateRender();

        // Call loadText on the ComplexPlane object
        complexPlane.loadText(infoText);

        // --- Draw Scene segment ---

        window.clear(sf::Color::Black); // Clear the RenderWindow object

        window.draw(complexPlane); // draw the ComplexPlane object
        window.draw(infoText);     // draw the Text object

        window.display(); // Display the RenderWindow object
    }

    return 0;
}