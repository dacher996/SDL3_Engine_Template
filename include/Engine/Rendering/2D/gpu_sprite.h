#ifndef ENGINE_GPU_SPRITE_H
#define ENGINE_GPU_SPRITE_H

namespace Engine {
    /// Structure used for sprite data transfers between CPU and GPU
    struct GPUSprite {
        /// The point in 2D space where the sprite will be drawn
        float x, y;

        /// The size of the final output when drawing the sprite
        float w, h;

        /// Padding included to comply with std140's 16-byte alignment
        float padding_a, padding_b;

        /// Angle at which the sprite is rendered
        float rotation;

        /// The layer within the texture where the sprite is located
        float textureLayerId;

        /// The rect within the texture layer which will be drawn (x,y,width,height)
        float tex_u, tex_v, tex_w, tex_h;

        /// Color applied to the output sprite
        float r, g, b, a;
    };
}

#endif //ENGINE_GPU_SPRITE_H
