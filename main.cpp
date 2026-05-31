#define _CRT_SECURE_NO_WARNINGS
#include<GSgameLight.h>
#include "myUtil.h"
#include "myDefine.h"

class MyGame :public gslib::Game
{
public:
	//コンストラクタ
	MyGame() :gslib::Game{ ScreenWidth,ScreenHeight }{}

private:

	//変数宣言
	PLAYER player;
	PlayerBullet playerBullets[PlayerBulletsMax];
	Enemy enemy;
	Enemy enemies[EnemiesMax];
	EnemyBullet enemyBullets[EnemyBulletsMax];
	Explosion explosions[ExplosionMax];
	BOSS boss;
	State state;

	int theta = 0.0f;
	float radius = 100.0f;
	int der = 1;
	int enemyData[Height][Width];
	float position;
	int counter = 0;

	//ステージ初期状態にする
	void Reset()
	{
		//プレイヤーの初期化
		player.x = PlayerInitPosX;
		player.y = PlayerInitPosY;
		player.moveSpeed = PlayerMoveSpeed;
		player.collisionRadius = PlayerCollisionRadius;
		player.isDead = 0;
		player.life = PLayerInitLife;
		player.InvincibleTimer = 0;

		boss.enemy.isDead = 0;
		boss.enemy.state = Appear;
		boss.enemy.life = BossLife;
		boss.enemy.collisionRadius = BossCollisionRadius;
		boss.enemy.angle = 0.0f;

		position = 0;

		counter = 0;

		//自機弾データの初期化
		for (int i = 0; i < PlayerBulletsMax; i++)
		{
			playerBullets[i].x = 0;
			playerBullets[i].y = 0;
			playerBullets[i].isDead = 1;
		}

		//敵弾データの初期化
		for (int i = 0; i < EnemyBulletsMax; i++)
		{
			enemyBullets[i].x = 0;
			enemyBullets[i].y = 0;
			enemyBullets[i].isDead = 1;
		}

		//爆発エフェクト初期化
		for (int i = 0; i < ExplosionMax; i++)
		{
			explosions[i].isDead = 1;
		}

		gsPlayBGM(BGM);
		state = StateTitle;
	}

	//開始
	void start()override
	{
		gsLoadTexture(eTitle, "Image/shooting_Title.png");
		gsLoadTexture(eClear, "Image/shooting_clear.png");
		gsLoadTexture(eGameover, "Image/shooting_gameover.png");
		gsLoadTexture(eBack, "Image/shooting_back.png");
		gsLoadTexture(ePLAYER, "Image/player.png");
		gsLoadTexture(ePLAYERBULLET, "Image/player_bullet.png");
		gsLoadTexture(eENEMYBULLET, "Image/enemy_bullet_16.png");
		gsLoadTexture(eENEMY00, "Image/zako0.png");
		gsLoadTexture(eENEMY01, "Image/zako1.png");
		gsLoadTexture(eENEMY02, "Image/zako2.png");
		gsLoadTexture(eENEMY03, "Image/zako3.png");
		gsLoadTexture(eENEMY04, "Image/zako4.png");
		gsLoadTexture(eENEMY05, "Image/zako5.png");
		gsLoadTexture(eENEMY06, "Image/zako6.png");
		gsLoadTexture(eENEMY07, "Image/zako7.png");
		gsLoadTexture(eENEMY08, "Image/zako8.png");
		gsLoadTexture(eENEMY09, "Image/zako9.png");
		gsLoadTexture(eEXPLOSION, "Image/explosion.png");
		gsLoadTexture(eBOSS_NORMAL, "Image/boss1.png");
		gsLoadTexture(eBOSS_SWOON, "Image/boss2.png");
		gsLoadTexture(eBOSS_ANGRY, "Image/boss3.png");

		gsLoadSE(Damage, "Sound/damage.wav", 1, GWAVE_DEFAULT);
		gsLoadSE(Leaser, "Sound/don.wav", 2, GWAVE_DEFAULT);
		gsLoadBGM(BGM, "Sound/music_background.ogg", GS_TRUE);

		LoadEnemyPos("Map/stage1.csv");

		Reset();
	}

	//更新
	void update(float delta_time)override
	{
		if (state == StateTitle)
		{
			if (gsGetKeyState(GKEY_SPACE))
			{
				state = StatePlaying;
			}
		}

		else if (state == StatePlaying)
		{
			//プレイヤー更新処理
			UpdatePlayer();
			//プレイヤー弾更新処理
			UpdatePlayerBullets();
			//敵の動き
			UpdataEnemies(delta_time);
			//敵弾の更新処理
			UpdateEnemyBullets();
			//衝突判定
			CollisionDetection();
			//ボス
			UpdateBoss();
			//爆発エフェクト更新処理
			UpdateExplosion();
			//マップデータをスクロール
			Scroll(ScrollSpeed);

			if (boss.enemy.isDead)
			{
				counter++;
				if (counter >= 120)
				{
					state = StateGameClear;
				}
			}

			else if (player.isDead)
			{
				counter++;
				if (counter >= 120)
				{
					state = StateGameover;
				}
			}
		}
	}

	//プレイヤー更新処理
	void UpdatePlayer()
	{
		if (player.isDead)return;  //プレイヤーが死んでいたら何もしない

		//無敵時間カウントを減らす
		player.InvincibleTimer--;

		float vx = 0.0f;
		float vy = 0.0f;

		if (gsGetKeyState(GKEY_LEFT))
		{
			vx += -player.moveSpeed;
		}
		if (gsGetKeyState(GKEY_RIGHT))
		{
			vx += player.moveSpeed;
		}
		if (gsGetKeyState(GKEY_UP))
		{
			vy += -player.moveSpeed;
		}
		if (gsGetKeyState(GKEY_DOWN))
		{
			vy += player.moveSpeed;
		}

		//Zボタンで自機弾を発射
		if (gsGetKeyTrigger(GKEY_Z))
		{
			float speed = 23.0f;
			float bulletvx = speed;
			float angle = 0;
			ShotBullet(player.x, player.y, speed, angle);
			angle = 10;
			ShotBullet(player.x, player.y, speed, angle);
			angle = 350;
			ShotBullet(player.x, player.y, speed, angle);
		}

		//斜めに移動している場合
		//斜め移動も同じ速度になるように調整
		if (vx != 0 && vy != 0)
		{
			vx /= Sqrt2;
			vy /= Sqrt2;
		}

		player.x += vx;
		player.y += vy;

		if (player.x < 32)player.x = 32;
		if (player.x > ScreenWidth - PlayerImageSizeX + 32)player.x = ScreenWidth - PlayerImageSizeX + 32;
		if (player.y < 32)player.y = 32;
		if (player.y > ScreenHeight - PlayerImageSizeY + 32)player.y = ScreenHeight - PlayerImageSizeY + 32;
	}

	//自機弾移動
	void UpdatePlayerBullets()
	{
		for (int i = 0; i < PlayerBulletsMax; i++)
		{
			if (playerBullets[i].isDead) continue;
			playerBullets[i].x += playerBullets[i].vx;
			playerBullets[i].y += playerBullets[i].vy;

			if (playerBullets[i].x > ScreenWidth || playerBullets[i].x < 0 || playerBullets[i].y<0 || playerBullets[i].y>ScreenHeight)
			{
				playerBullets[i].isDead = 1;
			}
		}
	}

	//敵更新
	void UpdataEnemies(float delte_time)
	{
		static float theta = 0.0f;
		theta += 0.05f;

		for (int i = 0; i < EnemiesMax; i++)
		{
			if (enemies[i].isDead)continue;
			enemies[i].vx = -1.0f;
			enemies[i].vy = 0.0f;
			enemies[i].x += enemies[i].vx;
			enemies[i].y += enemies[i].vy;

			if (enemies[i].counter == 0)
			{
				enemies[i].baseY = enemies[i].y;
			}

			float speed = 3.0f;

			switch (enemies[i].type)
			{
			case 0:
				if (enemies[i].coolTime > 0)
					enemies[i].coolTime--;
				if (enemies[i].coolTime <= 0)
				{
					//縦バレット
					ShotEnemyBullet(enemies[i].x + enemies[i].collisionRadius / 2,
						enemies[i].y + enemies[i].collisionRadius / 2, 65.0f, 90.0f);
					ShotEnemyBullet(enemies[i].x + enemies[i].collisionRadius / 2,
						enemies[i].y + enemies[i].collisionRadius / 2, 65.0f, 270.0f);
					enemies[i].coolTime = 10;
				}
				break;

			case 1:
				enemies[i].counter++;
				if (enemies[i].counter % 6000 == 0)
				{
					for (int angle = 130; angle < 240; angle += 8)
					{
						ShotEnemyBullet(enemies[i].x, enemies[i].y, 9.0f, angle);
					}
				}
				break;

			case 2: // 自機に向かいながらランダム範囲弾を撃つ
				enemies[i].counter++;
				if (enemies[i].counter % 2 == 0)
				{
					float angleToPlayer =
						PointToPointAngle(enemies[i].x, enemies[i].y, player.x, player.y);
					float speed = 6.0f;
					enemies[i].x += (float)cos(angleToPlayer) > 0 ? -speed / 2 : (float)cos(angleToPlayer) * speed;
					enemies[i].y += (float)sin(angleToPlayer) * speed;
					ShotEnemyBullet(enemies[i].x, enemies[i].y, 15.0f, 180.0f + RandomPlusMinus(15.0f));
				}
				break;

			case 3: // 上下移動しながら自機狙い弾を撃つ
				enemies[i].vy = (int)(sin(theta) * radius / 9);
				enemies[i].y += enemies[i].vy;
				enemies[i].counter++;
				if (enemies[i].counter % 10 == 0)
				{
					float angle =
						PointToPointAngle(enemies[i].x, enemies[i].y, player.x, player.y) *
						Rad2Deg;
					float shakeAngle = RandomPlusMinus(15.0f);
					ShotEnemyBullet(enemies[i].x, enemies[i].y, 8.0f, angle + shakeAngle);
				}
				break;

				//追尾5Way敵弾
			case 4:
				enemies[i].counter++;
				if (enemies[i].counter % 60 == 0)
				{
					float angle = PointToPointAngle(enemies[i].x, enemies[i].y, player.x, player.y) * Rad2Deg;
					float shakeAngle = RandomPlusMinus(15.0f);
					ShotEnemyBullet(enemies[i].x, enemies[i].y, 9.0f, angle);
					ShotEnemyBullet(enemies[i].x, enemies[i].y, 9.0f, angle + 4.0f);
					ShotEnemyBullet(enemies[i].x, enemies[i].y, 9.0f, angle + 8.0f);
					ShotEnemyBullet(enemies[i].x, enemies[i].y, 9.0f, angle + 356.0f);
					ShotEnemyBullet(enemies[i].x, enemies[i].y, 9.0f, angle + 352.0f);
				}

				//垂直上下
				enemies[i].y += -sin(theta) * 4.0f;
				break;

				//円弾幕
			case 5:
				enemies[i].counter++;
				if (enemies[i].counter % 60 == 0)
				{
					for (int angle = 0; angle < 360; angle += 10)
					{
						ShotEnemyBullet(enemies[i].x, enemies[i].y, 9.0f, angle);
					}
				}

				//うずまき
				float spiralRadius = (float)(3.0f * sin(theta / 10) * sin(theta / 10));
				enemies[i].x += cos(theta) * spiralRadius;
				enemies[i].y += sin(theta) * spiralRadius;
				break;
			}

			if (enemies[i].x + enemies[i].collisionRadius * 2 < 0 ||
				enemies[i].y + enemies[i].collisionRadius * 2 < 0 - ToleranceHigh ||
				enemies[i].y > ScreenHeight + ToleranceHigh)
			{
				enemies[i].isDead = 1;
			}
		}
	}

	void UpdateEnemyBullets()
	{
		for (int i = 0; i < EnemyBulletsMax; i++)
		{
			//死んだら何もしない
			if (enemyBullets[i].isDead)continue;
			//速度の分だけ移動
			enemyBullets[i].x += enemyBullets[i].vx;
			enemyBullets[i].y += enemyBullets[i].vy;
			//画面外にでたら死亡フラグを立てる
			if (enemyBullets[i].x > ScreenWidth || enemyBullets[i].x < 0 || enemyBullets[i].y<0 || enemyBullets[i].y>ScreenHeight)
			{
				enemyBullets[i].isDead = 1;
			}
		}
	}

	//爆発エフェクト更新
	void UpdateExplosion()
	{
		for (int i = 0; i < ExplosionMax; i++)
		{
			if (explosions[i].isDead)continue;

			explosions[i].counter++;
			explosions[i].imageIndex = explosions[i].counter / 3;

			if (explosions[i].imageIndex >= 16)
			{
				explosions[i].isDead = 1;
			}
		}
	}

	//ボス更新
	void UpdateBoss()
	{
		float rushMotionTheta = 0.0f;
		if (boss.enemy.isDead)return;

		//ボスの状態によって処理変える
		switch (boss.enemy.state)
		{
			//出現時
		case Appear:
			boss.enemy.x -= 1;   //左へ移動

			if (boss.enemy.x <= 750) //x座標が750以下になったら
			{
				boss.enemy.state = Normal;    //通常状態へ移行
				boss.restartX = boss.enemy.x;
				boss.restartY = boss.enemy.y;
				boss.enemy.vx = 0.0f;
				boss.enemy.vy = 2.0f;
			}
			break;

			//通常時
		case Normal:
			boss.normalTime++;
			boss.enemy.x += boss.enemy.vx;
			boss.enemy.y += boss.enemy.vy;

			for (int i = 0; i < BossLife; i++)
			{
				if (boss.enemy.isDead)continue;

				boss.enemy.counter++;
				if (boss.enemy.counter % 6000 == 0)
				{
					for (int angle = 130; angle < 240; angle += 8)
					{
						ShotEnemyBullet(boss.enemy.x, boss.enemy.y, 9.0f, angle);
					}
				}
			}

			if (boss.enemy.y >= 500)
			{
				boss.enemy.y = 500;
				boss.enemy.vy = -boss.enemy.vy;
			}
			else if (boss.enemy.y <= 50)
			{
				boss.enemy.y = 50;
				boss.enemy.vy = -boss.enemy.vy;
			}
			break;

			//気絶時
		case Swoon:
			boss.swoonTime--;  //タイマー減少
			//気絶タイマーがリセット前なら
			if (boss.swoonTime > 40)
			{
				boss.enemy.x += boss.enemy.vx;
				boss.enemy.y += boss.enemy.vy;
				boss.enemy.vx *= 0.995f;
				boss.enemy.vy *= 0.995f;
			}
			//リスタート中だったら
			else
			{
				const float Agility = 0.07f;
				boss.enemy.x = boss.enemy.x + (boss.restartX - boss.enemy.x) * Agility;
				boss.enemy.y = boss.enemy.y + (boss.restartY - boss.enemy.y) * Agility;
			}
			if (boss.swoonTime <= 0)  //タイマーが0になったら
			{
				boss.enemy.state = Angry;  //発狂モードへ
				boss.enemy.x = boss.restartX;
				boss.enemy.y = boss.restartY;
				boss.centerX = boss.enemy.x;
				boss.centerY = boss.enemy.y;
				boss.angryTime = 0;
			}
			break;

			//怒り時
		case Angry:
			boss.angryTime++;

			//突進運動
			rushMotionTheta = 1.0f * boss.angryTime * Deg2Rad;
			boss.enemy.x = boss.centerX + 300.0f * ((float)cos(2.0f * rushMotionTheta) - 1.0f);
			boss.enemy.y = boss.centerY + 150.0f * ((float)sin(rushMotionTheta));

			for (int i = 0; i < BossLife; i++)
			{
				if (boss.enemy.isDead)continue;
				boss.enemy.x += boss.enemy.vx;
				boss.enemy.y += boss.enemy.vy;

				boss.enemy.counter++;
				if (boss.enemy.counter % 400 == 0)
				{
					int angle = RandomRange(180 - 30, 180 + 30);
					ShotEnemyBullet(boss.enemy.x, boss.enemy.y, 9.0f, angle);
				}
			}
			break;

			//死亡時
		case Dying:
			boss.enemy.y++;
			boss.explosionWaitCnt++;
			boss.enemy.angle += 0.1f * Deg2Rad;
			float vibrationTheta = 2.8f * boss.dyingTime + Deg2Rad;
			boss.enemy.x = boss.centerX + 3.0f * (float)cos(17.0f * vibrationTheta);
			boss.enemy.y = boss.centerY + 3.0f * (float)sin(23.0f * vibrationTheta);
			boss.centerY += 0.5f;

			boss.dyingTime--;  //タイマー減少
			if (boss.dyingTime <= 0)  //タイマーが0になったら
			{
				SetExplosion(boss.enemy.x, boss.enemy.y);
				boss.enemy.isDead = 1; //完全に消滅
			}
			break;
		}
	}

	//描画
	void draw()override
	{
		if (state == StateTitle)
		{
			DrawSprite2D(eTitle, 0, 0);
		}
		else if (state == StatePlaying)
		{
			DrawSprite2D(eBack, 0, 0);
			DrawPlayer();
			DrawPlayerBullets();
			DrawEnemies();
			DrawEnemyBullets();
			DrawExplosion();
			DrawBoss();
		}
		else if (state == StateGameClear)
		{
			DrawSprite2D(eBack, 0, 0);
			DrawPlayer();
			DrawPlayerBullets();
			DrawEnemies();
			DrawEnemyBullets();
			DrawExplosion();
			DrawBoss();
			DrawSprite2D(eClear, 0, 0);
		}
		else if (state == StateGameover)
		{
			DrawSprite2D(eBack, 0, 0);
			DrawPlayer();
			DrawPlayerBullets();
			DrawEnemies();
			DrawEnemyBullets();
			DrawExplosion();
			DrawBoss();
			DrawSprite2D(eGameover, 0, 0);
		}
	}

	//自機描画
	void DrawPlayer()
	{
		if (player.isDead)return;

		//無敵中は2フレームに1回描画
		if (player.InvincibleTimer <= 0 || player.InvincibleTimer % 7 >= 3)
		{
			GSvector2 playerPos = { player.x,player.y };
			GSvector2 playerCenter = { PlayerImageSizeX / 2,PlayerImageSizeY / 2 };
			gsDrawSprite2D(ePLAYER, &playerPos, NULL, &playerCenter, NULL, NULL, NULL);
		}
	}

	//自機弾描画
	void DrawPlayerBullets()
	{
		for (int i = 0; i < PlayerBulletsMax; i++)
		{
			if (playerBullets[i].isDead) continue;
			GSvector2 bulletCenter = { playerBullets[i].collisionRadius,playerBullets[i].collisionRadius };
			GSvector2 bulletsPos = { playerBullets[i].x,playerBullets[i].y };
			gsDrawSprite2D(ePLAYERBULLET, &bulletsPos, NULL, &bulletCenter, NULL, NULL, playerBullets[i].angle);
		}
	}

	//敵描画
	void DrawEnemies()
	{
		for (int i = 0; i < EnemiesMax; i++)
		{
			if (enemies[i].isDead)continue;
			GSvector2 enemyPos = { enemies[i].x,enemies[i].y };
			GSvector2 enemyCenter = { enemies[i].collisionRadius,enemies[i].collisionRadius };
			gsDrawSprite2D(eENEMY00 + i, &enemyPos, NULL, &enemyCenter, NULL, NULL, NULL);
		}
	}

	void DrawEnemyBullets()
	{
		for (int i = 0; i < EnemyBulletsMax; i++)
		{
			if (enemyBullets[i].isDead) continue;
			GSvector2 Center = { enemyBullets[i].collisionRadius,enemyBullets[i].collisionRadius };
			GSvector2 Pos = { enemyBullets[i].x,enemyBullets[i].y };
			gsDrawSprite2D(eENEMYBULLET, &Pos, NULL, &Center, NULL, NULL, NULL);
		}
	}

	//爆発エフェクト描画
	void DrawExplosion()
	{
		for (int i = 0; i < ExplosionMax; i++)
		{
			if (explosions[i].isDead)continue;

			float srcX, srcY;
			srcX = (explosions[i].imageIndex % 8) * ExplosionSize;
			srcY = (explosions[i].imageIndex / 8) * ExplosionSize;
			GSvector2 pos = { explosions[i].x,explosions[i].y };
			GSvector2 center = { ExplosionSize / 2,ExplosionSize / 2 };
			GSrect rect = { srcX,srcY,srcX + ExplosionSize,srcY + ExplosionSize };
			gsDrawSprite2D(eEXPLOSION, &pos, &rect, &center, NULL, NULL, NULL);
		}
	}

	//ボス描画
	void DrawBoss()
	{
		if (boss.enemy.isDead || boss.enemy.state == Standby)return;

		int image = eBOSS_NORMAL;
		switch (boss.enemy.state)
		{
			//出現時とノーマル時
		case Appear:
		case Normal:
			image = eBOSS_NORMAL;
			break;
			//気絶時
		case Swoon:
			image = eBOSS_SWOON;
			break;
			//怒り時
		case Angry:
			image = eBOSS_ANGRY;
			break;
			//死亡時
		case Dying:
			image = eBOSS_SWOON;
			break;
		}

		GSvector2 pos = { boss.enemy.x,boss.enemy.y };
		GSvector2 center = { boss.enemy.collisionRadius,boss.enemy.collisionRadius };
		gsDrawSprite2D(image, &pos, NULL, &center, NULL, NULL, boss.enemy.angle * (180.0f / PI));
	}

	//自機がダメージを受けた時の処理
	void PlayerTakeDamege()
	{
		//ライフ減少
		player.life--;
		//ライフがなくなったら死亡
		if (player.life <= 0)
		{
			//ライフがなくなったので死亡扱いにする
			player.isDead = 1;
			SetExplosion(player.x, player.y);
		}
		else
		{
			player.InvincibleTimer = InvincibleTime;
		}
	}

	//当たり判定の処理
	void CollisionDetection()
	{
		//自機弾と敵の衝突判定
		for (int i = 0; i < PlayerBulletsMax; ++i)
		{
			if (playerBullets[i].isDead)continue; //使用中でない弾は判定しない
			for (int k = 0; k < EnemiesMax; ++k)
			{
				if (enemies[k].isDead)continue;   //死亡した敵は判定しない

				//自機弾と敵が重なっているか
				if (CircleCircleIntersection(
					playerBullets[i].x,
					playerBullets[i].y,
					playerBullets[i].collisionRadius,
					enemies[k].x,
					enemies[k].y,
					enemies[k].collisionRadius))
				{
					playerBullets[i].isDead = 1;

					//敵のライフを減らす
					enemies[k].life--;
					//ライフが0以下なら死亡
					if (enemies[k].life <= 0)
					{
						enemies[k].isDead = 1;
						SetExplosion(enemies[k].x, enemies[k].y);
					}
				}
			}

			if (playerBullets[i].isDead || boss.enemy.isDead)continue;
			//自機弾とボスが重なっているか
			if (CircleCircleIntersection(
				playerBullets[i].x + playerBullets[i].collisionRadius / 2,
				playerBullets[i].y + playerBullets[i].collisionRadius / 2,
				playerBullets[i].collisionRadius,
				boss.enemy.x + boss.enemy.collisionRadius / 2,
				boss.enemy.y + boss.enemy.collisionRadius / 2,
				boss.enemy.collisionRadius))
			{
				//登場時、気絶時、死亡時は被弾しても何もしない
				if (boss.enemy.state == Appear || boss.enemy.state == Swoon || boss.enemy.state == Dying)return;

				boss.enemy.life--;
				if (boss.enemy.life <= 0)
				{
					//ライフがなくなったら、すぐ消滅するのではなく、死亡状態へ移行
					boss.enemy.state = Dying;
					boss.dyingTime = DyingTime;
					boss.centerX = boss.enemy.x;
					boss.centerY = boss.enemy.y;
				}
				else if (boss.enemy.state == Normal && boss.enemy.life <= 50)
				{
					//通常状態でライフが50以下になったら、気絶する
					boss.enemy.state = Swoon;
					boss.swoonTime = SwoonTime;
					//気絶時の初速度を決める
					boss.enemy.vx = 1.2f;
					boss.enemy.vy = 1.2f;
				}
				playerBullets[i].isDead = 1;
			}
		}

		//自機が生きている時、衝突判定を実施
		if (player.isDead == 0)
		{
			//自機と敵の衝突判定
			for (int i = 0; i < EnemiesMax; ++i)
			{
				//敵が死んでいたらスキップする
				if (enemies[i].isDead)continue;

				//自機と敵が重なっているか
				if (CircleCircleIntersection(
					player.x,
					player.y,
					player.collisionRadius,
					enemies[i].x,
					enemies[i].y,
					enemies[i].collisionRadius))
				{
					//プレイヤーが無敵じゃなければ
					if (player.InvincibleTimer <= 0)
					{
						PlayerTakeDamege();
						enemies[i].isDead = 1;
						SetExplosion(enemies[i].x, enemies[i].y);
					}
				}
			}

			for (int i = 0; i < EnemyBulletsMax; ++i)
			{
				//敵弾が死んでいたらスキップする
				if (enemyBullets[i].isDead)continue;

				//自機と敵が重なっているか
				if (CircleCircleIntersection(
					player.x,
					player.y,
					player.collisionRadius,
					enemyBullets[i].x,
					enemyBullets[i].y,
					enemyBullets[i].collisionRadius))
				{
					//プレイヤーが無敵じゃなければ
					if (player.InvincibleTimer <= 0)
					{
						PlayerTakeDamege();
						enemyBullets[i].isDead = 1;
					}
				}
			}
		}
	}


	//自機の弾生成
	void ShotBullet(float x, float y, float speed, float angle)
	{
		PlayerBullet pb;
		pb.x = x;
		pb.y = y;
		pb.speed = speed;
		pb.vx = cosf(angle * Deg2Rad) * speed;
		pb.vy = sinf(angle * Deg2Rad) * speed;
		pb.isDead = 0;
		pb.angle = angle;
		pb.collisionRadius = PlayerBulletCollisionRadius;

		for (int i = 0; i < PlayerBulletsMax; i++)
		{
			if (playerBullets[i].isDead)
			{
				playerBullets[i] = pb;
				break;
			}
		}
		gsPlaySE(Leaser);
	}

	void ShotEnemyBullet(float x, float y, float speed, float angle)
	{
		EnemyBullet eb;
		eb.x = x;
		eb.y = y;
		eb.speed = speed;
		eb.vx = cosf(angle * Deg2Rad) * speed;
		eb.vy = sinf(angle * Deg2Rad) * speed;
		eb.imageHandle = eENEMYBULLET;
		eb.isDead = 0;
		eb.collisionRadius = EnemyBulletCollisionRadius;

		for (int i = 0; i < EnemyBulletsMax; i++)
		{
			if (enemyBullets[i].isDead)
			{
				enemyBullets[i] = eb;
				break;
			}
		}
	}

	//爆発エフェクトを発生させる
	void SetExplosion(float x, float y)
	{
		Explosion explosion;
		explosion.x = x;
		explosion.y = y;
		explosion.isDead = 0;
		explosion.counter = 0;
		explosion.imageIndex = 0;

		for (int i = 0; i < ExplosionMax; i++)
		{
			if (explosions[i].isDead)
			{
				explosions[i] = explosion;
				break;
			}
		}
		gsPlaySE(Damage);
	}

	//敵配置データcsv読み込み
	void LoadEnemyPos(const char* filePath)
	{
		FILE* fp;
		char line[Width * 4];

		fp = fopen(filePath, "r");
		int y = 0;
		if (fp == NULL)
		{
			printf("%s file not open!\n", filePath);
			return;
		}

		char* strDelimimt = (char*)",";
		while (fgets(line, sizeof(line), fp) != NULL)
		{
			printf("%s", line);
			char* p1;
			int x = 0;
			p1 = strtok(line, strDelimimt);

			while (p1 != NULL)
			{
				enemyData[y][x] = atoi(p1);
				p1 = strtok(NULL, strDelimimt);
				x++;
			}
			y++;
		}
		fclose(fp);
	}

	void Scroll(float delta)
	{
		int prevRightCell = (int)(position + ScreenWidth) / CellSize;
		position += delta;

		int currentRightCell = (int)(position + ScreenWidth) / CellSize;

		if (currentRightCell >= Width)
			return;

		if (prevRightCell == currentRightCell)
			return;

		float x = currentRightCell * CellSize - position;

		for (int cellY = 0; cellY < Height; cellY++)
		{
			float y = cellY * CellSize;
			int id = enemyData[cellY][currentRightCell];

			switch (id)
			{
			case -1:
				break;

				//雑魚敵
			case eSTRAIGHT:
			case eCOMING:
			case eUPDOWN:
			case eUPDOWN_REVERSE:
			case eSTRAIGHT_SHOOT:
			case eCOMING_SHOOT:
			case eHARD_ENEMY:
			case eRAPIDFIRE_ENEMY:
			case e3WAYSHOOT:
				AddEnemy(x, y, id);
				break;

			case eBOSS:
				InitBoss(x + 90, y + 32);

				break;
			}
		}
	}

	void AddEnemy(float x, float y, int type)
	{
		int enemyno = 0;
		for (int i = 0; i < EnemiesMax; i++)
		{
			if (enemies[i].isDead)
			{
				Enemy enemy;
				enemy.x = x;
				enemy.y = y;
				enemy.type = type;
				enemy.life = 1;
				enemy.imageHandle = eENEMY00;
				enemy.vx = -1;
				enemy.vy = 0;
				switch (enemy.type)
				{
				case eSTRAIGHT:
					enemy.imageHandle = eENEMY00;
					break;

				case eCOMING:
					enemy.imageHandle = eENEMY01;
					break;

				case eUPDOWN:
					enemy.imageHandle = eENEMY02;
					break;

				case eUPDOWN_REVERSE:
					enemy.imageHandle = eENEMY03;
					break;

				case eSTRAIGHT_SHOOT:
					enemy.imageHandle = eENEMY04;
					break;

				case eCOMING_SHOOT:
					enemy.imageHandle = eENEMY05;
					break;

				case eHARD_ENEMY:
					enemy.imageHandle = eENEMY06;
					break;

				case eRAPIDFIRE_ENEMY:
					enemy.imageHandle = eENEMY07;
					break;

				case e3WAYSHOOT:
					enemy.imageHandle = eENEMY08;
					break;

				case eBOSS:
					enemy.life = 100;
					break;

				default:
					enemy.imageHandle = eENEMY00;
					break;
				}

				enemy.collisionRadius = EnemyCollisionRadius;
				enemy.isDead = false;
				enemies[i] = enemy;
				break;
			}
		}
	}

	void InitBoss(float x, float y)
	{
		boss.enemy.state = Appear;
		boss.enemy.life = BossLife;
		boss.enemy.collisionRadius = BossCollisionRadius;
		boss.enemy.x = x;
		boss.enemy.y = y;
		boss.enemy.isDead = false;
	}

	//終了
	void end()override
	{

	}
};

//メイン関数
int main()
{
	return MyGame().run();
}