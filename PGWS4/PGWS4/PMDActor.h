#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<string>
#include<wrl.h>

class Dx12Wrapper;
class PMDRenderer;

class PMDActor
{
	friend PMDRenderer;
private:
	PMDRenderer& _renderer;
	Dx12Wrapper& _dx12;

	// �}�e���A���f�[�^
	struct MaterialForHlsl// �V�F�[�_�[���ɓ�������}�e���A���f�[�^
	{
		DirectX::XMFLOAT3 diffuse; // �f�B�t���[�Y�F
		float alpha; // �f�B�t���[�Y��
		DirectX::XMFLOAT3 specular; // �X�y�L�����F
		float specularity; // �X�y�L�����̋����i��Z�l�j
		DirectX::XMFLOAT3 ambient; // �A���r�G���g�F
	};

	struct AdditionalMaterial// ����ȊO�̃}�e���A���f�[�^
	{
		std::string texPath; // �e�N�X�`���t�@�C���p�X
		int toonIdx; // �g�D�[���ԍ�
		bool edgeFlg; // �}�e���A�����Ƃ̗֊s���t���O
	};

	struct Material// �S�̂��܂Ƃ߂�f�[�^
	{
		unsigned int indicesNum; // �C���f�b�N�X��
		MaterialForHlsl material;
		AdditionalMaterial additional;
	};
	unsigned int _materialNum; // �}�e���A����
	std::vector<Material> materials;
	Microsoft::WRL::ComPtr<ID3D12Resource> _materialBuff = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> _textureResources;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> _sphResources;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> _spaResources;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> _toonResources;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _materialDescHeap = nullptr;//�}�e���A���q�[�v(5�Ԃ�)

	//���_
	Microsoft::WRL::ComPtr<ID3D12Resource> _vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _idxBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _vbView = {};
	D3D12_INDEX_BUFFER_VIEW _ibView = {};

	// ���W�ϊ�
	struct Transform {
		//�����Ɏ����Ă�XMMATRIX�����o��16�o�C�g�A���C�����g�ł��邽��
		//Transform��new����ۂɂ�16�o�C�g���E�Ɋm�ۂ���
		void* operator new(size_t size);
		DirectX::XMMATRIX world;
	};
	Transform _transform;
	Transform* _mappedTransform = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _transformBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _transformMat = nullptr;//���W�ϊ��s��(���̓��[���h�̂�)
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _transformHeap = nullptr;//���W�ϊ��q�[�v

	float _angle;//�e�X�g�pY����]

private:
	// [6] �`�������W���
    // �|���S����U��񂵂Ă݂悤
	void RevolutionObj(float* angle, DirectX::XMMATRIX* worldMat);

public:
	// �������̕�������
	HRESULT LoadPMDFile(const char* path);//PMD�t�@�C���̃��[�h
	void CreateMaterialData();//�ǂݍ��񂾃}�e���A�������ƂɃ}�e���A���o�b�t�@���쐬
	void CreateMaterialAndTextureView();//�}�e���A�����e�N�X�`���̃r���[���쐬
	void CreateTransformView();

public:
	PMDActor(const char* filepath, PMDRenderer& renderer);
	~PMDActor();

	///�N���[���͒��_����у}�e���A���͋��ʂ̃o�b�t�@������悤�ɂ���
	PMDActor* Clone();

	void Update();
	void Draw();
};
