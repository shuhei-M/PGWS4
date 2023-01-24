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
	DirectX::XMMATRIX* _mappedMatrices = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _transformBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _transformMat = nullptr;//���W�ϊ��s��(���̓��[���h�̂�)
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _transformHeap = nullptr;//���W�ϊ��q�[�v

	// �{�[���֌W
	std::vector<DirectX::XMMATRIX> _boneMatrices;   // GPU�փR�s�[���邽�߂̃{�[�����
	struct BoneNode
	{
		int boneIdx;   //�{�[���C���f�b�N�X
		DirectX::XMFLOAT3 startPos;  //�{�[����_(��]���S)
		std::vector<BoneNode*> children;   // �q�m�[�h
	};
	std::map<std::string, BoneNode> _boneNodeTable;   // ���O�ō��������ł���悤��
	
	void RecursiveMatrixMultipy(BoneNode& node, const DirectX::XMMATRIX& mat);
	
	/// �L�[�t���[���\����
	struct KeyFrame 
	{
		unsigned int frameNo;   //�t���[����(�A�j���[�V�����J�n����̌o�ߎ���)
		DirectX::XMVECTOR quaternion;   //�N�H�[�^�j�I��
		DirectX::XMFLOAT2 p1, p2;
		KeyFrame(
			unsigned int fno, const DirectX::XMVECTOR& q,
			const DirectX::XMFLOAT2 ip1, const DirectX::XMFLOAT2 ip2) :
			frameNo(fno),
			quaternion(q), p1(ip1), p2(ip2) {}
	};
	std::map<std::string, std::vector<KeyFrame>> _motiondata;
	
	DWORD _startTime;//�A�j���[�V�����J�n���_�̃~���b����
	unsigned int _duration = 0;
	void MotionUpdate();

	float _angle;//�e�X�g�pY����]

private:
	// ���]������
	void RotationActor(float rotation);

	// [6] �`�������W���
    // �|���S����U��񂵂Ă݂悤
	void RevolutionActor(float* angle, DirectX::XMMATRIX* worldMat);

	void UpLeftElbow();

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

	void LoadVMDFile(const char* filepath, const char* name);

	void Update();
	void Draw();

	void PlayAnimation();
};
