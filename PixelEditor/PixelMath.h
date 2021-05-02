#pragma once
#include <iostream>
#include <string>

#ifndef PI
#define PI 3.14159265f
#endif // !PI


namespace PixelMath
{
	struct Vec2D
	{
		Vec2D() { X = 0.0f; Y = 0.0f; }
		Vec2D(float x, float y) : X(x), Y(y){}

		float X;
		float Y;

		void ZeroOut() { X = 0.0f; Y = 0.0f; }		
		void Round() { X = round(X); Y = round(Y); }
		void Floor() { X = std::floor(X); Y = std::floor(Y); }
		float GetMag()
		{
			float length;
			length = sqrt(X * X + Y * Y);
			return length;
		}
		void Normalize()
		{
			float mag = GetMag();

			X /= mag;
			Y /= mag;
		}	
		std::string AsString()
		{
			return ("X = " + std::to_string(X) + ", Y = " + std::to_string(Y));
		}
	};

	inline float GetRotationBasedOnAngle(float angleInDeg)
	{
		return angleInDeg * 0.0175f;
	}


	///////////////////////// OVERRLOADS /////////////////////////
	static Vec2D operator - (Vec2D a, Vec2D b)
	{

		Vec2D result;
		result.X = a.X - b.X;
		result.Y = a.Y - b.Y;

		//return Vec2D{ a.X - b.X, a.Y - b.Y };
		return result;
	}

	static Vec2D operator - (Vec2D a, float b)
	{

		Vec2D result;
		result.X = a.X - b;
		result.Y = a.Y - b;

		//return Vec2D{ a.X - b.X, a.Y - b.Y };
		return result;
	}

	static Vec2D operator + (Vec2D a, Vec2D b)
	{

		Vec2D result;
		result.X = a.X + b.X;
		result.Y = a.Y + b.Y;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return result;
	}

	static Vec2D operator + (Vec2D a, float b)
	{

		Vec2D result;
		result.X = a.X + b;
		result.Y = a.Y + b;

		return result;
	}

	static Vec2D operator += (Vec2D &a, Vec2D &b)
	{

		a.X += b.X;
		a.Y += b.Y;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static Vec2D operator += (Vec2D &a, float &b)
	{

		a.X += b;
		a.Y += b;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static Vec2D operator -= (Vec2D &a, float &b)
	{

		a.X -= b;
		a.Y -= b;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static Vec2D operator -= (Vec2D &a, Vec2D &b)
	{

		a.X -= b.X;
		a.Y -= b.Y;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}
	static Vec2D operator / (Vec2D a, float b)
	{
		a.X /= b;
		a.Y /= b;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	//static Vec2D operator * (Vec2D a, float b)
	//{
	//	a.X *= b;
	//	a.Y *= b;
	//
	//	return a;
	//}
	//
	//static Vec2D operator * (float b, Vec2D a)
	//{
	//	a.X *= b;
	//	a.Y *= b;
	//
	//	return a;
	//}

	static Vec2D operator *= (Vec2D& a, Vec2D& b)
	{

		a.X *= b.X;
		a.Y *= b.Y;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static Vec2D operator *= (Vec2D& a, float b)
	{

		a.X *= b;
		a.Y *= b;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static Vec2D operator /= (Vec2D& a, Vec2D& b)
	{

		a.X /= b.X;
		a.Y /= b.Y;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static Vec2D operator /= (Vec2D& a, float b)
	{

		a.X /= b;
		a.Y /= b;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static bool operator == (Vec2D a, Vec2D b)
	{
		return ((a.X == b.X) && (a.Y == b.Y));
	}

	static bool operator != (Vec2D a, Vec2D b)
	{
		return ((a.X != b.X) || (a.Y != b.Y));
	}

	/////////////////////// END OVERRLOADS ///////////////////////


	namespace Functions
	{
		// Return length/magnitude of a vector
		static float Mag(Vec2D vec);
		// Return length/magnitude of a vector squared (faster way of comparing what object is closer to his target)
		static float MagSqr(Vec2D vec);
		static void SetMag(Vec2D &vec, float newMag);
		static Vec2D Mult(Vec2D v, float multiplier);
		static Vec2D Normalize(Vec2D v);
		static float Dot(Vec2D v1, Vec2D v2);
		static float Rad2Deg(float rad);
		static float Deg2Rad(float deg);

		static void Clamp(float &variable, float max, float min);
	};	

	static Vec2D operator * (Vec2D a, float b)
	{
		a.X *= b;
		a.Y *= b;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}

	static Vec2D operator * (Vec2D a, Vec2D b)
	{
		a.X *= b.X;
		a.Y *= b.Y;

		//return Vec2D{ a.X + b.X, a.Y + b.Y };
		return a;
	}
};

float PixelMath::Functions::Mag(Vec2D vec)
{
	float length;
	length = sqrt(vec.X * vec.X + vec.Y * vec.Y);
	return length;
}

float PixelMath::Functions::MagSqr(Vec2D vec)
{
	float length;
	length = vec.X * vec.X + vec.Y * vec.Y;
	return length;
}

void PixelMath::Functions::SetMag(Vec2D &vec, float newMag)
{
	float mag = Mag(vec);

	vec.X = vec.X * newMag / mag;
	vec.Y = vec.Y * newMag / mag;
}

static PixelMath::Vec2D PixelMath::Functions::Mult(PixelMath::Vec2D v, float multiplier)
{
	PixelMath::Vec2D newVec;
	newVec.X = v.X * multiplier;
	newVec.Y = v.Y * multiplier;
	return newVec;
}

static PixelMath::Vec2D PixelMath::Functions::Normalize(PixelMath::Vec2D v)
{
	float mag = Mag(v);
	Vec2D newVector;

	newVector.X = v.X / mag;
	newVector.Y = v.Y / mag;

	return newVector;
}

static float PixelMath::Functions::Dot(Vec2D v1, Vec2D v2)
{
	return (v1.X * v2.X + v1.Y * v2.Y);
}

static void PixelMath::Functions::Clamp(float &variable, float max, float min)
{
	if (variable > max) variable = max;
	if (variable < min) variable = min;
}

//Converting from radians to degrees
static float  PixelMath::Functions::Rad2Deg(float rad)
{
	return rad * 180.0f / PI;
}

// Converting from degrees to radians
static float  PixelMath::Functions::Deg2Rad(float deg)
{
	return deg * PI / 180.0f;
}