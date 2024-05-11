#include "Ball.h"

Ball::Ball() : velocity({ 0, -ball::SPEED }), ball({ 400, 400, 8 }) {}

void Ball::Draw() const {
	// ボール描画
	ball.draw();
}

void Ball::Update() {
	// ボール移動
	ball.moveBy(velocity * Scene::DeltaTime());
}
