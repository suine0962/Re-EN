#include"DirectionalLight.hlsli"


struct Material
{
	float32_t4 color;
	float32_t shininess;
};
struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intensity;
};
ConstantBuffer<Material>gMaterial :register(b0);

struct Camera {
	float32_t3 worldPosition;
};

ConstantBuffer<Camera> gCamera:register(b2);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4>gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {

	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {

	PixelShaderOutput output;

	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);


	float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);

	float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

	float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));

	float RdotE = dot(reflectLight, toEye);

	float specularPow = pow(saturate(RdotE), gMaterial.shininess);

	//拡散反射
	float32_t3 diffuse =
		gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;

	//鏡面反射
	float32_t3 specular =
		gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);

	//拡散反射+鏡面反射
	output.color.rgb = diffuse + specular;

	//αは今まで通り
	output.color.a = gMaterial.color.a * textureColor.a;

	//output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	// gMaterial.color * textureColor;

	return output;
}