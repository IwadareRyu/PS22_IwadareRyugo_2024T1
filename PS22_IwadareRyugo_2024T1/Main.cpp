# include <Siv3D.hpp> // Siv3D v0.6.13

constexpr int ball_Rad = 40;

constexpr float ball_Move_Speed = 200;

constexpr Size brick_Size{40,20};

constexpr int x_Count = 20;
constexpr int y_Count = 5;
constexpr int max = x_Count * y_Count;

void Main()
{

#pragma region ボール
	Vec2 ballVec{0,ball_Move_Speed};

	Circle ball{ 400,400,8 };
#pragma endregion

#pragma region ブロック
	Rect bricks[max];
	bricks[0] = Rect(0,0);
	for (auto y = 0; y < y_Count; y++)
	{
		for (auto x = 0; x < x_Count; x++)
		{
			int index_Count = y * x_Count + x;
			bricks[index_Count] = Rect{
				x * brick_Size.x,
				brick_Size.y + y * brick_Size.y,
				brick_Size
		};

		}
	}
#pragma endregion

	while (System::Update())
	{

#pragma region 更新
		const Rect paddle{
		/* 座標 */ Arg::center(Cursor::Pos().x,500),
		/* Xサイズ */ 60,
		/* Yサイズ */ 10
		};

		ball.moveBy(ballVec * Scene::DeltaTime());
#pragma endregion

#pragma region 衝突
		/// ブロックの衝突の検知
		for (auto i = 0; i < max; i++)
		{
			auto& refbrick = bricks[i];

			if (refbrick.intersects(ball))
			{
				// ブロックの上辺、または底辺と交差。
				if (refbrick.bottom().intersects(ball) || refbrick.top().intersects(ball))
				{
					ballVec.y *= -1;
				}
				else
				{
					ballVec.x *= -1;
				}	// ブロックの右辺、または左辺と交差。

				//ブロックを画面外に出す。
				refbrick.y -= 600;

				break;
			}

		}

		/// 天井との衝突を検知
		if (ball.y < 0 && ballVec.y < 0)
		{
			ballVec.y = -1;
		}

		/// 壁との衝突を検知
		if((ball.x < 0 && ballVec.x < 0)
			|| (Scene::Width() < ball.x && 0 < ballVec.x))
		{
			ballVec.x *= -1;
		}

		if (0 < ballVec.y && paddle.intersects(ball))
		{
			ballVec = Vec2{
				(ball.x - paddle.center().x) * 10,
				-ballVec.y
			}.setLength(ball_Move_Speed);
		}
#pragma endregion

#pragma region 描画
		for (int i = 0; i < max; i++)
		{
			bricks[i].stretched(-1).draw(HSV{ bricks[i].y - 40 });
		}
		ball.draw();
		paddle.rounded(3).draw();
#pragma endregion
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
