#ifndef FUNC_H
#define FUNC_H

//指定した範囲の整数の乱数を取得する(maxは出ない)
int RandomRange(int min, int max);

//指定した範囲の少数の乱数を取得する(maxは出ない)
float RandomRange(float min, float max);

//指定した範囲で乱数を返却する
//例えば1.5fを指定すると、-1.5～+1.5の範囲の値を返却する
float RandomPlusMinus(float value);

//円と円が重なっているかを調べる
int CircleCircleIntersection(float x1, float y1, float radius1, float x2, float y2, float radius2);

//点から点への角度(ラジアン)を求める。
float PointToPointAngle(float fromX, float fromY, float toX, float toY);

#endif FUNC_H
