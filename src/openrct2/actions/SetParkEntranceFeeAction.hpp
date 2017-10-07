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

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

#include "../cheats.h"
#include "../interface/window.h"
#include "../world/park.h"

struct SetParkEntranceFeeAction : public GameActionBase<GAME_COMMAND_SET_PARK_ENTRANCE_FEE, GameActionResult>
{
private:
    money16 _fee;

public:
    SetParkEntranceFeeAction() {}
    SetParkEntranceFeeAction(money16 fee) 
        : _fee(fee)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _fee;
    }

    GameActionResult::Ptr Query() const override
    {
        bool noMoney = (gParkFlags & PARK_FLAGS_NO_MONEY) != 0;
        bool forceFreeEntry = (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) && !gCheatsUnlockAllPrices;
        if (noMoney || forceFreeEntry)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, STR_NONE);
        }
        if (_fee < MONEY_FREE || _fee > MONEY(100,00))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        gParkEntranceFee = _fee;
        window_invalidate_by_class(WC_PARK_INFORMATION);
        return std::make_unique<GameActionResult>();
    }
};

