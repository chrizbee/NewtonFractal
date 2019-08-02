// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

uniform float EPS;          // Max error allowed
uniform int rootCount;      // Number of roots <= 10
uniform int maxIterations;  // Maximum number of iterations
uniform vec2 damping;       // Complex damping factor
uniform vec2 size;          // Width = x, height = y
uniform vec4 limits;        // Top = x, right = y, bottom = z, left = w
uniform vec2 roots[10];     // Real = x, imaginary = y
uniform vec3 colors[10];    // Rgba

vec2 cmpxcjg(vec2 c)
{
    // Complex conjugate
    return vec2(c.x, -c.y);
}

vec2 cmpxmul(vec2 a, vec2 b)
{
    // Complex multiplication
    return vec2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

vec2 cmpxdiv(vec2 a, vec2 b)
{
    // Complex division
    return cmpxmul(a, cmpxcjg(b)) / dot(b, b);
}

vec3 rgb2hsv(vec3 c)
{
    // Convert vec3.rgb to vec3.hsv
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    // Convert vec3.hsv to vec3.rgb
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 darker(vec3 c, float f)
{
    // Darken the color by f
    if (f > 1.0) {
        vec3 hsv = rgb2hsv(c);
        hsv.z = hsv.z / f;
        return hsv2rgb(hsv);

    // Lighten the color by 1/f
    } else if (f < 1.0) {
        vec3 hsv = rgb2hsv(c);
        hsv.y = hsv.y * f;
        return hsv2rgb(hsv);

    // Do nothing if f == 1.0
    } else return c;
}

void func(vec2 z, inout vec2 f, inout vec2 df)
{
    // Test if more than 2 roots
    if (rootCount < 2) return;

    // Calculate f and derivative with given roots
    vec2 r = z - roots[0];
    vec2 l = z - roots[1];
    for (int i = 1; i < rootCount - 1; ++i) {
        l = cmpxmul((z - roots[i + 1]), (l + r));
        r = cmpxmul(r, (z - roots[i]));
    }
    df = l + r;
    f = cmpxmul(r, (z - roots[rootCount - 1]));
}

void main()
{
    // Get complex number from pixel position and limits
    float xFactor = float(limits.y - limits.w) / float(size.x - 1.0);
    float yFactor = float(limits.z - limits.x) / float(size.y - 1.0);
    vec2 z = vec2(gl_FragCoord.x * xFactor + limits.w, (size.y - gl_FragCoord.y) * yFactor + limits.x);

    // Newton iteration
    for (int i = 0; i < maxIterations; ++i) {
        vec2 f, df;
        func(z, f, df);
        vec2 z0 = z - cmpxmul(damping, cmpxdiv(f, df));

        // Check which root was reached
        if (length(z0 - z) < EPS) {
            for (int r = 0; r < rootCount; ++r) {
                if (length(z0 - roots[r]) < EPS) {

                    // Color by root and number of iterations
                    gl_FragColor = vec4(darker(colors[r], float(50 + i * 8) / 100.0), 1.0);
                    return;
                }
            }
        }
        z = z0;
    }

    // No root -> black
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
