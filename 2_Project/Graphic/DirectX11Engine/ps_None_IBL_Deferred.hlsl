#include "PBRLibrary.hlsli"
#include "Utility.hlsli"

cbuffer cbMaterial : register(b0)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float smoothness;
    float metallic;
    float normalFactor;
    float emissiveFactor;
};

// 이 아래는 매 프레임별로 한번만 밀어넣으면 되는 것
cbuffer cbLightMatrix : register(b1)
{
    float4x4 lightMatrix[4];
    uint4 shadowLightType;
    uint lightCount;
    // 현재 사용 중인 라이트의 개수, 라이트 타입도 넘어와야한다
}

cbuffer Camera : register(b2)
{
    float4 EyePos;
}

cbuffer ObjectID : register(b3)
{
    uint objectID;
}

cbuffer ibl : register(b4)
{
    float iblFactor;
}

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outWorldPos : POSITION0;
    float3 outNormal : NORMAL0;
    float2 outTexCoord : TEXCOORD0;
    float4 outTangent : TANGENT;
    float4 ShadowPosH[4] : TEXCOORD1;
    
    float3 outPosV : POSITION1;
    float3 outNormalDepth : NORMAL1;
    float2 outSSAOTex : TEXCOORD5;
};

struct PS_GBUFFER_OUT
{
    float4 Albedo : SV_TARGET0;
    float4 Emissive : SV_TARGET1;
    float4 NormalMap : SV_TARGET2;
    float4 NormalMapDepth : SV_TARGET3;
    float4 Material : SV_TARGET4; // FresnelR0 + roughness 
    float4 Ambient : SV_TARGET5; // Ambient + metallic + prefilter + irradiance
    float4 Shadow : SV_TARGET6;
    uint4 ObjectID : SV_TARGET7;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D objNormal : TEXTURE : register(t1);
Texture2D objMetalRough : TEXTURE : register(t2);
Texture2D objEmissvie : TEXTURE : register(t3);
Texture2D ShadowMap[4] : TEXTURE : register(t4);
Texture2D brdfLUT : TEXTURE : register(t8);
TextureCube irradianceMap : TEXTURE : register(t9);
TextureCube preFilterMap : TEXTURE : register(t10);

// 일단은 4장만 쓸 예정 기본적으로 DirectionalLight, SpotLight 두개가 될 것
// 실험 해볼것, 아마도 몇개가 들어갔는지 카운트도 포함되어야할듯

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerComparisonState PCFSample : SAMPLER : register(s1);

PS_GBUFFER_OUT main(VS_OUTPUT pin) : SV_Target
{
    PS_GBUFFER_OUT deferredOut;
     
    deferredOut.NormalMapDepth = float4(normalize(pin.outNormalDepth), pin.outPosV.z);
    
    // 알베도 맵이 없으면 Material의 알베도 맵을 가져와서 그려준다
    deferredOut.Albedo = objTexture.Sample(objSamplerState, pin.outTexCoord) * DiffuseAlbedo;
    float4 _emissive = objEmissvie.Sample(objSamplerState, pin.outTexCoord);
    
    if (isApproximatelyEqual(_emissive, 0.0))
    {
        _emissive = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // 이미시브맵
        _emissive = (_emissive * _emissive) * (emissiveFactor * 100);
    }
    
    deferredOut.Emissive = _emissive;
    
    // metal-rough니까 메탈이 먼저
    float4 metalRoughMap = objMetalRough.Sample(objSamplerState, pin.outTexCoord);
    
    float nowMetalFactor = saturate(metalRoughMap.r * metallic);
    float nowRoughnessFactor = saturate(metalRoughMap.g * smoothness);
    deferredOut.Material = float4(nowMetalFactor, nowRoughnessFactor, 0.0, 1.0);
    
    // 노말
    float3 normalMapSample = objNormal.Sample(objSamplerState, pin.outTexCoord).rgb;
    float _nowNormalFactor = (normalFactor * 0.5) + 0.5;
    
    if (length(normalMapSample) == 0.0f)
    {
        deferredOut.NormalMap = float4((normalize(pin.outNormal) * 0.5f + 0.5f), 1.0f);
    }
    else
    {
        pin.outNormal = NormalSampleToWorldSpace(normalMapSample, pin.outNormal, pin.outTangent);
        deferredOut.NormalMap = float4((pin.outNormal * 0.5f) + 0.5f, 1.0f);
    }
    
    
    {
        // IBL 이미지를 엠비언트로 변경
        // 조도맵을 추출
        float3 irradianceColor = irradianceMap.Sample(objSamplerState, pin.outNormal).rgb;
    
        float3 indirectDiffuse = irradianceColor * deferredOut.Albedo.xyz;
        deferredOut.Ambient.xyz = indirectDiffuse * iblFactor;
        deferredOut.Ambient.w = 1.0;
    }
    
    
    float shadow = 0.0f;
    float4 shadowValue = 1.0f;
    [unroll]
    for (uint i = 0; i < lightCount; i++)
    {
        uint _nowType = shadowLightType[i];
        
        float _origin = 0.0f;
        switch (_nowType)
        {
            case 0:
                _origin = CalcShadowFactor(PCFSample, ShadowMap[i], pin.ShadowPosH[i]);
                shadowValue[i] = saturate(_origin + 0.5);
                break;
            case 2:
                _origin = SpotShadowPCF(PCFSample, ShadowMap[i], pin.ShadowPosH[i]);
                shadowValue[i] = saturate(_origin + 0.5);
                break;
        }
    }
   
    deferredOut.Shadow = float4(shadowValue.x, shadowValue.y, shadowValue.z, shadowValue.w);
    deferredOut.ObjectID.x = objectID;
                                  
    return deferredOut;
}