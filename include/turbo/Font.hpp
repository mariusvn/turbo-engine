
#ifndef __TURBO_FONT_HPP__
#define __TURBO_FONT_HPP__

#include "Path.hpp"
#include <allegro5/allegro_font.h>

namespace turbo {
    /**
     * @brief Represent the text font
     */
    class Font {
    public:
        /**
         * @brief Construct a new Font for a ttf file
         * @param path path to the ttf file
         * @param size size of the font in px
         */
        Font(const Path& path, unsigned short size);

    #ifndef DOXYGEN_SHOUL_SKIP_THIS
        void* __get_font() const;
    #endif

    protected:
        /**
         * @brief target allegro font
         */
        ALLEGRO_FONT* font;
    };
}

#endif