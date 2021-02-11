#include <turbo/graphics/CircleShape.hpp>
#include <allegro5/allegro_primitives.h>
#include <stdexcept>

namespace turbo {
    CircleShape::CircleShape(unsigned short radius, const Color& color): radius(radius), color(color) {}

    void CircleShape::draw() const {
        ALLEGRO_COLOR color = al_map_rgba(this->color.r, this->color.g, this->color.b, this->color.a);
        if (this->is_filled) {
            al_draw_filled_circle(0, 0, this->radius, color);
        } else {
            if (this->thickness < 0)
                throw std::runtime_error("Thickness must be positive");
            al_draw_circle(0, 0, this->radius, color, this->thickness);
        }
    }
}