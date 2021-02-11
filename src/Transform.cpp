#include <turbo/Transform.hpp>
#include <turbo/Utils.hpp>
#include <iostream>

namespace turbo {
    Transform::Transform(
        const Vector2<float> &position,
        float angle,
        const Vector2<float> &scale,
        const Vector2<int> &center) {
            this->position = position;
            this->angle = angle;
            this->scale = scale;
            this->center = center;
        }
    
    Transform::Transform(
        float x, float y,
        int scaleX, int scaleY,
        float angle,
        int centerX, int centerY) {
            this->position.x = x;
            this->position.y = y;
            this->scale.x = scaleX;
            this->scale.y = scaleY;
            this->angle = angle;
            this->center.x = centerX;
            this->center.y = centerY;
        }

    Transform::~Transform() {
        delete this->transform;
    }

    void Transform::translate(float x, float y) {
        this->position.x += x;
        this->position.y += y;
        this->property_has_changed = true;
    }

    void Transform::translate(const Vector2<float> &offset) {
        this->position = this->position + offset;
        this->property_has_changed = true;
    }

    void Transform::set_position(float x, float y) {
        this->position.x = x;
        this->position.y = y;
        this->property_has_changed = true;
    }

    void Transform::set_position(const Vector2<float> &pos) {
        this->position = pos;
        this->property_has_changed = true;
    }

    void Transform::rotate(float deg) {
        this->angle += deg;
        if (this->angle > 360) {
            this->angle -= 360;
        } else if (this->angle < 0) {
            this->angle += 360;
        }
        this->property_has_changed = true;
    }

    void Transform::set_rotation(float deg) {
        if (deg > 360 || deg < 0)
            return;
        this->angle = deg;
        this->property_has_changed = true;
    }

    void Transform::set_center(int x, int y) {
        this->center.x = x;
        this->center.y = y;
        this->property_has_changed = true;
    }

    void Transform::set_center(const Vector2<int> &center) {
        this->center = center;
        this->property_has_changed = true;
    }

    void Transform::set_scale(const Vector2<float> &scale) {
        this->scale.x = scale.x;
        this->scale.y = scale.y;
        this->property_has_changed = true;
    }

    void Transform::set_scale(float x, float y) {
        this->scale.x = x;
        this->scale.y = y;
        this->property_has_changed = true;
    }

    const Vector2<float>& Transform::get_position() const {
        return this->position;
    }

    const Vector2<float>& Transform::get_scale() const {
        return this->scale;
    }

    const Vector2<int>& Transform::get_center() const {
        return this->center;
    }

    float Transform::get_angle() const {
        return this->angle;
    }

    ALLEGRO_TRANSFORM *Transform::get_transformer() {
        if (this->property_has_changed) {
            al_identity_transform(transform);
            al_scale_transform(transform, this->scale.x, this->scale.y);
            al_translate_transform(transform, -1 * this->center.x, -1 * this->center.y);
            al_rotate_transform(transform, DEGTORAD(this->angle));
            al_translate_transform(transform, this->center.x + this->position.x, this->center.y + this->position.y);
            this->property_has_changed = false;
        }
        return transform;
    }
}