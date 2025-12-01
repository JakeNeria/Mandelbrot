#include "ComplexPlane.h"
#include <iostream>
#include <sstream>
#include <iomanip>


ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
    // Assign m_pixel_size with the parameter values
    m_pixel_size = { pixelWidth, pixelHeight };

    // Calculate and assign the aspect ratio: height / width
    m_aspectRatio = (float)m_pixel_size.y / m_pixel_size.x;

    // Assign m_plane_center with {0,0}
    m_plane_center = { 0.0f, 0.0f };

    // Assign m_plane_size with {BASE_WIDTH, BASE_HEIGHT * m_aspectRatio}
    m_plane_size.x = BASE_WIDTH;
    m_plane_size.y = BASE_WIDTH * m_aspectRatio; // Note: BASE_WIDTH used here to match BASE_HEIGHT in the prompt (4.0)

    // Assign m_zoomCount with 0
    m_zoomCount = 0;

    // Assign m_State with State::CALCULATING
    m_state = State::CALCULATING;

    // Initialize VertexArray
    m_vArray.setPrimitiveType(sf::Points);
    m_vArray.resize((size_t)m_pixel_size.x * m_pixel_size.y);
}


void ComplexPlane::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // This function only needs one line of code:
    target.draw(m_vArray);
}


void ComplexPlane::updateRender() {
    // If m_State is CALCULATING
    if (m_state == State::CALCULATING) {
        // Create a double for loop to loop through all pixels
        for (int i = 0; i < m_pixel_size.y; ++i) { // i for y (row)
            for (int j = 0; j < m_pixel_size.x; ++j) { // j for x (column)
                size_t index = (size_t)j + (size_t)i * m_pixel_size.x;

                // Set the position variable in the element of VertexArray
                m_vArray[index].position = { (float)j, (float)i };

                // Use mapPixelToCoords to find the Vector2f coordinate
                sf::Vector2f coord = mapPixelToCoords({ j, i });

                // Call countIterations and store the number of iterations
                size_t iterations = countIterations(coord);

                // Declare three local Uint8 variables r,g,b
                sf::Uint8 r, g, b;

                // Pass the number of iterations and the RGB variables into iterationsToRGB
                iterationsToRGB(iterations, r, g, b);

                // Set the color variable in the element of VertexArray
                m_vArray[index].color = sf::Color(r, g, b);
            }
        }
        // Set the state to DISPLAYING
        m_state = State::DISPLAYING;
    }
}


void ComplexPlane::zoomIn() {
    // Increment m_zoomCount
    m_zoomCount++;

    // Set a local variable for the x size to BASE_WIDTH * (BASE_ZOOM to the m_ZoomCount power)
    float new_x_size = BASE_WIDTH * std::pow(BASE_ZOOM, m_zoomCount);

    // Set a local variable for the y size to BASE_HEIGHT * m_aspectRatio * (BASE_ZOOM to the m_ZoomCount power)
    // Note: Using BASE_WIDTH here to be consistent with the constructor's calculation: BASE_WIDTH * m_aspectRatio
    float new_y_size = BASE_WIDTH * m_aspectRatio * std::pow(BASE_ZOOM, m_zoomCount);

    // Assign m_plane_size with this new size
    m_plane_size.x = new_x_size;
    m_plane_size.y = new_y_size;

    // Set m_State to CALCULATING
    m_state = State::CALCULATING;
}


void ComplexPlane::zoomOut() {
    // Decrement m_zoomCount, ensuring it doesn't go below 0
    m_zoomCount = std::max(0, m_zoomCount - 1);

    // Same as zoomIn, using the new m_zoomCount
    float new_x_size = BASE_WIDTH * std::pow(BASE_ZOOM, m_zoomCount);
    float new_y_size = BASE_WIDTH * m_aspectRatio * std::pow(BASE_ZOOM, m_zoomCount);

    m_plane_size.x = new_x_size;
    m_plane_size.y = new_y_size;

    // Set m_State to CALCULATING
    m_state = State::CALCULATING;
}


void ComplexPlane::setCenter(sf::Vector2i mousePixel) {
    // Use mapPixelToCoords to find the Vector2f coordinate
    sf::Vector2f new_center = mapPixelToCoords(mousePixel);

    // Assign m_plane_center with this coordinate
    m_plane_center = new_center;

    // Set m_State to CALCULATING
    m_state = State::CALCULATING;
}


void ComplexPlane::setMouseLocation(sf::Vector2i mousPixel) {
    m_mouseLocation = mousPixel;
}


void ComplexPlane::loadText(sf::Text& text) {
    // Calculate mouse complex coordinates live
    sf::Vector2f mouse_c = mapPixelToCoords(m_mouseLocation);

    std::ostringstream ss;
    ss.precision(4);
    ss << std::fixed;

    // The formula for the base zoom factor is based on BASE_ZOOM to the power of m_zoomCount.
    // The overall zoom level (relative to the initial view) is the reciprocal of the plane size ratio.
    float totalZoom = BASE_WIDTH / m_plane_size.x;

    ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << "i)\n";
    ss << "Zoom: " << m_zoomCount << " (" << totalZoom << "x)\n";
    ss << "Cursor: (" << mouse_c.x << ", " << mouse_c.y << "i)";

    text.setString(ss.str());
}


size_t ComplexPlane::countIterations(sf::Vector2f coord) {
    // c = a + bi where a=coord.x and b=coord.y
    std::complex<double> c(coord.x, coord.y);
    // z_0 = 0
    std::complex<double> z(0.0, 0.0);

    size_t i = 0;

    // z_{n+1} = z_n^2 + c
    // Check if |z| never goes above 2.0 (equivalent to |z|^2 never going above 4.0)
    // Using z.real()*z.real() + z.imag()*z.imag() is often faster than abs(z)
    while ((z.real() * z.real() + z.imag() * z.imag()) < 4.0 && i < MAX_ITER) {
        z = z * z + c;
        i++;
    }
    return i;
}


void ComplexPlane::iterationsToRGB(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b) {
    if (count == MAX_ITER) {
        // At MAX_ITER I colored the pixel black {0,0,0}
        r = 0; g = 0; b = 0;
    }
    else {
        // Simple color mapping based on escape time (H S L strategy implemented using a smooth gradient)
        // Normalize count to a float between [0, 1)
        float t = (float)count / MAX_ITER;

        // A smooth transition through colors (based on a common fractal coloring palette)
        // This simulates cycling through hues:
        r = (sf::Uint8)(9.0f * (1.0f - t) * t * t * t * 255.0f);
        g = (sf::Uint8)(15.0f * (1.0f - t) * (1.0f - t) * t * t * 255.0f);
        b = (sf::Uint8)(8.5f * (1.0f - t) * (1.0f - t) * (1.0f - t) * t * 255.0f);
    }
}


sf::Vector2f ComplexPlane::mapPixelToCoords(sf::Vector2i mousePixel) {
    // Unpacking variables for clarity
    float px = (float)mousePixel.x;
    float py = (float)mousePixel.y;
    float pixel_width = (float)m_pixel_size.x;
    float pixel_height = (float)m_pixel_size.y;

    float plane_width = m_plane_size.x;
    float plane_height = m_plane_size.y;
    float center_x = m_plane_center.x;
    float center_y = m_plane_center.y;

    // The offset of +c is always equal to either (m_plane_center.x - m_plane_size.x / 2.0)
    float real_min = center_x - plane_width / 2.0f;
    float imag_min = center_y - plane_height / 2.0f;

    // General formula: ((n - a) / (b - a)) * (d - c) + c


    // n = px, [a, b] = [0, pixel_width], magnitude (d-c) = plane_width, offset c = real_min
    float complexX = (px / pixel_width) * plane_width + real_min;

    // formula for pixel y ([0, height]) to complex y ([imag_max, imag_min]):
    float imag_max = center_y + plane_height / 2.0f;
    // The Y coordinate maps [0, height] to [imag_max, imag_min]
    float complexY = imag_max - (py / pixel_height) * plane_height;

    return sf::Vector2f(complexX, complexY);
}