#include <Siv3D.hpp>

class Ball;
class Score;
class GameManager;

namespace constants {

	/// @brief ボール用の定数
	namespace ball {
		/// @brief ボールの速さ
		constexpr double SPEED = 480.0;
	}

	/// @brief ブロック用の定数
	namespace brick {
		/// @brief ブロックのサイズ
		constexpr Size SIZE{ 40, 20 };

		/// @brief ブロックの数　縦
		constexpr int Y_COUNT = 10;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 20;

		/// @brief 合計ブロック数
		constexpr int MAX = Y_COUNT * X_COUNT;

		constexpr int HindBrickCount = 3;

		constexpr int NotBreakCount = 2;

		enum class BreakType
		{
			// 壊れるブロック
			Break,
			// 壊れないブロック
			NotBreak,
			// お邪魔ブロック
			HindRance,
			// 動くブロック
			MoveBrick,
			// 属性をもたないブロック
			NullBrick
		};
	}

	/// @brief パドル用の定数
	namespace paddle {
		int Y_POS = 500;
		int X_SIZE = 100;
		int Y_SIZE = 10;
	}

	/// @brief ライフ用の定数
	namespace life {
		int LifeCount = 5;
	}

	namespace gameManager {
		enum class Manager {
			GameStart,
			inGame,
			GameOver,
			GameClear,
		};
	}
}

/// @brief ブロック
class Bricks final {
public:
	/// @brief ブロックのクラス
	class BrickClass {
	public:
		/// @brief ブロックのオブジェクト
		Rect brickObj = Rect{ 0,0 };
		/// @brief ブロックのタイプ
		constants::brick::BreakType _breakType;
		BrickClass() : brickObj(0, 0), _breakType(constants::brick::BreakType::NullBrick) {}
	};

	// 全てのブロック
	BrickClass _brickstruct[constants::brick::MAX];
	// 動くブロック
	BrickClass* moveBrick[constants::brick::Y_COUNT * constants::brick::HindBrickCount];
	// 動くブロックの速さ
	float _speed;
	// 壊れないブロックのX_countで生成するindexの配列
	int notBreakBrick[constants::brick::NotBreakCount];
	// お邪魔ブロックのX_countで生成するindexの配列
	int hindBrick[constants::brick::HindBrickCount];
	// クリア判定
	bool _clear;

	Bricks() : notBreakBrick{ 3,constants::brick::X_COUNT - 3 },
		hindBrick{ constants::brick::X_COUNT / 2 ,5 ,constants::brick::X_COUNT - 5},
		_speed(400),_clear(false) {
		using namespace constants::brick;
		int moveBrickCount = 0;
		//ブロックをたくさん作る
		for (int y = 0; y < constants::brick::Y_COUNT; ++y) {
			for (int x = 0; x < X_COUNT; ++x) {
				int index = y * X_COUNT + x;
				BrickClass* brick = &_brickstruct[index];
				//ブロックのオブジェクトの作成
				brick->brickObj = Rect{
					x * SIZE.x,
					60 + y * SIZE.y,
					SIZE,
				};

				//ブロックのタイプの設定
				bool isState = false;
				//NotBreak
				for (int i : notBreakBrick){
					if (x == i) {
						brick->_breakType = BreakType::NotBreak;
						isState = true;
						break;
					}
				}
				if (!isState){
					//HindRance
					for (int i : hindBrick){
						if (x == i){
							brick->_breakType = BreakType::HindRance;
							moveBrick[moveBrickCount] = &_brickstruct[index];
							moveBrickCount++;
							isState = true;
							break;
						}
					}
					//Break
					if (!isState)
					{
						brick->_breakType = BreakType::Break;
					}
				}
			}
		}

		// moveBrickの配列の空きをNullBrickの属性を持ったBrickで埋める。
		if (moveBrickCount != Y_COUNT * HindBrickCount){
			for (moveBrickCount; moveBrickCount < Y_COUNT * HindBrickCount; moveBrickCount++){
				BrickClass* brickobj = new BrickClass();
				brickobj->_breakType = BreakType::NullBrick;
				moveBrick[moveBrickCount] = brickobj;
			}
		}
	}

	void Update()
	{
		using namespace constants::brick;
		for (int i = 0; i < Y_COUNT * HindBrickCount;++i) {
			// 参照のない配列の要素はnullptrでnullチェックできない(と思う)ので物理的に作ったNullでnullチェックをしている。
			if (moveBrick[i]->_breakType == BreakType::NullBrick) { break; }
			if (moveBrick[i]->_breakType == BreakType::MoveBrick)
			{
				//MoveBrickならブロックを動かす。
				moveBrick[i]->brickObj.y += _speed * Scene::DeltaTime();
				// ブロックの高さが規定値に達したらゲームに干渉しないところへ移動し、属性を変える。
				if (moveBrick[i]->brickObj.y > Scene::Height() + 10)
				{
					moveBrick[i]->brickObj.y += 600;
					moveBrick[i]->_breakType = BreakType::NotBreak;
				}
			}
		}
	}

	// ブロック描画
	void Draw() {
		
		for (int i = 0; i < constants::brick::MAX; ++i) {
			BrickClass* brick = &_brickstruct[i];
			//Break属性ならyの位置に合わせた色
			if (brick->_breakType == constants::brick::BreakType::Break) {
				brick->brickObj.stretched(-1).draw(HSV{ brick->brickObj.y - 40 });
			}
			//NotBreak属性ならグレー
			else if (brick->_breakType == constants::brick::BreakType::NotBreak) {
				brick->brickObj.stretched(-1).draw(Palette::Gray);
			}
			//HindRance属性なら赤
			else if(brick->_breakType == constants::brick::BreakType::HindRance) {
				brick->brickObj.stretched(-1).draw(Palette::Red);
			}
			//それ以外(主にMoveBrick属性)なら白
			else {
				brick->brickObj.stretched(-1).draw(Palette::White);
			}

		}
	}

	//ゲームクリア判定(クラス分けるの面倒だったのでここに書いた)
	void ChackClear(){
		using namespace constants::brick;
		bool isChackClear = true;
		for (auto const brick : _brickstruct){
			if (brick._breakType != BreakType::NotBreak && brick._breakType != BreakType::MoveBrick) {
				isChackClear = false;
				break;
			}
		}
		if (isChackClear) {
			_clear = true;
		}
	}

	void Intersects(Ball* target, Score* score);
};

/// @brief ボール
class Ball final {
public:
	// 初期位置
	Vec2 init_Pos;
	// 初期の動き
	Vec2 init_velocity;
	// ボールの速度
	Vec2 velocity;

	// ボールオブジェクト
	Circle ball;
	Ball() : init_Pos({ 400,400 }), init_velocity({ 0, -constants::ball::SPEED}),velocity(init_velocity), ball({ (init_Pos), 8 }) {}

	// ボール描画
	void Draw() const {
		ball.draw();
	}

	// ボール移動
	void Update() {
		ball.moveBy(velocity * Scene::DeltaTime());
	}
};

/// @brief パドル
class Paddle final {
public:
	//パドルのオブジェクト
	Rect paddle;

	Paddle() : paddle({ Arg::center(Cursor::Pos().x, constants::paddle::Y_POS), constants::paddle::X_SIZE, constants::paddle::Y_SIZE }) {}

	// パドル描画
	void Draw() {
		
		paddle.rounded(3).draw();
	}

	void Update() {
		using namespace constants::paddle;
		paddle = { Arg::center(Cursor::Pos().x, Y_POS), X_SIZE,Y_SIZE };
	}

	void Intersects(Ball* target) {
		// パドルとの衝突を検知
		if ((0 < target->velocity.y) && paddle.intersects(target->ball))
		{
			target->velocity = Vec2{
				(target->ball.x - paddle.center().x) * 10,
				-target->velocity.y
			}.setLength(constants::ball::SPEED);
		}
	}
};

/// @brief ライフ
class Life {
public:
	const int maxLife;
	int currentlLife;
	Vec2 lifeTextPos;
	Life() : maxLife(constants::life::LifeCount), currentlLife(maxLife), lifeTextPos({ Scene::Width() - 20, 20 }) {}

	void ChackDeath(Ball* target, GameManager* manager);

	void Draw(Font font) {
		font(U"ライフ:", currentlLife).draw(Arg::topRight = lifeTextPos);
		if (currentlLife <= 0) {
			font(U"GAME OVER...").drawAt(Scene::Width() / 2, Scene::Height() / 2);
		}
	}

};

/// @brief スコア
class Score {
public:
	float score;
	Score() : score(0) {}

	void AddScore() {
		score += 10;
	}

	void Draw(Font font) {
		font(score).draw(20, 20);
	}
};

/// @brief クリア
class Clear {
public:
	// 描画
	void Draw(Font font) {
		font(U"GAME CREAR!").drawAt(Scene::Width() / 2, Scene::Height() / 2);
	}
};

class GameManager {
public:
	Ball ball;
	Bricks bricks;
	Paddle paddle;
	Score score;
	Life life;
	Clear clear;

	Font font{ 50 };
	constants::gameManager::Manager _gameManagerState;

	GameManager() : _gameManagerState(constants::gameManager::Manager::GameStart) {}

	void StartGame(){
		using namespace constants::gameManager;
		font(U"Start With Left Click").drawAt(Scene::Width() / 2, Scene::Height() / 2);
		if (MouseL.up()) {
			_gameManagerState = Manager::inGame;
		}
	}

	void Update(){
		using namespace constants;
		//==============================
		// 更新
		//==============================
		// パドル
		paddle.Update();
		// ボール移動
		ball.Update();
		life.ChackDeath(&ball,this);
		bricks.Update();

		//==============================
		// コリジョン
		//==============================
		bricks.Intersects(&ball, &score);
		paddle.Intersects(&ball);


		//==============================
		// 描画
		//==============================
		ball.Draw();
		bricks.Draw();
		paddle.Draw();
		score.Draw(font);
		life.Draw(font);
		if (bricks._clear) {
			_gameManagerState = gameManager::Manager::GameClear;
		}
	}

	void GameOver()
	{
		score.Draw(font);
		life.Draw(font);
	}

	void GameClear()
	{
		score.Draw(font);
		life.Draw(font);
		clear.Draw(font);
	}
};

// ブロックとボールの当たり判定
void Bricks::Intersects(Ball* target,Score* score) {
	using namespace constants::brick;
	// ブロックとの衝突を検知
	for (int i = 0; i < MAX; ++i) {
		// 参照で保持
		Rect& refBrick = _brickstruct[i].brickObj;

		// 衝突を検知
		if (refBrick.intersects(target->ball))
		{
			// ブロックの上辺、または底辺と交差
			if (refBrick.bottom().intersects(target->ball) || refBrick.top().intersects(target->ball))
			{
				target->velocity.y *= -1;
			}
			else // ブロックの左辺または右辺と交差
			{
				target->velocity.x *= -1;
			}
			// あたったブロックは画面外に出す
			if (_brickstruct[i]._breakType == BreakType::Break || _brickstruct[i]._breakType == BreakType::HindRance) {
				score->AddScore();
				if (_brickstruct[i]._breakType == BreakType::Break) {
					refBrick.y -= 600;
					_brickstruct[i]._breakType = BreakType::NotBreak;
				}
				else {
					_brickstruct[i]._breakType = BreakType::MoveBrick;
				}
			}
			ChackClear();
			// 同一フレームでは複数のブロック衝突を検知しない
			break;
		}

		// 天井との衝突を検知
		if ((target->ball.y < 0) && (target->velocity.y < 0))
		{
			target->velocity.y *= -1;
		}

		// 壁との衝突を検知
		if (((target->ball.x < 0) && (target->velocity.x < 0))
			|| ((Scene::Width() < target->ball.x) && (0 < target->velocity.x)))
		{
			target->velocity.x *= -1;
		}
	}
}

// ライフ、ゲームオーバー判定
void Life::ChackDeath(Ball* target, GameManager* manager) {
	using namespace constants;
	if (target->ball.y < Scene::Height() + 50) return;

	currentlLife--;
	if (currentlLife > 0) {
		target->ball.setPos(target->init_Pos);
		target->velocity = target->init_velocity;
	}
	else {
		manager->_gameManagerState = gameManager::Manager::GameOver;
	}
}

void Main()
{
	//Ball ball;
	//Bricks bricks;
	//Paddle paddle;
	//Score score;
	//Life life;
	//Clear clear;
	GameManager _manager;

	const Font font{50};


	while (System::Update())
	{
		using namespace constants::gameManager;
		switch(_manager._gameManagerState)
		{
		case Manager::GameStart :
			_manager.StartGame();
			break;
		case Manager::inGame :
			_manager.Update();
			break;
		case Manager::GameOver:
			_manager.GameOver();
			break;
		case Manager::GameClear:
			_manager.GameClear();
			break;
		}
		////==============================
		//// 更新
		////==============================
		//// パドル
		//paddle.Update();
		//// ボール移動
		//ball.Update();
		//if (!bricks._clear) {
		//	life.ChackDeath(&ball);
		//}
		//bricks.Update();

		////==============================
		//// コリジョン
		////==============================
		//bricks.Intersects(&ball, &score);
		//paddle.Intersects(&ball);


		////==============================
		//// 描画
		////==============================
		//ball.Draw();
		//bricks.Draw();
		//paddle.Draw();
		//score.Draw(font);
		//life.Draw(font);
		//if (bricks._clear) {
		//	clear.Draw(font);
		//}
	}
}

