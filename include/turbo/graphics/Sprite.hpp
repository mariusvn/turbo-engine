
#ifndef __TURBO_SPRITE_HPP__
#define __TURBO_SPRITE_HPP__

#include "Drawable.hpp"
#include "../Transform.hpp"
#include "../Rectangle.hpp"
#include "Texture.hpp"

namespace turbo {
    /**
     * @brief Represent a drawable sprite
     */
    class Sprite: public Drawable {
    public:
        Sprite(Texture* texture);
        void draw() const override;

        /**
         * @brief Get the width of the sprite
         * @return unsigned int width
         */
        unsigned int get_width() const;
        /**
         * @brief Get the height of the sprite
         * @return unsigned int height
         */
        unsigned int get_height() const;

    protected:
        /**
         * @brief Target texture
         */
        Texture* texture;
        /**
         * @brief Target texture region
         */
        Rectangle<> rect = Rectangle<>();
    };
}

#endif