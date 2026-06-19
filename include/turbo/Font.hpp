
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

        /**
         * @brief Destroy the Font and free the underlying Allegro font
         */
        ~Font();

        // A Font owns its Allegro font; copying would double-free it.
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

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