
#ifndef __TURBO_RECTANGLE_HPP__
#define __TURBO_RECTANGLE_HPP__

namespace turbo {
    /**
     * @brief Class used to define a generic rectangle
     * 
     * @tparam unit
     */
    template<typename T = unsigned short>
    class Rectangle {
    public:
        Rectangle(T width, T height, T x, T y)
        : width(width), height(height), x(x), y(y) {}

        Rectangle()
        : width(T()), height(T()), x(T()), y(T()) {}

        Rectangle(const Rectangle<T>& rect) {
            this->width = rect.width;
            this->height = rect.height;
            this->x = rect.x;
            this->y = rect.y;
        }

        Rectangle& operator=(const Rectangle& rect) {
            this->width = rect.width;
            this->height = rect.height;
            this->x = rect.x;
            this->y = rect.y;
            return *this;
        }

        T width;
        T height;
        T x;
        T y;
    };
}

#endif