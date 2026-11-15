// Grass wave shader.
// The top of the grass sways in a travelling wave while the root stays
// still. Displacement grows quadratically with height (non-linear) so the
// blade curves rather than shearing.

uniform float u_time;

Vertex(Vec2 position, Vec2 uv){
    // Wave configuration (local constants).
    float wave_frequency       = 2.0;   // Base ripple frequency across the field.
    float wave_speed           = 1.0;   // Base ripple travel speed.
    float secondary_frequency  = 6.0;  // Secondary ripple frequency (layering).
    float secondary_speed      = 2.0;   // Secondary ripple travel speed.
    float secondary_amount     = 8;  // Secondary ripple amplitude.
    float bend_amount          = 10.0;  // Maximum horizontal bend at the tip.

    // Height along the blade: 0 at the root, 1 at the tip.
    // (aUV.y is 1 at the base and 0 at the tip, so invert it.)
    float height = 1.0 - uv.y;

    // Travelling ripple across the field. The second sine creates a layered,
    // less-uniform sway rather than a single rigid wave.
    float wave = sin(uv.x * wave_frequency - u_time * wave_speed)
               + secondary_amount
                   * sin(uv.x * secondary_frequency - u_time * secondary_speed);

    // Non-linear (quadratic) ease: zero motion at the root, maximum at the
    // tip. The squared term makes the bend curve instead of shearing.
    float bend = wave * bend_amount * height * height;

    Vec2 swayed = Vec2(position.x + bend - 0.5 * height, position.y);

    vertex_output(swayed);
}

Fragment(Vec2 uv){
    // Grass colouring (local constants).
    Vec4 tip_color  = Vec4(0.35, 0.75, 0.15, 1.0);
    Vec4 root_color = Vec4(0.05, 0.30, 0.05, 1.0);

    // Sharpness of the parabolic blade point.
    float taper_amount = 1.0;

    // uv.y is 1 at the root and 0 at the tip, so t runs 0 (root) -> 1 (tip).
    float t = 1.0 - uv.y;

    // Horizontal distance from the blade centre, -1 (left) .. 1 (right).
    float px = 2.0 * (uv.x - 0.5);

    // Blade profile: a parabola. A fragment is inside the blade when
    // px*px <= (1 - t), i.e. the blade narrows parabolically to a point.
    float alpha = 1.0;
    if (px * px > taper_amount * (1.0 - t)) {
        alpha = 0.0;
    }

    Vec4 grass = root_color * (1.0 - t) + tip_color * t;
    grass = Vec4(grass.x, grass.y, grass.z, alpha);

    fragment_output(grass);
}

