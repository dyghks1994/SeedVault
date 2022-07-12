#pragma pack_matrix( row_major )

#include "Deferred.hlsl"
#include "PBRLibrary.hlsli"
#include "Utility.hlsli"

cbuffer Light : register(b0)
{
    Light lightInfo[24];
}

cbuffer LightCount : register(b1)
{
    uint4 lightType[6];
    uint lightCount;
}

cbuffer objectPer : register(b2)
{
    float4 EyePos;
}

cbuffer view : register(b3)
{
    float4x4 viewInv;
}

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 ToFarPlane : TEXCOORD0;
    float2 Tex : TEXCOORD1;
};

struct OUT_TARGET
{
    float4 accum : SV_Target0;
    float4 reveal : SV_Target1;
};

Texture2D textureAlbedo : TEXTURE : register(t0);
Texture2D textureEmissive : TEXTURE : register(t1);
Texture2D textureNormal : TEXTURE : register(t2);
Texture2D textrueNormalDepth : TEXTURE : register(t3);
Texture2D textureMaterial : TEXTURE : register(t4);
Texture2D textureAmbient : TEXTURE : register(t5);
Texture2D textureShadow : TEXTURE : register(t6);
Texture2D textureLight : TEXTURE : register(t7);
Texture2D SSAOMap : TEXTURE : register(t8);

SamplerState pointSampler : SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);

OUT_TARGET main(VS_OUTPUT pin) : SV_TARGET
{
    OUT_TARGET nowtarget;
    
    float4 color = float4(textureAlbedo.Sample(objSamplerState, pin.Tex));
    float4 emissive = uint4(textureEmissive.Sample(objSamplerState, pin.Tex));
    float4 normal = float4(textureNormal.Sample(objSamplerState, pin.Tex));
    float4 material = float4(textureMaterial.Sample(objSamplerState, pin.Tex));
    float4 ambient = float4(textureAmbient.Sample(objSamplerState, pin.Tex));
    float4 shadow = float4(textureShadow.Sample(pointSampler, pin.Tex));
    float4 lightTexture = float4(textureLight.Sample(pointSampler, pin.Tex));
    
    
    float viewPosZ = float4(textrueNormalDepth.Sample(pointSampler, pin.Tex)).w;
    float _linearDepth = viewPosZ / pin.ToFarPlane.z;
    float4 position = CalcWorldPos(viewPosZ, pin.ToFarPlane, viewInv);
    
    float alphaStore = color.a;
    
    // ��ü�� ���� �ȼ��̶�� �ش� �÷����� �Ѱ��ش�
    if (isApproximatelyEqual(normal.w, 0.0))
    {
        float weight = clamp(pow(min(1.0, alphaStore * 10.0) + 0.01, 3.0) * 1e8 *
                         //pow(1.0 - position.z * 0.9, 3.0), 1e-2, 3e3);
                            pow(1.0 - _linearDepth * 0.9, 3.0), 1e-2, 3e3);

        nowtarget.accum = float4(color.rgb * alphaStore, alphaStore) * weight;
        nowtarget.reveal = alphaStore;
    
        return nowtarget;
    }
    
    // �ں��Ʈ�� �����Ѵ�
    float3 calcAmbient = float3(ambient.xyz);
    
    float metallic = material.r;
    float roughness = material.g;
    
    // 0~1�� ����� �븻�� �ٽ� -1~1�� ����Ѵ�
    float3 _nowNormal = (normal - 0.5f) * 2.0f;
    float3 toEyeW = normalize(EyePos.xyz - position.xyz);
    
    float3 result = 0.0f;
    // ����Ʈ�� ���� lightType[0];
    uint _round = 0;
    //[unroll]
    for (uint i = 0; i < lightCount; ++i)
    {
        uint nowType = lightInfo[_round].shadowInfo.x;
        uint isCastShadow = lightInfo[_round].shadowInfo.y;
        uint isUseTexture = lightInfo[_round].shadowInfo.w; // �ؽ��ĸ� ������ �ƴ��� �Ǵ�
        
        switch (isCastShadow)
        {
                // �׸��� ����
            case 0:
                switch (isUseTexture)
                {
                    case 0:
                        {
                            //result += CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz, -normalize(look.xyz), roughness, metallic, ambient.xyz).xyz;
                            result += CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                    case 1:
                        {
                            result += lightTexture.xyz *
                    CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                        
                }
                break;
            case 1:
                uint nowShadowMap = lightInfo[_round].shadowInfo.z;
                switch (isUseTexture)
                {

                    case 0:
                        {
                            result += shadow[nowShadowMap] *
                    CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                    case 1:
                        {
                            result += lightTexture.xyz * shadow[nowShadowMap] *
                    CalcLight(nowType, lightInfo[_round], color, position.xyz, _nowNormal.xyz,
                    toEyeW.xyz, roughness, metallic, ambient.xyz).xyz;
                            break;
                        }
                }
                break;
        }
        
        _round++;
        if (_round == lightCount)
            break;
       
    }
    
    // weight function
    float weight = clamp(pow(min(1.0, alphaStore * 10.0) + 0.01, 3.0) * 1e8 *
                         //pow(1.0 - position.w * 0.9, 3.0), 1e-2, 3e3);
                           pow(1.0 - _linearDepth * 0.9, 3.0), 1e-2, 3e3);

    // store pixel color accumulation
    nowtarget.accum = float4(result.rgb * alphaStore, alphaStore) * weight;

    // store pixel revealage threshold
    nowtarget.reveal.rgb = 0.0;
    nowtarget.reveal.w = alphaStore;
    
    return nowtarget;
}