#pragma once

/// @brief ボール用の定数
namespace ball {
	/// @brief ボールの速さ
	constexpr double SPEED = 480.0;
}

class Ball final {
public:
	/// @brief ボールの速度
	Vec2 velocity;

	/// @brief ボール
	Circle ball;

	Ball();

	void Draw() const;

	void Update();
};
