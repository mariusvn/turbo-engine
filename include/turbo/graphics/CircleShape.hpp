
#ifndef __TURBO_CIRCLE_SHAPE_HPP__
#define __TURBO_CIRCLE_SHAPE_HPP__

#include "Shape.hpp"
#include "Color.hpp"
#include "../Vector2.hpp"

namespace turbo {
    /**
     * @brief Simple cicle shape
     */
    class CircleShape: public Shape {
    public:
        CircleShape(unsigned short radius = 10, const Color& color = Color());
        void draw() const override;

        /**
         * @brief Radius of the circle in pixel
         */
        unsigned short radius = 10;
        /**
         * @brief Color of the circle
         */
        Color color = Color();
        /**
         * @brief Thickness of the border if `is_filled` is `true`
         */
        float thickness = 1.0f;
        /**
         * @brief Is filled or only show border
         */
        bool is_filled = true;
    };
}

#endif