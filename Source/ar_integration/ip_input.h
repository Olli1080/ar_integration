#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "ip_input.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(F_next_delegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(F_prev_delegate);

/*
 * @class U_ip_input consuming [partial]
 * ip4 tuples from left to right
 */
UCLASS(Blueprintable)
class U_ip_input : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * push digit from left to right onto triplet
	 *
	 * @returns false if invalid digit, trip_pos or
	 *			if new value would exceed 255
	 */
	UFUNCTION(BlueprintCallable)
	bool push(int digit, int trip_pos);

	/**
	 * pop digit from right to left from triplet
	 *
	 * @returns false if trip_pos invalid or if value == 0
	 */
	UFUNCTION(BlueprintCallable)
	bool pop(int trip_pos);

	/**
	 * get value of specific triplet
	 *
	 * @returns -1 if trip_pos invalid
	 */
	UFUNCTION(BlueprintCallable)
	int get_trip_val(int trip_pos) const;

	/**
	 * reset all values
	 */
	UFUNCTION(BlueprintCallable)
	void clear();

	/**
	 * toString method for specific triplet
	 *
	 * @returns either value of triplet of string or
	 *			empty string if trip_pos invalid
	 */
	UFUNCTION(BlueprintCallable)
	FString to_string_trip(int trip_pos) const;

	/**
	 * @returns dot separated ip address
	 */
	UFUNCTION(BlueprintCallable)
	FString to_string() const;

	/**
	 * emits if a triplet is full or the new value would exceed 255
	 */
	UPROPERTY(BlueprintAssignable)
	F_next_delegate on_next;

	/**
	 * emits if a triplet is empty
	 *
	 * does not emit if triplet index == 0
	 */
	UPROPERTY(BlueprintAssignable)
	F_prev_delegate on_prev;

private:

	uint8_t triplets[4]{};

	/**
	 * @returns true if trip_pos is valid
	 */
	static bool valid_trip_pos(int trip_pos);
};