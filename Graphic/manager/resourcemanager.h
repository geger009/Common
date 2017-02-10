// 게임에서 사용될 리소스를 관리하는 클래스
// 재활용 할 수 있는 리소스를 다시 로딩하지 않고 쓸 수 있게 한다.
// 재활용 리소스
// - model
// - animation
// - texture
// - shader
#pragma  once


namespace graphic
{
	struct sRawMesh;
	struct sRawAni;
	struct sRawBone;


	class cResourceManager : public common::cSingleton<cResourceManager>
	{
	public:
		cResourceManager();
		virtual ~cResourceManager();
		
		sRawMeshGroup* LoadModel( const string &fileName );
		bool LoadModel(sRawMeshGroup *meshes);
		sRawMeshGroup2* LoadModel2(const string &fileName);
		bool LoadModel2(sRawMeshGroup2 *meshes);
		sRawAniGroup* LoadAnimation( const string &fileName );
		bool LoadAnimation(sRawAniGroup *anies);
		cMeshBuffer* LoadMeshBuffer(cRenderer &renderer, const string &meshName);
		cMeshBuffer* LoadMeshBuffer(cRenderer &renderer, const sRawMesh &rawMesh);
		cTexture* LoadTexture(cRenderer &renderer, const string &fileName, const bool isSizePow2 = true);
		cTexture* LoadTexture(cRenderer &renderer, const string &dirPath, const string &fileName, const bool isSizePow2 = true);
		cShader* LoadShader(cRenderer &renderer, const string &fileName);

		sRawMeshGroup* FindModel( const string &fileName );
		sRawMeshGroup2* FindModel2(const string &fileName);
		sRawAniGroup* FindAnimation( const string &fileName );
		cMeshBuffer* FindMeshBuffer( const string &meshName );
		cTexture* FindTexture( const string &fileName );
		cShader * FindShader(const string &fileName);
		string FindFile( const string &fileName );

		void SetMediaDirectory( const string &path );
		const string& GetMediaDirectory() const;
		string GetRelativePathToMedia( const string &fileName );
		void ReloadFile();
		void Clear();

		RESOURCE_TYPE::TYPE GetFileKind( const string &fileName );


	private:
		map<string, sRawMeshGroup*> m_meshes; // key = fileName
		map<string, sRawMeshGroup2*> m_meshes2; // key = fileName
		map<string, sRawAniGroup*> m_anies;	// key = fileName
		map<string, cMeshBuffer*> m_mesheBuffers; // key = meshName
		map<string, cTexture*> m_textures; // key = fileName
		map<string, cShader*> m_shaders; // key = fileName
		string m_mediaDirectory; // default : ../media/
		set<string> m_reLoadFile;
	};


	inline void cResourceManager::SetMediaDirectory( const string &path ) { m_mediaDirectory = path; }
	inline const string& cResourceManager::GetMediaDirectory() const { return m_mediaDirectory; }
}
