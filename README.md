# PS22A 岩垂龍吾 ブロック崩し

## やったこと

### GameManagerの追加、GameClear、GameOverの追加
### 列挙型でBrickの種類の追加
### 種類によって挙動が違うBrickの要素の追加。
### ライフ、スコアの追加
### vectorを使いたい人生だった…(std縛りだったので配列で無理やり何とかしました。)

## 反省点
### _moveBrick関連の設計はおかしいのでこれから先注意する。(『newしてあるもの』と『してないもの』を一緒に入れない。)
### メモリ確保、開放を気にかけるようにする
