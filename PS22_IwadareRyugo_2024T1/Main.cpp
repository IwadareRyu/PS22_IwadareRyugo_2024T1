#include <Siv3D.hpp>
#include "Ball.h"
#include "Paddle.h"

class Ball;

namespace constants {


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

	namespace Paddle {
		int Y_POS = 500;
		int X_SIZE = 60;
		int Y_SIZE = 10;
	}
}

class Bricks final {
public:
	Rect bricks[constants::brick::MAX];

	Bricks() {
		for (int y = 0; y < constants::brick::Y_COUNT; ++y) {
			for (int x = 0; x < constants::brick::X_COUNT; ++x) {
				int index = y * constants::brick::X_COUNT + x;
				bricks[index] = Rect{
					x * constants::brick::SIZE.x,
					60 + y * constants::brick::SIZE.y,
					constants::brick::SIZE
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

	void Intersects(Ball* target) {
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

				// あたったブロックは画面外に出す
				refBrick.y -= 600;

				// 同一フレームでは複数のブロック衝突を検知しない
				break;
			}
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
};

class Paddle final{
public:
	Rect paddle;

	Paddle() : paddle({ Arg::center(Cursor::Pos().x, constants::Paddle::Y_POS), constants::Paddle::X_SIZE, constants::Paddle::Y_SIZE }) {}

	void Draw(){
		// パドル描画
		paddle.rounded(3).draw();
	}

	void Update(){
		using namespace constants::Paddle;
		paddle = { Arg::center(Cursor::Pos().x, Y_POS), X_SIZE,Y_SIZE };
	}

	void Intersects(Ball* target){
		// パドルとの衝突を検知
		if ((0 < target->velocity.y) && paddle.intersects(target->ball))
		{
			target->velocity = Vec2{
				(target->ball.x - paddle.center().x) * 10,
				-target->velocity.y
			}.setLength(ball::SPEED);
		}
	}
};

void Main()
{
	Ball ball;
	Bricks bricks;
	Paddle paddle;

	while (System::Update())
	{
		//==============================
		// 更新
		//==============================
		// パドル
		paddle.Update();
		// ボール移動
		ball.Update();

		//==============================
		// コリジョン
		//==============================
		bricks.Intersects(&ball);
		paddle.Intersects(&ball);

		//==============================
		// 描画
		//==============================
		ball.Draw();
		bricks.Draw();
		paddle.Draw();
	}
}

