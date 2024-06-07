#include <Siv3D.hpp>

class Ball;
class Score;

namespace constants {

	namespace ball {
		/// @brief ボールの速さ
		constexpr double SPEED = 480.0;
	}

	/// @brief ブロック用の定数
	namespace brick {
		/// @brief ブロックのサイズ
		constexpr Size SIZE{ 40, 20 };

		/// @brief ブロックの数　縦
		constexpr int Y_COUNT = 1;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 1;

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

	namespace paddle {
		int Y_POS = 500;
		int X_SIZE = 60;
		int Y_SIZE = 10;
	}
}

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

	/// @brief 全てのブロック
	BrickClass _brickstruct[constants::brick::MAX];
	/// @brief 動くブロック
	BrickClass* moveBrick[constants::brick::Y_COUNT * constants::brick::HindBrickCount];
	/// @brief 動くブロックの速さ
	float _speed;
	/// @brief 壊れないブロックのX_countで生成するindexの配列
	int notBreakBrick[constants::brick::NotBreakCount];
	/// @brief お邪魔ブロックのX_countで生成するindexの配列
	int hindBrick[constants::brick::HindBrickCount];

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
				for (int i : notBreakBrick){
					if (x == i) {
						brick->_breakType = BreakType::NotBreak;
						isState = true;
						break;
					}
				}
				if (!isState){
					for (int i : hindBrick){
						if (x == i){
							brick->_breakType = BreakType::HindRance;
							moveBrick[moveBrickCount] = &_brickstruct[index];
							moveBrickCount++;
							isState = true;
							break;
						}
					}
					if (!isState)
					{
						brick->_breakType = BreakType::Break;
					}
				}
			}
		}
		// moveBrickの配列の空きをNullBrickで埋める。
		if (moveBrickCount != Y_COUNT * HindBrickCount){
			for (moveBrickCount; moveBrickCount < Y_COUNT * HindBrickCount; moveBrickCount++){
				BrickClass* brickobj = new BrickClass();
				moveBrick[moveBrickCount] = brickobj;
				delete brickobj;
			}
		}
	}

	void Update()
	{
		using namespace constants::brick;
		for (int i = 0; i < Y_COUNT * HindBrickCount;++i) {
			if (moveBrick[i]->_breakType == BreakType::NullBrick) { break; }
			if (moveBrick[i]->_breakType == BreakType::MoveBrick)
			{
				moveBrick[i]->brickObj.y += _speed * Scene::DeltaTime();
				if (moveBrick[i]->brickObj.y > Scene::Height() + 10)
				{
					moveBrick[i]->brickObj.y += 600;
					moveBrick[i]->_breakType = BreakType::NotBreak;
				}
			}
		}
	}

	void Draw() {
		// ブロック描画
		for (int i = 0; i < constants::brick::MAX; ++i) {
			BrickClass* brick = &_brickstruct[i];
			if (brick->_breakType == constants::brick::BreakType::Break) {
				brick->brickObj.stretched(-1).draw(HSV{ brick->brickObj.y - 40 });
			}
			else if (brick->_breakType == constants::brick::BreakType::NotBreak) {
				brick->brickObj.stretched(-1).draw(Palette::Gray);
			}
			else if(brick->_breakType == constants::brick::BreakType::HindRance) {
				brick->brickObj.stretched(-1).draw(Palette::Red);
			}
			else {
				brick->brickObj.stretched(-1).draw(Palette::White);
			}

		}
	}

	void ChackClear(){
		using namespace constants::brick;
		bool isChackClear = true;
		for (auto brick : _brickstruct){
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

class Ball final {
public:

	Vec2 init_Pos;
	Vec2 init_velocity;
	/// @brief ボールの速度
	Vec2 velocity;

	/// @brief ボール
	Circle ball;
	Ball() : init_Pos({ 400,400 }), init_velocity({ 0, -constants::ball::SPEED }),velocity(init_velocity), ball({ (init_Pos), 8 }) {}

	void Draw() const {
		// ボール描画
		ball.draw();
	}

	void Update() {
		// ボール移動
		ball.moveBy(velocity * Scene::DeltaTime());
	}
};

class Paddle final {
public:
	Rect paddle;

	Paddle() : paddle({ Arg::center(Cursor::Pos().x, constants::paddle::Y_POS), constants::paddle::X_SIZE, constants::paddle::Y_SIZE }) {}

	void Draw() {
		// パドル描画
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

class Life {
public:
	const int maxLife;
	int currentlLife;
	Vec2 lifeTextPos;
	Life() : maxLife(3), currentlLife(maxLife), lifeTextPos({ Scene::Width() - 20, 20 }) {}

	void ChackDeath(Ball* target);

	void Draw(Font font) {
		font(U"ライフ:", currentlLife).draw(Arg::topRight = lifeTextPos);
		if (currentlLife <= 0) {
			font(U"GAME OVER...").drawAt(Scene::Width() / 2, Scene::Height() / 2);
		}
	}

};

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

class Clear {
public:
	void Draw(Font font) {
		font(U"GAME CREAR!").drawAt(Scene::Width() / 2, Scene::Height() / 2);
	}
};

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

void Life::ChackDeath(Ball* target) {
	if (target->ball.y < Scene::Height() + 50 || currentlLife <= 0) { return; }

	currentlLife--;
	if (currentlLife > 0) {
		target->ball.setPos(target->init_Pos);
		target->velocity = target->init_velocity;
	}
}

void Main()
{
	Ball ball;
	Bricks bricks;
	Paddle paddle;
	Score score;
	Life life;
	Clear clear;

	const Font font{50};


	while (System::Update())
	{
		//==============================
		// 更新
		//==============================
		// パドル
		paddle.Update();
		// ボール移動
		ball.Update();
		if (!bricks._clear) { life.ChackDeath(&ball); }
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
		if (bricks._clear) { clear.Draw(font); }
	}
}

