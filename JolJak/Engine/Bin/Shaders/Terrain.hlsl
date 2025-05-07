// === 상수 버퍼 ===
cbuffer ObjectCB : register(b0)
{
    float4x4 gWorld;
};

cbuffer PassCB : register(b1)
{
    float4x4 gViewProj;
    float3 gEyePosW;
    float pad;
};

Texture2D gTextures[3] : register(t1);
SamplerState gsamLinear : register(s0);

struct VertexIn
{
    float2 PosXZ : POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    // heightmap 샘플링
    float height = gTextures[0].Sample(gsamLinear, vin.TexC).r;

    float3 posW = float3(vin.PosXZ.x, height, vin.PosXZ.y);

    // 월드 변환
    posW = mul(float4(posW, 1.0f), gWorld).xyz;

    vout.PosW = posW;
    vout.PosH = mul(float4(posW, 1.0f), gViewProj);
    vout.TexC = vin.TexC;

    // normal map의 Z축 normal을 기본 normal로 임시 지정
    float3 normalT = float3(0, 1, 0); // 나중에 normal map 적용
    vout.NormalW = normalize(mul(normalT, (float3x3) gWorld));

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // normal map 샘플링
    float3 normalSample = gTextures[2].Sample(gsamLinear, pin.TexC).rgb * 2.0f - 1.0f;

    // tangent space → world space (tangent 없으니 그냥 y축 normal로 대체)
    float3 normalW = normalize(mul(normalSample, (float3x3) gWorld));

    // diffuse map 샘플링
    float4 diffuseColor = gTextures[1].Sample(gsamLinear, pin.TexC);

    // 간단한 조명 계산 (디렉셔널 라이트 1개)
    float3 lightDir = normalize(float3(0.577f, -0.577f, 0.577f));
    float diffuseFactor = saturate(dot(normalW, -lightDir));
    float3 ambient = 0.1f * diffuseColor.rgb;
    float3 diffuse = diffuseFactor * diffuseColor.rgb;

    float3 finalColor = ambient + diffuse;

    return float4(finalColor, diffuseColor.a);
}