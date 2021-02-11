
#ifndef __TURBO_RECTANGLESHAPE_HPP__
#define __TURBO_RECTANGLESHAPE_HPP__

#include "Shape.hpp"
#include "../Vector2.hpp"
#include "../Rectangle.hpp"
#include "Color.hpp"

namespace turbo {
    /**
     * @brief Represents a drawable rectangle
     * 
     */
    class RectangleShape: public Shape {
    public:
        RectangleShape(const Vector2<unsigned int>& size, const Color& color = Color());
        RectangleShape(unsigned int width, unsigned int height, const Color& color = Color());
        void draw() const override;

        /**
         * @brief Is filled or only borders ?
         */
        bool is_filled = true;
        /**
         * @brief Thickness of the border is `is_filled` is `false`
         * @note trigger runtime exception if negative
         */
        float thickness = 1.0f;
        /**
         * @brief Border radius of the rectangle
         * @note trigger runtime exception if negative
         */
        float border_radius = 0.0f;
        /**
         * @brief Color of the shape
         */
        Color color = Color();
        /**
         * @brief size of the rectangle in pixels
         */
        Vector2<unsigned int> size;
    };
}

#endif