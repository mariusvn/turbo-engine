#include <turbo/graphics/Sprite.hpp>
#include <allegro5/allegro.h>
#include <turbo/Utils.hpp>

namespace turbo {
    Sprite::Sprite(Texture* texture): texture(texture) {
        this->rect.width = texture->get_width();
        this->rect.height = texture->get_height();
    }

    void Sprite::draw() const {
        ALLEGRO_BITMAP* bitmap = (ALLEGRO_BITMAP*) this->texture->__get_texture_bitmap();

        al_draw_bitmap_region(bitmap, this->rect.x, this->rect.y, this->rect.width, this->rect.height, 0, 0, 0);
    }

    unsigned int Sprite::get_width() const {
        return this->texture->get_width();
    }

    unsigned int Sprite::get_height() const {
        return this->texture->get_height();
    }
}