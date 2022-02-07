//=============================================================================
//
//   Exercise code for the lecture "Computer Graphics"
//     by Prof. Mario Botsch, TU Dortmund
//
//   Copyright (C)  Computer Graphics Group, TU Dortmund
//
//=============================================================================

#include "bezier_patch.h"
#include <algorithm>
#include <cfloat>

using namespace pmp;

//=============================================================================

Bezier_patch::Bezier_patch()
    : selected_control_point_(0),
      cpoly_vertex_array_(0),
      cpoly_vertex_buffer_(0),
      cpoly_index_buffer_(0),
      surf_vertex_array_(0),
      surf_vertex_buffer_(0),
      surf_normal_buffer_(0),
      surf_index_buffer_(0),
      use_de_Casteljau_(true)
{
    // initialize control polygon to zero
    for (unsigned int i = 0; i < 4; ++i)
        for (unsigned int j = 0; j < 4; ++j)
            control_points_[i][j] = vec3(0, 0, 0);

    // tessellate control polygon: edges between control points
    control_edges_.clear();
    control_edges_.reserve(48);
    control_edges_.push_back(0);
    control_edges_.push_back(1);
    control_edges_.push_back(1);
    control_edges_.push_back(2);
    control_edges_.push_back(2);
    control_edges_.push_back(3);
    control_edges_.push_back(4);
    control_edges_.push_back(5);
    control_edges_.push_back(5);
    control_edges_.push_back(6);
    control_edges_.push_back(6);
    control_edges_.push_back(7);
    control_edges_.push_back(8);
    control_edges_.push_back(9);
    control_edges_.push_back(9);
    control_edges_.push_back(10);
    control_edges_.push_back(10);
    control_edges_.push_back(11);
    control_edges_.push_back(12);
    control_edges_.push_back(13);
    control_edges_.push_back(13);
    control_edges_.push_back(14);
    control_edges_.push_back(14);
    control_edges_.push_back(15);
    control_edges_.push_back(0);
    control_edges_.push_back(4);
    control_edges_.push_back(4);
    control_edges_.push_back(8);
    control_edges_.push_back(8);
    control_edges_.push_back(12);
    control_edges_.push_back(1);
    control_edges_.push_back(5);
    control_edges_.push_back(5);
    control_edges_.push_back(9);
    control_edges_.push_back(9);
    control_edges_.push_back(13);
    control_edges_.push_back(2);
    control_edges_.push_back(6);
    control_edges_.push_back(6);
    control_edges_.push_back(10);
    control_edges_.push_back(10);
    control_edges_.push_back(14);
    control_edges_.push_back(3);
    control_edges_.push_back(7);
    control_edges_.push_back(7);
    control_edges_.push_back(11);
    control_edges_.push_back(11);
    control_edges_.push_back(15);
}

//-----------------------------------------------------------------------------

Bezier_patch::~Bezier_patch()
{
    // delete OpenGL buffers for control polygon
    glDeleteBuffers(1, &cpoly_vertex_buffer_);
    glDeleteBuffers(1, &cpoly_index_buffer_);
    glDeleteVertexArrays(1, &cpoly_vertex_array_);

    // delete OpenGL buffers for surface mesh
    glDeleteBuffers(1, &surf_vertex_buffer_);
    glDeleteBuffers(1, &surf_normal_buffer_);
    glDeleteBuffers(1, &surf_index_buffer_);
    glDeleteVertexArrays(1, &surf_vertex_array_);
}

//-----------------------------------------------------------------------------

void Bezier_patch::bounding_box(vec3 &_bbmin, vec3 &_bbmax) const
{
    _bbmin = _bbmax = control_points_[0][0];

    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            _bbmin = min(_bbmin, control_points_[i][j]);
            _bbmax = max(_bbmax, control_points_[i][j]);
        }
    }
}

//------------------------------------------------------------------------------

int factorial(int n) {
    int r = 1;
    for (int i = n; i > 0; i--) {
        r *= i;
    }
    return r;
}

float bernstein(int i, int n, float t) {
    float r = (float)factorial(n) / (float)(factorial(i) * factorial(n - i));
    r *= pow(t, i);
    r *= pow(1 - t, n - i);
    return r;
}

//------------------------------------------------------------------------------

void Bezier_patch::position_normal(float _u, float _v, vec3 &_p, vec3 &_n) const
{
    /** \todo Evaluate the Bezier patch at parameter (`_u`,`_v`) in order to
     *   compute a position (to be stored in `_p`) and a normal vector (to
     *   be stored in `_n`).
     *
     *   Note that the normal vector is the cross product of the u- and
     * v-tangent. Use either the analytic defition of the Bezier patch, i.e.,
     * the cubic Bernstein polynomials \f$ B_i^3 \f$, or the bilinear de
     * Casteljau algorithm dependent on the boolean `use_de_Casteljau`. Compare
     * their performance by using the GUI.
     */

    vec3 p(0.0);
    vec3 du(0.0);
    vec3 dv(0.0);
    vec3 n(0.0);

    if (use_de_Casteljau_) {
        vec3 res_points[3][3];

        for (unsigned int i = 0; i < 3; ++i) {
            for (unsigned int j = 0; j < 3; ++j) {
                res_points[i][j] = (1 - _v) * ((1 - _u) * control_points_[i][j] + _u * control_points_[i][j + 1]) + _v * ((1 - _u) * control_points_[i + 1][j] + _u * control_points_[i + 1][j + 1]);
            }
        }

        vec3 res_points2[2][2];

        for (unsigned int i = 0; i < 2; ++i) {
            for (unsigned int j = 0; j < 2; ++j) {
                res_points2[i][j] = (1 - _v) * ((1 - _u) * res_points[i][j] + _u * res_points[i][j + 1]) + _v * ((1 - _u) * res_points[i + 1][j] + _u * res_points[i + 1][j + 1]);
            }
        }

        p = (1 - _v) * ((1 - _u) * res_points2[0][0] + _u * res_points2[0][1]) + _v * ((1 - _u) * res_points2[1][0] + _u * res_points2[1][1]);

        vec3 p1 = (1 - _u) * res_points2[0][0] + _u * res_points2[0][1];
        vec3 p2 = (1 - _u) * res_points2[1][0] + _u * res_points2[1][1];
        vec3 p1p2 = p2 - p1;
        
        vec3 p3 = (1 - _v) * res_points2[0][0] + _v * res_points2[1][0];
        vec3 p4 = (1 - _v) * res_points2[0][1] + _v * res_points2[1][1];
        vec3 p3p4 = p4 - p3;

        n = normalize(cross(p1p2, p3p4));
    }
    else {
        for (unsigned int i = 0; i < 4; ++i) {
            for (unsigned int j = 0; j < 4; ++j) {
                float B1 = bernstein(i, 3, _u);
                float B2 = bernstein(j, 3, _v);
                p += control_points_[i][j] * B1 * B2;
            }
        }

        vec3 p12;
        vec3 p34;

        for (unsigned int i = 0; i < 3; ++i) {
            for (unsigned int j = 0; j < 4; ++j) {
                float B11 = bernstein(i, 2, _u);
                float B21 = bernstein(j, 3, _v);
                p12 += (control_points_[i+1][j] - control_points_[i][j]) * B11 * B21;
            }
        }

        p12 *= 4;

        for (unsigned int i = 0; i < 4; ++i) {
            for (unsigned int j = 0; j < 3; ++j) {
                float B12 = bernstein(i, 3, _u);
                float B22 = bernstein(j, 2, _v);
                p34 += (control_points_[i][j+1] - control_points_[i][j]) * B12 * B22;
            }
        }

        p34 *= 4;

        n = normalize(cross(p12, p34));
    }

    // copy resulting position and normal to output variables
    _p = p;
    _n = n;
}

//-----------------------------------------------------------------------------

void Bezier_patch::tessellate(unsigned int _resolution)
{
    // just to get slightly cleaner code below...
    const unsigned int N = _resolution;
    /** \todo Tessellate the Bezier patch into a set of triangles.
     *   This requires the following steps (with `N` being `_resolution`):
     *   - Evaluate the Bezier patch at a regular `N`x`N` grid of
     *   parameter values (`u`, `v`) within the unit square [0,1]x[0,1].
     *   So for example `N == 2` would sample `u` and `v` at 0.0 and 1.0
     *   and `N == 3` at 0.0, 0.5 and 1.0 and so on.
     *   For every parameter (`u`,`v`) compute the position and normal
     *   of the Bezier patch using the function `position_normal(u,v,p,n)`.
     *   Store the resulting `N`x`N` grids of points and normals in a
     *   linearized manner in the arrays `surface_vertices_[]` and
     *   `surface_normals_[]`, which in the end should have `N`*`N` elements.
     *   - Connect these surface samples by triangles. Each quad in your
     *   regular grid can simply be split into
     *   two triangles. Hence, since we have `(N-1)*(N-1)` quads, which we
     *   split into `2*(N-1)*(N-1)` triangles, and every triangle needs
     *   3 indices, you should in the end have `6*(N-1)*(N-1)` entries
     *   in the index array `surface_triangles_`.
     *
     *   Hints:
     *   - After filling positions and normals, you should already see the
     *   mesh's points in drawmode "Points".
     *   - To get the tesselation right, it helps to draw a rectangular NxN grid
     *   with a small N on a sheet of paper to visualize which points should be
     *   connected to triangles.
     *
     *   The arrays that you produce (points, normals, indices) will be uploaded
     *   to OpenGL in `upload_opengl_buffers()` at the end of this function.
     */

    surface_vertices_.clear();
    surface_normals_.clear();
    surface_triangles_.clear();

    for (unsigned int i = 0; i < N; ++i)
    {
        for (unsigned int j = 0; j < N; ++j)
        {
            float u = (float)i / (N - 1.0);
            float v = (float)j / (N - 1.0);
            vec3 p, n;
            position_normal(u, v, p, n);
            surface_vertices_.push_back(p);
            surface_normals_.push_back(n);
        }
    }

    for (unsigned int i = 0; i < N-1; ++i)
    {
        for (unsigned int j = 0; j < N-1; ++j)
        {
            surface_triangles_.push_back(i + j * N);
            surface_triangles_.push_back(i + j * N + 1);
            surface_triangles_.push_back(i + (j+1) * N);
            surface_triangles_.push_back(i + j * N + 1);
            surface_triangles_.push_back(i + (j+1) * N + 1);
            surface_triangles_.push_back(i + (j+1) * N);
        }
    }
    


    // DELETE ME BEGIN

    // This code is just to see something after finishing the first task
    // it samples the bezier patch at 4 uv-positions (0,25,0.25),(0.25,0.75),(0.75,0.25) and (0.75,0.75)
    /*for (unsigned int i = 0; i < 2; ++i)
    {
        for (unsigned int j = 0; j < 2; ++j)
        {
            float u = i == 0 ? 0.25 : 0.75;
            float v = j == 0 ? 0.25 : 0.75;
            vec3 p, n;
            position_normal(u, v, p, n);
            surface_vertices_.push_back(p);
            surface_normals_.push_back(n);
        }
    }*/
    // DELETE ME END


    // test the results to avoid ulgy memory leaks

    if (surface_vertices_.size() != N * N)
    {
        std::cerr
            << "[Bezier_patch::tessellate] The number of surface vertices is "
               "wrong\n";
    }
    if (surface_normals_.size() != N * N)
    {
        std::cerr
            << "[Bezier_patch::tessellate] The number of surface normals is "
               "wrong\n";
    }
    if (surface_normals_.size() != surface_vertices_.size())
    {
        std::cerr
            << "[Bezier_patch::tessellate] The number of surface vertices "
               "and surface normals is different\n";
    }
    if (surface_triangles_.size() != 6 * (N - 1) * (N - 1))
    {
        std::cerr
            << "[Bezier_patch::tessellate] The number of triangle indices is "
               "wrong\n";
    }
    for (unsigned int i : surface_triangles_)
    {
        if (i >= std::min(surface_vertices_.size(), surface_normals_.size()))
        {
            std::cerr << "[Bezier_patch::tessellate] Triangle index " << i
                      << " >= number of vertices. This will lead to bad memory "
                         "access!\n";
        }
    }

    upload_opengl_buffers();
}

//-----------------------------------------------------------------------------

void Bezier_patch::upload_opengl_buffers()
{
    // generate buffers for control polygon
    if (!cpoly_vertex_array_)
    {
        glGenVertexArrays(1, &cpoly_vertex_array_);
        glGenBuffers(1, &cpoly_vertex_buffer_);
        glGenBuffers(1, &cpoly_index_buffer_);
    }

    // upload buffers for control polygon
    if (cpoly_vertex_array_)
    {
        glBindVertexArray(cpoly_vertex_array_);

        // positions
        glBindBuffer(GL_ARRAY_BUFFER, cpoly_vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, 3 * 16 * sizeof(float), control_points_,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // edge indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cpoly_index_buffer_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     control_edges_.size() * sizeof(GLuint), &control_edges_[0],
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    // generate buffers for surface mesh
    if (!surf_vertex_array_)
    {
        glGenVertexArrays(1, &surf_vertex_array_);
        glGenBuffers(1, &surf_vertex_buffer_);
        glGenBuffers(1, &surf_normal_buffer_);
        glGenBuffers(1, &surf_index_buffer_);
    }

    // upload buffers for surface mesh
    if (surf_vertex_array_)
    {
        glBindVertexArray(surf_vertex_array_);

        // positions
        glBindBuffer(GL_ARRAY_BUFFER, surf_vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, surface_vertices_.size() * sizeof(vec3),
                     &surface_vertices_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // normals
        glBindBuffer(GL_ARRAY_BUFFER, surf_normal_buffer_);
        glBufferData(GL_ARRAY_BUFFER, surface_normals_.size() * sizeof(vec3),
                     &surface_normals_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        // triangle indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surf_index_buffer_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     surface_triangles_.size() * sizeof(GLuint),
                     &surface_triangles_[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
}

//-----------------------------------------------------------------------------

void Bezier_patch::draw_control_polygon()
{
    // did we generate OpenGL buffers?
    if (!cpoly_vertex_array_)
    {
        upload_opengl_buffers();
    }

    // draw control points & control polygon
    glBindVertexArray(cpoly_vertex_array_);
    glPointSize(7);
    glDrawArrays(GL_POINTS, 0, 16);
    glDrawElements(GL_LINES, control_edges_.size(), GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------

void Bezier_patch::draw_surface(std::string drawmode, bool upload)
{
    // did we tessellate?
    if (surface_vertices_.empty())
    {
        tessellate(20);
    }

    // did we generate OpenGL buffers?
    if (!surf_vertex_array_ || upload)
    {
        upload_opengl_buffers();
    }

    glBindVertexArray(surf_vertex_array_);

    // draw tessellated Bezier patch
    if (!surface_triangles_.empty() && drawmode != "Points")
    {
        glDrawElements(GL_TRIANGLES, surface_triangles_.size(), GL_UNSIGNED_INT,
                       NULL);
    }
    else if (!surface_vertices_.empty())
    {
        glPointSize(3);
        glDrawArrays(GL_POINTS, 0, surface_vertices_.size());
    }

    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------

float Bezier_patch::pick(const vec2 &coord2d, const mat4 &mvp)
{
    float closest_dist = FLT_MAX;

    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            const vec3 p = control_points_[i][j];
            vec4 ndc = mvp * vec4(p, 1.0f);
            ndc /= ndc[3];

            const float d = distance(vec2(ndc[0], ndc[1]), coord2d);
            if (d < closest_dist)
            {
                closest_dist = d;
                selected_control_point_ = (int)(i * 4 + j);
            }
        }
    }

    return closest_dist;
}

vec3 Bezier_patch::get_selected_control_point()
{
    return control_points_[selected_control_point_ / 4]
                          [selected_control_point_ % 4];
}

void Bezier_patch::set_selected_control_point(const vec3 &p)
{
    control_points_[selected_control_point_ / 4][selected_control_point_ % 4] =
        p;
}

//-----------------------------------------------------------------------------

void Bezier_patch::toggle_de_Casteljau()
{
    use_de_Casteljau_ = !use_de_Casteljau_;
}
//=============================================================================
