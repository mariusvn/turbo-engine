
#ifndef __TURBO_VECTOR2_HPP__
#define __TURBO_VECTOR2_HPP__

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace turbo {
    /**
     * Used to represent a 2 dimensional container
     * @tparam T type
     */
    template<typename T>
    class Vector2 {
    public:
        Vector2(T x, T y): x(x), y(y) {}
        Vector2(): x(T()), y(T()) {}
        Vector2(const Vector2<T>& vec): x(vec.x), y(vec.y) {}

        void reset() {
            this->x = T();
            this->y = T();
        }

        operator const char* () const {
            try {
                std::ostringstream stream;
                stream << "{x: " << x << ", y: " << y << "}";
                return stream.str().c_str();
            } catch (std::exception &e) {
                return "{vector2}";
            }
        }

        bool operator==(const Vector2<T>& vec) {
            return this->x == vec.x && this->y == vec.y;
        }

        Vector2<T> &operator=(const Vector2<T>& target) {
            this->x = target.x;
            this->y = target.y;
            return *this;
        }

        friend Vector2<T> operator-(const Vector2<T>& left, const Vector2<T> right) {
            return Vector2<T>(left.x - right.x, left.y - right.y);
        }

        friend Vector2<T> operator+(const Vector2<T>& left, const Vector2<T> right) {
            return Vector2<T>(left.x + right.x, left.y + right.y);
        }

        friend Vector2<T> operator*(const Vector2<T>& left, const Vector2<T> right) {
            return Vector2<T>(left.x * right.x, left.y * right.y);
        }

        friend Vector2<T> operator/(const Vector2<T>& left, const Vector2<T> right) {
            return Vector2<T>(left.x / right.x, left.y / right.y);
        }

        friend Vector2<T> operator-(const Vector2<T>& left, const T right) {
            return Vector2<T>(left.x - right, left.y - right);
        }

        friend Vector2<T> operator+(const Vector2<T>& left, const T right) {
            return Vector2<T>(left.x + right, left.y + right);
        }

        friend Vector2<T> operator*(const Vector2<T>& left, const T right) {
            return Vector2<T>(left.x * right, left.y * right);
        }

        friend Vector2<T> operator/(const Vector2<T>& left, const T right) {
            return Vector2<T>(left.x / right, left.y / right);
        }

        friend std::ostream &operator<<(std::ostream& os, const Vector2<T>& vec) {
            os << "{x: " << vec.x << ", y: " << vec.y << "}";
            return os;
        }

        T x;
        T y;
    };
}

#endif