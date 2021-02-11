#include <turbo/graphics/Text.hpp>
#include <allegro5/allegro.h>
#include <turbo/Utils.hpp>

namespace turbo {
    Text::Text(Font* font, const char* text): font(font), text(text) {
    }

    void Text::draw() const {
        al_draw_text((ALLEGRO_FONT*)this->font->__get_font(), al_map_rgb(255,255,255), 0, 0, 0, this->text);
    }

    Text::operator const char *() const {
        return this->text;
    }
}