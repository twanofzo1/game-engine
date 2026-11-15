// Example .shader file for the engine's C-like shader language.
// Compiled by ShaderCompiler into GLSL vertex + fragment sources.

uniform Vec2 u_resolution;
uniform Texture2D u_my_texture;
uniform float u_time;

Vertex(Vec2 position){
    vertex_output(Vec2(position.x * sin(u_time), position.y));
}

Fragment(Vec2 uv){
    Vec4 color = Vec4(uv.x, uv.y, 0.0, 1.0);
    if (uv.x < sin(u_time*0.1)*0.5 + 0.5) {
        color = texture(u_my_texture, uv);
    }

    fragment_output(color);
}
