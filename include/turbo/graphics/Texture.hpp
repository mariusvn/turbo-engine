
#ifndef __TURBO_TEXTURE_HPP__
#define __TURBO_TEXTURE_HPP__

#include "../Path.hpp"
#include "Drawable.hpp"
#include <allegro5/allegro.h>

namespace turbo {
    /**
     * @brief Contains the data of the sprites
     */
    class Texture {
    public:
        /**
         * @brief Construct a new Texture object
         * @param path path to the image to load
         */
        Texture(Path* path);

        /**
         * @brief Get the width of the texture
         * @return unsigned int width
         */
        unsigned int get_width() const;
        /**
         * @brief Get the height of the texture
         * @return unsigned int height
         */
        unsigned int get_height() const;

    #ifndef DOXYGEN_SHOULD_SKIP_THIS
        void* __get_texture_bitmap() const;
    #endif

    protected:
        /**
         * @brief Target allegro bitmap
         */
        ALLEGRO_BITMAP* bitmap;
    };
}

#endif