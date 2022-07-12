#pragma pack_matrix( row_major )

cbuffer FrustmConer : register(b0)
{
    float4 gFrustumCorners[4];
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 ToFarPlaneIndex : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 ToFarPlane : TEXCOORD0;
    float2 Tex : TEXCOORD1;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
	
	// Already in NDC space.
    vout.PosH = float4(vin.PosL, 1.0f);

	// We store the index to the frustum corner in the normal x-coord slot.
    vout.ToFarPlane = gFrustumCorners[vin.ToFarPlaneIndex.x].xyz;

	// Pass onto pixel shader.
    vout.Tex = vin.Tex;
	
    return vout;
}

/*
cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 worldMatrixInverse;
};

struct VS_2D_IN
{
    float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
    // ���ؽ� ���̴����� �������� �������ش�.
    // �� �������� �ٰŷ� �ȼ� ���̴��� ��ġ�� ����ش�.
    // ���� ��ȯ, ������(����) ��ȯ
    
    // ������ ������ �ȼ� ���̴��� ��ġ�� ����� �� ��. w ���� 0.0�̾��� ��.
    float4 Position : SV_Position; // vertex position 
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT main(VS_2D_IN vin)
{
    VS_OUTPUT Output;
    
    Output.Position = float4(mul(float4(vin.PosL, 1.0f), worldMatrix).xyz, 1.0f);
    Output.Tex = vin.Tex;
    
    return Output;
}
*/