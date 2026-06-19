
#ifndef __TURBO_COLOR_HPP__
#define __TURBO_COLOR_HPP__

namespace turbo {
    /**
     * @brief Represent a color
     */
    class Color {
    public:
        /** @brief Build a colour from red, green, blue and (optional) alpha (0-255). */
        Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255): r(r), g(g), b(b), a(a) {}
        /** @brief Build a grey colour (all channels = @p vals; opaque). */
        Color(unsigned char vals = 255): r(vals), g(vals), b(vals) {}

        /**
         * @brief Red value
         */
        unsigned char r = 255;
        /**
         * @brief Green value
         */
        unsigned char g = 255;
        /**
         * @brief Blue value
         */
        unsigned char b = 255;
        /**
         * @brief Alpha value
         */
        unsigned char a = 255;
    };
}

#endif