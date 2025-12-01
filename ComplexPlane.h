#pragma once

#include <SFML/Graphics.hpp>
#include <complex>
#include <vector>
#include <algorithm> // For std::max in zoomOut
#include <cmath>     // For std::pow

// Global Constants
const unsigned int MAX_ITER = 64;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

// State Enum
enum class State {
    CALCULATING,
    DISPLAYING
};

class ComplexPlane : public sf::Drawable {
public:
    // Constructor
    ComplexPlane(int pixelWidth, int pixelHeight);

    void zoomIn();
    void zoomOut();
    void setCenter(sf::Vector2i mousePixel);
    void setMouseLocation(sf::Vector2i mousPixel);
    void loadText(sf::Text& text);
    void updateRender();

private:
    // Private drawing function (Overridden from sf::Drawable)
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::Vector2f mapPixelToCoords(sf::Vector2i mousePixel);
    size_t countIterations(sf::Vector2f coord);
    void iterationsToRGB(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b);

    sf::VertexArray m_vArray;
    State m_state;
    sf::Vector2i m_mouseLocation;
    sf::Vector2i m_pixel_size;      // Screen width/height in pixels
    sf::Vector2f m_plane_center;    // Center of the complex plane view (real, imag)
    sf::Vector2f m_plane_size;      // Current width/height of the complex plane view (real, imag)
    int m_zoomCount;                // Tracks number of zooms
    float m_aspectRatio;
};