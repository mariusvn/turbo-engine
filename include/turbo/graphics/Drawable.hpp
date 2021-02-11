
#ifndef __TURBO_DRAWABLE_HPP__
#define __TURBO_DRAWABLE_HPP__

namespace turbo
{
    /**
     * @brief Represent somthing that can be drawn
     */
    class Drawable {
    public:
        /**
         * @brief Draw the drawable -_-
         */
        virtual void draw() const = 0;
    };
}


#endif