// QuadShader.hlsl - Draws one textured screen-space quad from a destination rect and UV rect

cbuffer QuadConstants : register(b0)
{
    float4 DestRect;   // x, y, width, height in pixels
    float4 UvRect;     // u0, v0, u1, v1
    float4 Tint;
    float2 Viewport;   // width, height in pixels
    float2 Pad;
};

Texture2D SourceTexture : register(t0);
SamplerState LinearClamp : register(s0);

struct VSOutput
{
    float4 Position : SV_Position;
    float2 Uv : TEXCOORD0;
};

VSOutput VSMain(uint vertexId : SV_VertexID)
{
    float2 corner = float2(vertexId & 1, vertexId >> 1);

    float2 pixel = DestRect.xy + corner * DestRect.zw;
    float2 ndc = float2(pixel.x / Viewport.x * 2.0f - 1.0f, 1.0f - pixel.y / Viewport.y * 2.0f);

    VSOutput output;
    output.Position = float4(ndc, 0.0f, 1.0f);
    output.Uv = lerp(UvRect.xy, UvRect.zw, corner);
    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return SourceTexture.Sample(LinearClamp, input.Uv) * Tint;
}
