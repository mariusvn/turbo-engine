#include <turbo/graphics/RectangleShape.hpp>
#include <allegro5/allegro_primitives.h>
#include <stdexcept>

namespace turbo {
    RectangleShape::RectangleShape(const Vector2<unsigned int>& pos, const Color& color): color(color) {
        this->size.x = pos.x;
        this->size.y = pos.y;
    }

    RectangleShape::RectangleShape(unsigned int width, unsigned int height, const Color& color): color(color) {
        this->size.x = width;
        this->size.y = height;
    }

    void RectangleShape::draw() const {
        ALLEGRO_COLOR color = al_map_rgba(this->color.r, this->color.g, this->color.b, this->color.a);

        if (this->border_radius == 0) {
            if (this->is_filled) {
                al_draw_filled_rectangle(0, 0, this->size.x, this->size.y, color);
            } else {
                if (this->thickness < 0)
                    throw std::runtime_error("Rectangle thickness must be above 0");
                al_draw_rectangle(0, 0, this->size.x, this->size.y, color, this->thickness);
            }
        } else {
            if (this->border_radius < 0)
                throw std::runtime_error("Rectangle border-radius must be above 0");
            if (this->is_filled) {
                al_draw_filled_rounded_rectangle(0, 0, this->size.x, this->size.y, this->border_radius, this->border_radius, color);
            } else {
                if (this->thickness < 0)
                    throw std::runtime_error("Rectangle thickness must be above 0");
                al_draw_rounded_rectangle(0, 0, this->size.x, this->size.y, this->border_radius, this->border_radius, color, this->thickness);
            }
        }
    }
}