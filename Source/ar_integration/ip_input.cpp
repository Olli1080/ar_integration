#include "ip_input.h"

bool U_ip_input::push(int digit, int trip_pos)
{
	if (!valid_trip_pos(trip_pos) ||
		digit < 0 || digit > 9)
		return false;

	auto& trip_val = triplets[trip_pos];
	if (trip_val == 0)
	{
		trip_val = digit;
		return true;
	}
	if (trip_val < 100)
	{
		const int temp_val = trip_val * 10 + digit;
		if (temp_val > 255) return false;
		trip_val = static_cast<uint8_t>(temp_val);

		if (trip_val > 25)
			on_next.Broadcast();
		return true;
	}

	return false;
}

bool U_ip_input::pop(int trip_pos)
{
	if (!valid_trip_pos(trip_pos))
		return false;

	auto& trip_val = triplets[trip_pos];
	if (trip_val == 0)
	{
		if (trip_pos != 0)
			on_prev.Broadcast();
		return false;
	}
	
	if (trip_val < 10)
		trip_val = 0;
	else
		trip_val /= 10;
	
	return true;
}

int U_ip_input::get_trip_val(int trip_pos) const
{
	if (!valid_trip_pos(trip_pos))
		return -1;

	return triplets[trip_pos];
}

void U_ip_input::clear()
{
	for (size_t i = 0; i < 5; ++i)
		triplets[i] = 0;
}

FString U_ip_input::to_string_trip(int trip_pos) const
{
	if (!valid_trip_pos(trip_pos))
		return "";

	return FString::FromInt(triplets[trip_pos]);
}

FString U_ip_input::to_string() const
{
	FString temp;

	for (size_t i = 0; i < 3; ++i)
	{
		temp += FString::FromInt(triplets[i]);
		temp += ".";
	}
	return temp + FString::FromInt(triplets[3]);
}

bool U_ip_input::valid_trip_pos(int trip_pos)
{
	return (trip_pos < 4 && trip_pos >= 0);
}