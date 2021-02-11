#include <turbo/Path.hpp>

namespace turbo {

    Path::Path(const char* path) {
        this->path = al_create_path(path);
        this->absolute();
    }

    Path::~Path() {
        al_destroy_path(this->path);
        this->path = nullptr;
    }

    /* GETTERS */

    const char* Path::get_drive() const {
        return al_get_path_drive(this->path);
    }

    int Path::get_num_components() const {
        return al_get_path_num_components(this->path);
    }

    const char* Path::get_component(int i) const {
        return al_get_path_component(this->path, i);
    }

    const char* Path::get_tail() const {
        return al_get_path_tail(this->path);
    }

    const char* Path::get_filename() const {
        return al_get_path_filename(this->path);
    }

    const char* Path::get_basename() const {
        return al_get_path_basename(this->path);
    }

    const char* Path::get_file_extension() const {
        return al_get_path_extension(this->path);
    }

    /* SETTERS */

    void Path::set_drive(const char* drive) {
        al_set_path_drive(this->path, drive);
    }

    void Path::append_component(const char* component) {
        al_append_path_component(this->path, component);
    }

    void Path::insert_component(int i, const char* component) {
        al_insert_path_component(this->path, i, component);
    }

    void Path::replace_component(int i, const char* component) {
        al_replace_path_component(this->path, i, component);
    }

    void Path::remove_component(int i) {
        al_remove_path_component(this->path, i);
    }

    void Path::drop_tail() {
        al_drop_path_tail(this->path);
    }

    void Path::set_filename(const char* filename) {
        al_set_path_filename(this->path, filename);
    }

    void Path::set_file_extension(const char* ext) {
        al_set_path_extension(this->path, ext);
    }

    /* UTILITY */

    void Path::absolute() {
        al_make_path_canonical(this->path);
    }

    Path* Path::get_resources_path() {
        ALLEGRO_PATH* p_a = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
        Path* p = new Path(al_path_cstr(p_a, '/'));
        al_destroy_path(p_a);
        return p;
    }

    const char* Path::c_str() const {
        return al_path_cstr(this->path, '/');
    }

    Path::operator const char* () const {
        return this->c_str();
    }

    Path::operator std::string () const {
        return std::string(this->c_str());
    }

    std::ostream& operator<<(std::ostream& output, const turbo::Path& path) {
        output << path.c_str();
        return output;
    }
}