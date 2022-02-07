#version 330
precision mediump float;

in vec3 v2f_normal;
in vec2 v2f_texcoord;
in vec3 v2f_light;
in vec3 v2f_view;

out vec4 f_color;

uniform sampler2D day_texture;
uniform sampler2D night_texture;
uniform sampler2D cloud_texture;
uniform sampler2D gloss_texture;
uniform bool greyscale;

const float shininess = 20.0;
const vec3  sunlight = vec3(1.0, 0.941, 0.898);

void main()
{
    /** \todo Implement a fancy earth shader.
    * - Copy your working code from the fragment shader of your Phong shader use it as starting point
    * - instead of using a single texture, use the four texures `day_texure`, `night_texure`,
    *   `cloud_texure` and `gloss_texture` and mix them for enhanced effects:
    *       * The `gloss_texture` defines how glossy (specular) a point on earth is
    *       * The `cloud_texture` defines how cloudy a point on earth is, and more clouds should produce less reflections at this point
    *       * `day_texture` and `night_texture` define the day and night color of the earth. Make sure that there is a soft transition between both.
    *       * Note that there is no phong shading at the night side.
    * Hints:
    *   - cloud and gloss textures are just greyscales. So you'll just need one color component.
    *   - The texture(texture, 2d_position) returns a 4-vector (rgba). You can use either `texture(...).r` to get just the red component
    *     or `texture(...).rgb` to get a vec3 color value
    *   - use mix(vec3 a,vec3 b, float s) = a*(1-s) + b*s for linear interpolation of two colors
    *   - Lookup the documentation of the function called `smoothstep(...)` it might be helpful.
    *   - There is not the one right way to get the desired results. Feel free to use some magic numbers or creative solutions.
     */
    vec3 tex_color_day = texture(day_texture, v2f_texcoord.st).rgb;
    vec3 tex_color_night = texture(night_texture, v2f_texcoord.st).rgb;
    vec3 tex_color_cloud = texture(cloud_texture, v2f_texcoord.st).rgb;
    vec3 tex_color_gloss = texture(gloss_texture, v2f_texcoord.st).rgb;

    float alpha_day = texture(day_texture, v2f_texcoord.st).a;
    float alpha_night = texture(night_texture, v2f_texcoord.st).a;
    float alpha_cloud = texture(cloud_texture, v2f_texcoord.st).a;
    float alpha_gloss = texture(gloss_texture, v2f_texcoord.st).a;


    float temp = dot(v2f_light,v2f_normal);

    //ambient
    float temp1_2 = smoothstep(-0.5, 0.5, temp);
    tex_color_night *= 1-tex_color_cloud.r;
    vec3 color = mix(1.4*tex_color_night, .2*tex_color_day, temp1_2);

    if(temp > 0) {
        //diffuse
        color += sunlight*max(tex_color_day, tex_color_gloss.r*tex_color_cloud)*temp;

        //specular
        float temp2 = dot(v2f_view,normalize(reflect(-v2f_light,v2f_normal)));
        if(temp2 > 0) {
            color += tex_color_gloss.r*sunlight*tex_color_day*pow(temp2,shininess);
        }
    } 

//    vec3 color = .2*tex_color_day;
//    if(temp > 0) {
//        color += sunlight*max(tex_color_day, tex_color_cloud)*temp;
//        float temp2 = dot(v2f_view,normalize(reflect(-v2f_light,v2f_normal)));
//        if(temp2 > 0) {
//            color += .2*sunlight*tex_color_day*pow(temp2,shininess);
//        }
//    }
//    if(temp <= 0) {
//        color += sunlight*tex_color_night*-temp;
//        float temp2 = dot(v2f_view,normalize(reflect(-v2f_light,v2f_normal)));
//        if(temp2 <= 0) {
//            color += .2*sunlight*tex_color_night*pow(-temp2,shininess);
//        }
//    }
    
    // convert RGB color to YUV color and use only the luminance
    if (greyscale) color = vec3(0.299*color.r+0.587*color.g+0.114*color.b);

    // add required alpha value
    f_color = vec4(color, alpha_day);

}
