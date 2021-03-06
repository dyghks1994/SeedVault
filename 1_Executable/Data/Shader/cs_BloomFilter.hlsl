
Texture2D gInput : register(t0);
Texture2D gInput2 : register(t1);

RWTexture2D<float4> gOutput : register(u0);

cbuffer FinalPassConstants : register(b0)
{
    float huddle : packoffset(c0);
    float increase : packoffset(c0.y);
}

#define MAX_NUM 32
[numthreads(MAX_NUM, MAX_NUM, 1)]
void BloomFilter(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 _currPixel = dispatchThreadID.xy;
    float4 _nowColor = gInput[_currPixel];
    float4 _nullColor = float4(0, 0, 0, 1.0);
    
    // 크기가 일정 수치 이상이면
    if (length(_nowColor.rgb) >= huddle)
    {
        gOutput[_currPixel] = float4(_nowColor.rgb * increase, 1.0);
    }
    else
    {
        _nowColor = _nullColor;
        
        if (_nowColor.r >= huddle)
        {
            _nowColor.r = _nowColor.r * increase * increase;
        }
        if (_nowColor.g >= huddle)
        {
            _nowColor.g = _nowColor.g * increase * increase;
        }
        if (_nowColor.b >= huddle)
        {
            _nowColor.b = _nowColor.b * increase * increase;
        }
        
        
    }
    
    gOutput[_currPixel] = _nowColor;
    
}

[numthreads(MAX_NUM, MAX_NUM, 1)]
void BloomMerge(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 _currPixel = dispatchThreadID.xy;
    
    float4 _bloomColor = gInput[_currPixel];
    float4 _pixelColor = gInput2[_currPixel];
    
    float3 _nowColor = lerp(_bloomColor.rgb, _pixelColor.rgb, 0.5);
    
    gOutput[_currPixel] = float4(_nowColor.rgb, 1.0);
}