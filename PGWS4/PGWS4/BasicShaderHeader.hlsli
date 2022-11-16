// ���_�V�F�[�_����s�N�Z���V�F�[�_�ւ̂����Ɏg����
struct Output
{
	float4 svpos : SV_POSITION;   // �V�X�e���p���_���W
	float4 normal : NORMAL;   // �@���x�N�g��
	float2 uv : TEXCOORD;   // UV�l
};

Texture2D<float4> tex : register(t0); // 0 �ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); // 0 �ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

// �萔�o�b�t�@
cbuffer cbuff0 : register(b0)
{
	matrix world;   // ���[���h�ϊ��s��
	matrix viewproj;   // �r���[�v���W�F�N�V�����s��
	float lightAngle;
};

// �萔�o�b�t�@�[1
// �}�e���A���p
cbuffer Material : register(b1)
{
	float4 diffuse;	   // �f�B�t���[�Y�F
	float4 specular;   // �X�y�L����
	float3 ambient;	   // �A���r�G���g
};
