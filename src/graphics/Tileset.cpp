#include <turbo/graphics/Tileset.hpp>

namespace turbo {
    Tileset::Tileset(Path* path,
                     const Vector2<unsigned short>& tile_size,
                     const Vector2<unsigned short>& tiles_nbr,
                     const unsigned short& margin,
                     const unsigned short& border_offset) {
        this->texture = new Texture(path);
        this->size = tile_size;
        this->margin = margin;
        this->border_offset = border_offset;
        this->init_sprites();
    }

    Tileset::~Tileset() {
        for (auto& item : this->sprites) {
            for (auto elem : item) {
                delete elem;
            }
        }
        delete this->texture;
    }

    Sprite* Tileset::get_sprite(const Vector2<unsigned short>& coords) const noexcept(false) {
        return this->get_sprite(coords.x, coords.y);
    }

    Sprite* Tileset::get_sprite(unsigned short x, unsigned short y) const noexcept(false) {
        if (x >= this->tile_amount.x || y >= this->tile_amount.y)
            throw std::runtime_error("Sprite out of tileset");
        return this->sprites[x][y];
    }

    Sprite* Tileset::operator()(const Vector2<unsigned short>& coords) const noexcept(false) {
        return this->get_sprite(coords);
    }

    Sprite* Tileset::operator()(unsigned short x, unsigned short y) const noexcept(false) {
        return this->get_sprite(x, y);
    }

    const Vector2<unsigned short>& Tileset::get_size() const {
        return this->tile_amount;
    }

    void Tileset::init_sprites() {
        for (unsigned short x = 0; x < this->tile_amount.x; x++) {
            this->sprites.emplace_back();
            for (unsigned short y = 0; y < this->tile_amount.y; y++) {
                Sprite* to_add = new Sprite(this->texture);
                to_add->rect.x = this->border_offset + x * (this->size.x + this->margin);
                to_add->rect.x = this->border_offset + y * (this->size.y + this->margin);
                to_add->rect.width = this->size.x;
                to_add->rect.height = this->size.y;
                this->sprites[x].push_back(to_add);
            }
        }
    }
}