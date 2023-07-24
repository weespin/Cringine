#pragma once
#undef min
#undef max
#include <algorithm>

#include "DirectXMath.h"

class ColorHelper
{
public:
	// Structure for storing Hue, Saturation and Value
	struct HSV
	{
		float H; // Hue in degrees
		float S; // Saturation from 0 to 1
		float V; // Value from 0 to 1
	};

	// Structure for storing Red, Green and Blue values
	struct RGBA
	{
		float R;
		float G;
		float B;
		float A = 1.f;
		operator DirectX::XMFLOAT4() const
		{
			return DirectX::XMFLOAT4(R, G, B, A);
		}
		RGBA& operator=(const DirectX::XMFLOAT4& other)
		{
			R = other.x;
			G = other.y;
			B = other.z;
			A = other.w;
			return *this;
		}
	};

	// Convert RGB to HSV
	static HSV RGB2HSV(const RGBA in)
	{
		HSV out{};

		const float min = std::min(std::min(in.R, in.G), in.B);
		const float max = std::max(std::max(in.R, in.G), in.B);
		const float delta = max - min;

		// Calculate Value (V)
		out.V = max;

		// Calculate Saturation (S)
		if (max > 0.0f)
			out.S = delta / max;
		else
			out.S = 0.0f;

		// Calculate Hue (H)
		if (delta < 0.00001f)
			out.H = 0;
		else if (in.R >= max)
			out.H = 60.0f * ((in.G - in.B) / delta);
		else if (in.G >= max)
			out.H = 60.0f * (2.0f + (in.B - in.R) / delta);
		else
			out.H = 60.0f * (4.0f + (in.R - in.G) / delta);

		if (out.H < 0.0f)
			out.H += 360.0f;

		return out;
	}

	// Convert HSV to RGB
	static RGBA HSV2RGB(const HSV in)
	{
		RGBA out;

		if (in.S <= 0.0f)
		{
			out.R = in.V;
			out.G = in.V;
			out.B = in.V;
			return out;
		}

		float hh = in.H;

		if (hh >= 360.0f) hh = 0.0f;

		hh /= 60.0f;

		const long i = static_cast<long>(hh);

		const float ff = hh - i;

		const float p = in.V * (1.0f - in.S);
		const float q = in.V * (1.0f - (in.S * ff));
		const float t = in.V * (1.0f - (in.S * (1.0f - ff)));

		switch (i)
		{
		case 0:
			out.R = in.V;
			out.G = t;
			out.B = p;
			break;
		case 1:
			out.R = q;
			out.G = in.V;
			out.B = p;
			break;
		case 2:
			out.R = p;
			out.G = in.V;
			out.B = t;
			break;
		case 3:
			out.R = p;
			out.G = q;
			out.B = in.V;
			break;
		case 4:
			out.R = t;
			out.G = p;
			out.B = in.V;
			break;
		case 5:
		default:
			out.R = in.V;
			out.G = p;
			out.B = q;
			break;
		}

		return out;
	}
};
