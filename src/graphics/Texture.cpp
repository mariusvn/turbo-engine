#include <turbo/graphics/Texture.hpp>

#include <stdexcept>

namespace turbo {
    Texture::Texture(Path* path) {
        this->bitmap = al_load_bitmap(*path);
        if (!this->bitmap) {
            throw std::runtime_error(std::string("Cannot load bitmap at \"") + ((const char*)*path) + "\"");
        }
    }

    unsigned int Texture::get_width() const {
        return al_get_bitmap_width(this->bitmap);
    }

    unsigned int Texture::get_height() const {
        return al_get_bitmap_height(this->bitmap);
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void* Texture::__get_texture_bitmap() const {
        return this->bitmap;
    }
#endif
}