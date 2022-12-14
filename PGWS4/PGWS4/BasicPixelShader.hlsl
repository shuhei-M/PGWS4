#include "BasicShaderHeader.hlsli"

#define TOON

float4 BasicPS(Output input) : SV_TARGET
{
	// ���֌������x�N�g���i���s�����x�N�g���j
	float3 light = normalize(float3(cos(lightAngle), -1, sin(lightAngle)));  //cos(lightAngle), -1, sin(lightAngle)//(1, -1, 1)

	// ���C�g�̃J���[�i�^�����j
	float3 lightColor = float3(1.0, 1.0, 1.0) * 0.7;

	// �f�B�t���[�Y�v�Z
	float3 normal = normalize(input.normal.xyz);
	float diffuseB = saturate(dot(-light, normal));

	// ���̔��˃x�N�g��
	float3 refLight = normalize(reflect(light, normalize(input.normal.xyz)));
	float specularB = pow(saturate(dot(refLight, -normalize(input.ray))), specular.a);

	// �X�t�B�A�}�b�v�puv
	float2 sphereMapUV = input.vnormal.xy * float2(0.5, -0.5) + 0.5;

	// �e�N�X�`���J���[
	float4 texColor = tex.Sample(smp, input.uv);

#ifdef TOON
	float3 toonDif = toon.Sample(smpToon, float2(0, 1.0 - diffuseB)).rgb;
	float3 toonSpe = toon.Sample(smpToon, float2(0, 1.0 - specularB)).rgb;
	return float4(lightColor * // ���C�g�J���[
		(texColor.rgb // �e�N�X�`���J���[
			* sph.Sample(smp, sphereMapUV).rgb // �X�t�B�A�}�b�v�i��Z�j
			* (ambient + toonDif * diffuse.rgb) // �����{�f�B�t���[�Y�F
			+ spa.Sample(smp, sphereMapUV).rgb // �X�t�B�A�}�b�v�i���Z�j
			+ toonSpe * specular.rgb) // �X�y�L����
		, diffuse.a); // �A���t�@
#endif // TOON
}