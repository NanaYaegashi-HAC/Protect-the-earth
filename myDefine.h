//各種定義をこっちに記載する

//プレイヤー関連
const float PlayerInitPosX = 100;
const float PlayerInitPosY = 540 / 2;
const float PlayerMoveSpeed = 6.0f;
const float PI = 3.14159265359f;                 //円周率
const float Deg2Rad = PI / 180.0f;               //度からラジアンに変換する定数
const float Rad2Deg = 180.0f / PI;

const int EnemiesMax = 10;                       //敵の数
const int PlayerBulletsMax = 300;                //自機の弾数MAX
const int EnemyBulletsMax = 2000;                //敵の弾数MAX
const int ExplosionMax = 100;                    //爆発エフェクトMAX
const float ExplosionSize = 64.0f;               //爆発エフェクトサイズ
const int PLayerInitLife = 20;                    //自機のライフ初期値
const int InvincibleTime = 120;                  //無敵時間

const float PlayerBulletCollisionRadius = 16.0f; //プレイヤー弾当たり判定の半径
const float PlayerCollisionRadius = 3.0f;        //プレイヤー当たり判定の半径
const float EnemyCollisionRadius = 32.0f;        //雑魚敵当たり判定の半径
const float EnemyBulletCollisionRadius = 8.0f;   //敵弾当たり判定の半径

const int BossLife = 100;
const float BossCollisionRadius = 90.0f;
const int SwoonTime = 120;
const int DyingTime = 160;

const float PlayerImageSizeX = 64.0f;            //自機のxの大きさ
const float PlayerImageSizeY = 64.0f;            //自機のyの大きさ

//画像解像度
const int ScreenWidth = 960;  //幅
const int ScreenHeight = 540; //高さ

const int Width = 1000;
const int Height = 17;
const int CellSize = 32;
const float ScrollSpeed = 1.5f;

//数学関連
const float Sqrt2 = 1.41421356237f;  //ルート2

//自機構造体
struct PLAYER
{
	float x;               //x座標
	float y;               //y座標
	float moveSpeed;       //移動速度
	float collisionRadius; //当たり判定の半径
	int isDead;            //死亡フラグ
	int life;              //耐久力
	int InvincibleTimer;   //残り無敵時間。0以下なら無敵ではない
};

//自機弾構造体
struct PlayerBullet
{
	float speed;           //移動速度
	float x;               //x座標
	float y;               //y座標
	float vx;              //x方向移動速度
	float vy;              //y方向移動速度
	float collisionRadius; //当たり判定の半径
	int isDead;            //死亡フラグ(1:未使用 0:使用)
	float angle;           //角度
};

struct EnemyBullet
{
	float speed;           //移動速度
	float x;               //x座標
	float y;               //y座標
	float vx;              //x方向移動速度
	float vy;              //y方向移動速度
	float collisionRadius; //当たり判定の半径
	int isDead;            //死亡フラグ(1:未使用 0:使用)
	int type;              //タイプ
	int imageHandle;       //イメージハンドル
};

//敵構造体
struct Enemy
{
	float moveSpeed;       //移動速度
	float x;               //x座標
	float y;               //y座標
	float vx;              //x方向移動速度
	float vy;              //y方向移動速度
	float collisionRadius; //当たり判定
	int isDead;            //死亡フラグ
	int life;              //耐久力
	int type;              //タイプ
	int imageHandle;       //イメージハンドル
	int counter;           //時間を計るための変数
	int state;
	float angle;
	int baseY;
	int coolTime;
};

enum ENEMY_TYPE
{
	eSTRAIGHT = 0,
	eCOMING,
	eUPDOWN,
	eUPDOWN_REVERSE,
	eSTRAIGHT_SHOOT,
	eCOMING_SHOOT,
	eHARD_ENEMY,
	eRAPIDFIRE_ENEMY,
	e3WAYSHOOT,

	eBOSS = 100,
};

//ボスの状態
enum BossState
{
	Standby, //待機
	Appear,  //登場
	Normal,  //通常時
	Swoon,   //気絶時
	Angry,   //発狂モード
	Dying,   //死亡
};

//ボス
struct BOSS
{
	Enemy enemy;
	int swoonTime;
	int dyingTime;
	int explosionWaitCnt;
	float centerX;
	float centerY;
	int normalTime;
	int angryTime;
	float restartX;
	float restartY;
};

struct Explosion
{
	float x;        //x座標
	float y;        //y座表
	int isDead;     //死亡フラグ
	int counter;    //時間を計るための変数
	int imageIndex; //表示すべき画像の番号
};

//画像ハンドル
enum IMAGE_HANDLE
{
	eTitle,
	eClear,
	eGameover,
	eBack,
	ePLAYER,
	ePLAYERBULLET,
	eENEMYBULLET,
	eENEMY00,
	eENEMY01,
	eENEMY02,
	eENEMY03,
	eENEMY04,
	eENEMY05,
	eENEMY06,
	eENEMY07,
	eENEMY08,
	eENEMY09,
	eBOSS_NORMAL,
	eBOSS_SWOON,
	eBOSS_ANGRY,
	eEXPLOSION,

	eIMAGE_MAX
};

enum SOUND
{
	Damage,
	Leaser,
	BGM,
};

enum State
{
	StateTitle,
	StatePlaying,
	StateGameClear,
	StateGameover,
};