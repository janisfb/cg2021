#include "space_object.h"

void Space_Object::update()
{
    /** \todo Update `model_matrix_` and position (`position_`) for each object.
    * 1. Combine translation and scaling matrices to get a result like in screenshots/planet_system_initial.png assuming:
    *   - sun at origin
    *   - all objects scaled by radius
    *   - all objects translated by `distance` in x direction
    * 2. Now use y-axis-rotation matrices in the right places to allow rotation
    *    around the object's axis (angle_self) and sun (angle_parent)
    * 3. You can also support a tilt angle if you like (`angle_tilt`) rotations
    * 4. Update the object's position using your constructed model matrix
    * Hints:
    *   - See glmath.h/cpp for an overview about implemented matrices.
    *   - Order is important!
    */

    model_matrix_ = mat4::rotate_y(angle_parent_) * mat4::translate(vec3(distance_, 0, 0)) * mat4::scale(radius_) * mat4::rotate_z(angle_tilt_) * mat4::rotate_y(angle_self_);
    position_ = model_matrix_ * vec4(0, 0, 0, 1);
}

//-----------------------------------------------------------------------------

void Moon::update()
{
    /** \todo Update moon's `model_matrix_` and position (`position_`).
    *  The moon is a bit special, it must rotate around its `parent_planet_`. Be careful with the
    *  translation-rotation-order since rotation matrices always rotate around the current origin.
    */
    model_matrix_ = mat4::translate(parent_planet_->position_) * mat4::rotate_y(angle_parent_) * mat4::translate(vec3(distance_, 0, 0)) * mat4::scale(radius_) * mat4::rotate_y(angle_self_);
    position_ = model_matrix_ * vec4(0, 0, 0, 1);
}

//-----------------------------------------------------------------------------

bool Space_Object::create_rings(float inner_radius, float outer_radius,
                                int resolution, std::string texfile)
{
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<float> u_coords;
    std::vector<float> v_coords;

    /** \todo (optional) Define the saturn ring mesh.
     *    - Fill the vectors defined above. Therefore, find a way to define a ring by triangles.
     *    - `inner_radius_` and `outer_radius_` define the size of the ring.
     *    - `resolution_` should determine how many vertices build the outer and inner boundary.
     *      So, if you decrease `resolution_` to 3 for example, the ring should look like a triangle with a hole in the middle.
     *    - `textures/saturn_rings.png` is the texture we use for the ring.
     *      Think about how you have to define the texcoords `u_coords` and `v_coords` to get the desired effect.
     *
     * Hints: - If you use the color shader to render the rings in `Solar_Viewer::draw_scene(..)` the normal vectors are not used.
     *      So they can hold arbitrary values in this case.
     *        - To test your rings' vertices without worrying about texture coordinates, you can simply use `u = v = 0.5` for each vertex.
     */


    if (!rings_.initialize(positions, normals, u_coords, v_coords, indices))
    {
        std::cerr << "\nError: " << name_ << " rings cannot be initialized!\n"
                  << std::endl;
        return false;
    }

    ring_texture_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                       GL_LINEAR, GL_REPEAT);
    if (!ring_texture_.loadPNG(texfile))
    {
        std::cerr << "\nError: " << name_
                  << " rings texture cannot be initialized!\n"
                  << std::endl;
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

void Space_Object::create_glow(float outer_radius, vec3 color)
{
    // create Billboard
    glow_ = new Billboard;

    // initialize texture layout
    glow_->texture_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                         GL_LINEAR, GL_REPEAT);

    // create texture image
    int res = 1024;
    glow_->create_glow_texture(res, (int)(res / (2.0 * outer_radius)) - 1,
                               res / 2 - 1, color);

    // initialize quad vertex layouts
    glow_->init_GL_arrays();
}

//-----------------------------------------------------------------------------

void Space_Object::update_glow_angles(vec3 eye)
{
    if (glow_ != nullptr)
    {
        vec3 pos_to_eye = normalize(vec3(eye) - vec3(position_));
        glow_->update_angles(pos_to_eye);
    }
}

//=============================================================================
