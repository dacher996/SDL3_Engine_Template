// The fragment shader binds a 2D Texture representing the offscreen render target.
// Note that this is a standard 2D texture (t0) and NOT a 2D Array texture,
// since the render target is single-layered.
Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
    float2 TexCoord : TEXCOORD0;
};

float4 main(Input input) : SV_Target0
{
    // Sample the 2D texture using the interpolated texture coordinates from the vertex shader.
    // The sampler state uses linear filtering with clamping.
    return Texture.Sample(Sampler, input.TexCoord);
}

