struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Position : SV_Position;
};

// The screen rectangle is passed via a uniform buffer at slot 0 (register b0, space1).
// The float4 contains [x, y, w, h] where:
// - x, y are the top-left corner coordinates of the target rectangle in normalized screen space [0.0, 1.0].
// - w, h are the width and height of the target rectangle in normalized screen space [0.0, 1.0].
cbuffer UniformBlock : register(b0, space1)
{
    float4 ScreenRect;
};

// We draw a single quad using 6 vertices (2 triangles) to represent the render target on the screen.
// These static arrays define the topology (indices) and layout positions of the quad corners.
static const uint triangleIndices[6] = {0, 1, 2, 3, 2, 1};

// vertexPos maps each corner index to coordinates in [0.0, 1.0] range:
// 0: Top-Left (0.0, 0.0)
// 1: Top-Right (1.0, 0.0)
// 2: Bottom-Left (0.0, 1.0)
// 3: Bottom-Right (1.0, 1.0)
static const float2 vertexPos[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f}
};

Output main(uint id : SV_VertexID)
{
    // Map the VertexID (0 to 5) to the quad corner index (0 to 3)
    uint vert = triangleIndices[id % 6];
    
    // Get the normalized vertex position (0.0 to 1.0) for this corner
    float2 normCoord = vertexPos[vert];
    
    // Map the texture coordinates directly from the corner position.
    // This maps the full bound texture to the destination screen rectangle.
    float2 texcoord = normCoord;
    
    // Scale and translate the normalized position using the ScreenRect parameters.
    // ScreenRect.xy represents the top-left offset (x, y)
    // ScreenRect.zw represents the scale/size (w, h)
    float2 screenPos = ScreenRect.xy + normCoord * ScreenRect.zw;
    
    // Convert the normalized screen position [0.0, 1.0] to Normalized Device Coordinates (NDC) [-1.0, 1.0]
    // X axis: [0.0, 1.0] maps to [-1.0, 1.0] (left to right)
    // Y axis: [0.0, 1.0] maps to [1.0, -1.0] (top is 1.0, bottom is -1.0 in NDC)
    float4 position;
    position.x = screenPos.x * 2.0f - 1.0f;
    position.y = 1.0f - screenPos.y * 2.0f;
    position.z = 0.0f;
    position.w = 1.0f;
    
    Output output;
    output.Texcoord = texcoord;
    output.Position = position;
    
    return output;
}

