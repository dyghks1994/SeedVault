#pragma pack_matrix( row_major )

#include "Deferred.hlsl"
#include "Utility.hlsli"

Texture2D decalTexture : register(t0);
Texture2D normalDpeth : register(t1);	// ViewNormal과 viewPos.z 값이 저장된 텍스쳐

SamplerState linearSampler : SAMPLER : register(s0);

cbuffer matrixs : register(b0)
{
    float4x4 ViewInv;
    float4x4 decalWorldInv;
}
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 ToFarPlane : TEXCOORD0;
    float2 Tex : TEXCOORD1;
};


float4 main(VertexOut pin) : SV_TARGET
{
	
    // 뷰 스페이스 -> 월드 -> 로컬 순으로 바꾼다
    // 로컬에서 0.5를 더한 값으로 uv를 만든다
    
    // 이 쉐이더는 디퍼드 렌더링이 끝나고, 컴포짓이 발생하기 전에 처리되어야 한다
    float _nowDepth = normalDpeth.Sample(linearSampler, pin.Tex).w;
    float3 p = (_nowDepth / pin.ToFarPlane.z) * pin.ToFarPlane;
    
   
    float4 _objPos = mul(float4(p, 1.0), ViewInv);
    
    // 데칼 스페이스에서 월드의 로컬 포지션
    float4 _localPos = mul(_objPos, decalWorldInv);
    
    float3 _objAbs = abs(_localPos.xyz);
    clip(0.5 - _objAbs);
    float2 _decalUV = _localPos.xz + 0.5;

    float4 color = float4(decalTexture.Sample(linearSampler, _decalUV));
    
    //float dist = abs(_localPos.z);
    //float scaleDistance = max(dist * 2.0f, 1.0f);
    //float fadeOut = 1.0f - scaleDistance;
    //color.a *= fadeOut;
    
    color.rgb *=  10;
    return color;
}