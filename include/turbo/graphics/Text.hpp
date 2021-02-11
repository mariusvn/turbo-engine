
#ifndef __TURBO_TEXT_HPP__
#define __TURBO_TEXT_HPP__

#include "Drawable.hpp"
#include "../Transform.hpp"
#include "../Font.hpp"

namespace turbo {
    /**
     * @brief Represent a drawable text
     */
    class Text: public Drawable {
    public:
        /**
         * @brief Construct a new Text drawable
         * 
         * @param font Selected text font
         * @param text content
         */
        Text(Font* font, const char* text = "");
        ~Text();

        void draw() const override;

        operator const char*() const;

        /**
         * @brief Text content
         */
        const char* text;

    protected:
        /**
         * @brief Selected font
         */
        Font* font;
    };
}

#endif