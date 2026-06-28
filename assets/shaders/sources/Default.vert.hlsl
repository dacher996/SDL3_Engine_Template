struct SpriteData
{
    float2 Position;
    float2 Scale;
    float2 Padding;
    float Rotation;
    float TexId;
    float TexU, TexV, TexW, TexH;
    float4 Color;
};

struct Output
{
    uint TexIndex : TEXCOORD0;
    float2 Texcoord : TEXCOORD1;
    float4 Color : TEXCOORD2;
    float4 Position : SV_Position;
};

// WARNING: StructuredBuffers are not natively supported by SDL's GPU API.
// They will work with SDL_shadercross because it does special processing to
// support them, but not with direct compilation via dxc.
// See https://github.com/libsdl-org/SDL/issues/12200 for details.
StructuredBuffer<SpriteData> DataBuffer : register(t0, space0);

// Stores indices that point to the DataBuffer to reduce copied data.
// Basically a workaround around the indexed drawing with 1 instance.
StructuredBuffer<uint> IndexBuffer : register(t1, space0);

cbuffer UniformBlock : register(b0, space1)
{
    float4x4 ViewProjectionMatrix : packoffset(c0);
};

static const uint triangleIndices[6] = {0, 1, 2, 3, 2, 1};
static const float2 vertexPos[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f}
};

Output main(uint id : SV_VertexID)
{
    uint spriteIndex = id / 6;
    uint vert = triangleIndices[id % 6];
    uint realIndex = IndexBuffer[spriteIndex];
    SpriteData sprite = DataBuffer[realIndex];

    float2 texcoord[4] = {
        {sprite.TexU,               sprite.TexV              },
        {sprite.TexU + sprite.TexW, sprite.TexV              },
        {sprite.TexU,               sprite.TexV + sprite.TexH},
        {sprite.TexU + sprite.TexW, sprite.TexV + sprite.TexH}
    };

    float c = cos(sprite.Rotation);
    float s = sin(sprite.Rotation);

    float2 coord = vertexPos[vert];
    coord *= sprite.Scale;
    float2x2 rotation = {c, s, -s, c};
    coord = mul(coord, rotation);

    float3 coordWithDepth = float3(coord + sprite.Position.xy, 1.0f);

    Output output;

    output.Position = mul(ViewProjectionMatrix, float4(coordWithDepth, 1.0f));
    output.TexIndex = uint(int(sprite.TexId));
    output.Texcoord = texcoord[vert];
    output.Color = sprite.Color;

    return output;
}
