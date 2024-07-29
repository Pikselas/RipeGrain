struct VertexShaderOut // this is the ouput type
{
    float2 tex : TEXCOORD; // this is the user defined semantic for color which will be passed to pixel shader
    float4 pos : SV_POSITION; // this is the SYSTEM_VALUE SEMANTIC this is fixed (and defined by the API) 
};


cbuffer sprite_transform
{
    matrix transform;
};

cbuffer screen_size
{
    float2 half_screen_size;
    float2 padding;
};

cbuffer texture_coord
{
    float2 coord;
    float2 size;
};

VertexShaderOut main(float2 pos : POSITION)
{
    VertexShaderOut Out;
    
    Out.tex.x = coord.x + size.x * clamp(sign(pos.x), 0, 1);
    Out.tex.y = coord.y + size.y * clamp(sign(pos.y), 0, 1);
    
    pos = mul(transform, float4(pos, 0.0f, 1.0f)).xy;
    pos /= half_screen_size;
    pos.x -= 1.0f;
    pos.y = 1.0f - pos.y;
    Out.pos = float4(pos , 0.0f , 1.0f);
    return Out;
}