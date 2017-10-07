#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

#include "../localisation/date.h"
#include "../cheats.h"
#include "../interface/window.h"
#include "../world/park.h"
#include "../rct1.h"
#include "../ride/ride_data.h"
#include "../ride/ride.h"
#include "../ride/station.h"

class RideCreateGameActionResult final : public GameActionResult
{
public:
    RideCreateGameActionResult() : GameActionResult(GA_ERROR::OK, 0) {}
    RideCreateGameActionResult(GA_ERROR error, rct_string_id message) : GameActionResult(error, message) {}

    sint32 rideIndex = -1;
};

struct RideCreateAction : public GameActionBase<GAME_COMMAND_CREATE_RIDE, RideCreateGameActionResult>
{
private:
    sint32 _rideType;
    sint32 _subType;
    uint8 _colour1;
    uint8 _colour2;

public:
    RideCreateAction() {}
    RideCreateAction(sint32 rideType, sint32 subType, sint32 colour1, sint32 colour2) :
        _rideType(rideType),
        _subType(subType),
        _colour1(colour1),
        _colour2(colour2)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _rideType << _subType << _colour1 << _colour2;
    }

    GameActionResult::Ptr Query() const override
    {
        sint32 rideIndex = ride_get_empty_slot();
        if (rideIndex == -1)
        {
            // No more free slots available.
            return std::make_unique<RideCreateGameActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_NONE);
        }

        if (_rideType >= RIDE_TYPE_COUNT)
        {
            return std::make_unique<RideCreateGameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }

        sint32 rideEntryIndex = ride_get_entry_index(_rideType, _subType);
        if (rideEntryIndex >= 128)
        {
            return std::make_unique<RideCreateGameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }


        const track_colour_preset_list *colourPresets = &RideColourPresets[_rideType];
        if (_colour1 >= colourPresets->count)
        {
            // FIXME: Add new error string.
            return std::make_unique<RideCreateGameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }

        rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);
        vehicle_colour_preset_list *presetList = rideEntry->vehicle_preset_list;
        if ((presetList->count > 0 && presetList->count != 255) &&
            _colour2 >= presetList->count)
        {
            // FIXME: Add new error string.
            return std::make_unique<RideCreateGameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_TYPE);
        }

        return std::make_unique<RideCreateGameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        rct_ride_entry * rideEntry;
        auto res = std::make_unique<RideCreateGameActionResult>();

        sint32 rideEntryIndex = ride_get_entry_index(_rideType, _subType);
        sint32 rideIndex = ride_get_empty_slot();

        res->rideIndex = rideIndex;

        auto ride = get_ride(rideIndex);
        rideEntry = get_ride_entry(rideEntryIndex);
        if (rideEntry == nullptr || rideEntry == (rct_ride_entry *)-1)
        {
            log_warning("Invalid request for ride %u", rideIndex);
            res->Error = GA_ERROR::UNKNOWN;
            res->ErrorMessage = STR_UNKNOWN_OBJECT_TYPE;
            return std::move(res);
        }

        ride->type = _rideType;
        ride->subtype = rideEntryIndex;
        ride_set_colour_preset(ride, _colour1);
        ride->overall_view.xy = RCT_XY8_UNDEFINED;

        // Ride name
        if (rideEntryIndex == RIDE_ENTRY_INDEX_NULL)
        {
            ride_set_name_to_track_default(ride, rideEntry);
        }
        else
        {
            ride_set_name_to_default(ride, rideEntry);
        }

        for (size_t i = 0; i < MAX_STATIONS; i++)
        {
            ride->station_starts[i].xy = RCT_XY8_UNDEFINED;
            ride->entrances[i].xy = RCT_XY8_UNDEFINED;
            ride->exits[i].xy = RCT_XY8_UNDEFINED;
            ride->train_at_station[i] = 255;
            ride->queue_time[i] = 0;
        }

        for (size_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++)
        {
            ride->vehicles[i] = SPRITE_INDEX_NULL;
        }

        ride->status = RIDE_STATUS_CLOSED;
        ride->lifecycle_flags = 0;
        ride->vehicle_change_timeout = 0;
        ride->num_stations = 0;
        ride->num_vehicles = 1;
        ride->proposed_num_vehicles = 32;
        ride->max_trains = 32;
        ride->num_cars_per_train = 1;
        ride->proposed_num_cars_per_train = 12;
        ride->min_waiting_time = 10;
        ride->max_waiting_time = 60;
        ride->depart_flags = RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH | 3;
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_MUSIC_ON_DEFAULT)
        {
            ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
        }
        ride->music = RideData4[ride->type].default_music;

        auto rideProperties = RideProperties[ride->type];
        ride->operation_option = (rideProperties.min_value * 3 + rideProperties.max_value) / 4;

        ride->lift_hill_speed = RideLiftData[ride->type].minimum_speed;

        ride->measurement_index = 255;
        ride->excitement = (ride_rating)-1;
        ride->cur_num_customers = 0;
        ride->num_customers_timeout = 0;
        ride->chairlift_bullwheel_rotation = 0;

        ride->price = 0;
        ride->price_secondary = 0;
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            ride->price = RideData4[ride->type].price;
            ride->price_secondary = RideData4[ride->type].price_secondary;

            if (rideEntry->shop_item == SHOP_ITEM_NONE)
            {
                if (!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) && !gCheatsUnlockAllPrices)
                {
                    ride->price = 0;
                }
            }
            else
            {
                ride->price = DefaultShopItemPrice[rideEntry->shop_item];
            }
            if (rideEntry->shop_item_secondary != SHOP_ITEM_NONE)
            {
                ride->price_secondary = DefaultShopItemPrice[rideEntry->shop_item_secondary];
            }

            if (gScenarioObjectiveType == OBJECTIVE_BUILD_THE_BEST)
            {
                ride->price = 0;
            }

            if (ride->type == RIDE_TYPE_TOILETS)
            {
                if (shop_item_has_common_price(SHOP_ITEM_ADMISSION))
                {
                    money32 price = ride_get_common_price(ride);
                    if (price != MONEY32_UNDEFINED)
                    {
                        ride->price = (money16)price;
                    }
                }
            }

            if (rideEntry->shop_item != SHOP_ITEM_NONE)
            {
                if (shop_item_has_common_price(rideEntry->shop_item))
                {
                    money32 price = shop_item_get_common_price(ride, rideEntry->shop_item);
                    if (price != MONEY32_UNDEFINED)
                    {
                        ride->price = (money16)price;
                    }
                }
            }

            if (rideEntry->shop_item_secondary != SHOP_ITEM_NONE)
            {
                if (shop_item_has_common_price(rideEntry->shop_item_secondary))
                {
                    money32 price = shop_item_get_common_price(ride, rideEntry->shop_item_secondary);
                    if (price != MONEY32_UNDEFINED)
                    {
                        ride->price_secondary = (money16)price;
                    }
                }
            }

            // Set the on-ride photo price, whether the ride has one or not (except shops).
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP) && shop_item_has_common_price(SHOP_ITEM_PHOTO))
            {
                money32 price = shop_item_get_common_price(ride, SHOP_ITEM_PHOTO);
                if (price != MONEY32_UNDEFINED)
                {
                    ride->price_secondary = (money16)price;
                }
            }
        }

        Memory::Set(ride->num_customers, 0, sizeof(ride->num_customers));
        ride->value = 0xFFFF;
        ride->satisfaction = 255;
        ride->satisfaction_time_out = 0;
        ride->satisfaction_next = 0;
        ride->popularity = 255;
        ride->popularity_time_out = 0;
        ride->popularity_next = 0;
        ride->window_invalidate_flags = 0;
        ride->total_customers = 0;
        ride->total_profit = 0;
        ride->num_riders = 0;
        ride->slide_in_use = 0;
        ride->maze_tiles = 0;
        ride->build_date = gDateMonthsElapsed;
        ride->music_tune_id = 255;

        ride->breakdown_reason = 255;
        ride->upkeep_cost = (money16)-1;
        ride->reliability = RIDE_INITIAL_RELIABILITY;
        ride->unreliability_factor = 1;
        ride->inspection_interval = RIDE_INSPECTION_EVERY_30_MINUTES;
        ride->last_inspection = 0;
        ride->downtime = 0;
        memset(ride->downtime_history, 0, sizeof(ride->downtime_history));
        ride->no_primary_items_sold = 0;
        ride->no_secondary_items_sold = 0;
        ride->last_crash_type = RIDE_CRASH_TYPE_NONE;
        ride->income_per_hour = MONEY32_UNDEFINED;
        ride->profit = MONEY32_UNDEFINED;
        ride->connected_message_throttle = 0;
        ride->entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;
        ride->num_block_brakes = 0;
        ride->guests_favourite = 0;

        ride->num_circuits = 1;
        ride->mode = ride_get_default_mode(ride);
        ride->min_max_cars_per_train = (rideEntry->min_cars_in_train << 4) | rideEntry->max_cars_in_train;
        ride_set_vehicle_colours_to_random_preset(ride, _colour2);
        window_invalidate_by_class(WC_RIDE_LIST);

        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res->Position.x = (uint16)0x8000;

        return std::move(res);
    }
};
