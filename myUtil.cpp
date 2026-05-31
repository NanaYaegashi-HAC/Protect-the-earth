#include<GSgameLight.h>
#include "myUtil.h"

int RandomRange(int min, int max)
{
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

float RandomRange(float min, float max)
{
	return min + (float)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

float RandomPlusMinus(float value)
{
	return RandomRange(-value, value);
}

/// <summary>
/// ‰~‚Æ‰~‚ªd‚È‚Á‚Ä‚¢‚é‚©‚ğ’²‚×‚é
/// </summary>
/// <param name="x1">‰~1‚Ì’†S</param>
/// <param name="y1">‰~1‚Ì’†S</param>
/// <param name="radius1">‰~1‚Ì”¼Œa</param>
/// <param name="x2">‰~2‚Ì’†S</param>
/// <param name="y2">‰~2‚Ì’†S</param>
/// <param name="radius2">‰~2‚Ì”¼Œa</param>
/// <returns>d‚È‚Á‚Ä‚¢‚ê‚Î1Ad‚È‚Á‚Ä‚¢‚È‚¯‚ê‚Î0‚ğ•Ô‹p‚·‚é</returns>
int CircleCircleIntersection(float x1, float y1, float radius1, float x2, float y2, float radius2)
{
	return((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= ((radius1 + radius2) * (radius1 + radius2));
}

//“_‚©‚ç“_‚Ö‚ÌŠp“x(ƒ‰ƒWƒAƒ“)‚ğ‹‚ß‚éB
float PointToPointAngle(float fromX, float fromY, float toX, float toY)
{
	return(float)atan2(toY - fromY, toX - fromX);
}