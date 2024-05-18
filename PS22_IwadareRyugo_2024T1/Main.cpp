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
		constexpr int Y_COUNT = 5;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 20;

		/// @brief 合計ブロック数
		constexpr int MAX = Y_COUNT * X_COUNT;
	}

	namespace paddle {
		int Y_POS = 500;
		int X_SIZE = 60;
		int Y_SIZE = 10;
	}
}

class Bricks final {
public:
	Rect bricks[constants::brick::MAX];
	int notBreakBrick[2];
	int hindBrick;

	Bricks() : notBreakBrick{3,constants::brick::X_COUNT - 3},hindBrick(constants::brick::X_COUNT / 2) {
		for (int y = 0; y < constants::brick::Y_COUNT; ++y) {
			for (int x = 0; x < constants::brick::X_COUNT; ++x) {
				int index = y * constants::brick::X_COUNT + x;
				bricks[index] = Rect{
					x * constants::brick::SIZE.x,
					60 + y * constants::brick::SIZE.y,
					constants::brick::SIZE
					//for (int notBreakIndex = 0; notBreakIndex < notBreakBrick; ++notBreakBrick)
					//{

					//}
				};
			}
		}
	}

	void Draw(){
		// ブロック描画
		for (int i = 0; i < constants::brick::MAX; ++i) {
			bricks[i].stretched(-1).draw(HSV{ bricks[i].y - 40 });
		}
	}

	void Intersects(Ball* target, Score* score);

	enum BreakType
	{
		NotBreak,
		Break,
		HindRance,
	};
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
			font(U"GameOver").drawAt(Scene::Width() / 2, Scene::Height() / 2);
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

void Bricks::Intersects(Ball* target,Score* score) {
	using namespace constants::brick;
	// ブロックとの衝突を検知
	for (int i = 0; i < MAX; ++i) {
		// 参照で保持
		Rect& refBrick = bricks[i];

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

			score->AddScore();

			// あたったブロックは画面外に出す
			refBrick.y -= 600;

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
		life.ChackDeath(&ball);

		//==============================
		// コリジョン
		//==============================
		bricks.Intersects(&ball,&score);
		paddle.Intersects(&ball);


		//==============================
		// 描画
		//==============================
		ball.Draw();
		bricks.Draw();
		paddle.Draw();
		score.Draw(font);
		life.Draw(font);
	}
}

