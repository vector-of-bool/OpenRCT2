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

#include <functional>
#include <memory>

#include "../common.h"
#include "../core/DataSerialiser.h"
#include "../core/IStream.hpp"

#include "../game.h"
#include "../world/map.h"

/**
 * Common error codes for game actions.
 */
enum class GA_ERROR : uint16
{
    OK,
    INVALID_PARAMETERS,
    DISALLOWED,
    GAME_PAUSED,
    INSUFFICIENT_FUNDS,
    NOT_IN_EDITOR_MODE,

    NOT_OWNED,
    TOO_LOW,
    TOO_HIGH,
    NO_CLEARANCE,
    ITEM_ALREADY_PLACED,

    NO_FREE_ELEMENTS,

    UNKNOWN = UINT16_MAX,
};

namespace GA_FLAGS
{
    constexpr uint16 ALLOW_WHILE_PAUSED = 1 << 0;
    constexpr uint16 CLIENT_ONLY        = 1 << 1;
    constexpr uint16 EDITOR_ONLY        = 1 << 2;
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

/**
 * Represents the result of a game action query or execution.
 */
class GameActionResult
{
public:
    typedef std::unique_ptr<GameActionResult> Ptr;

    GA_ERROR        Error = GA_ERROR::OK;
    rct_string_id   ErrorTitle = (rct_string_id)-1;
    rct_string_id   ErrorMessage = (rct_string_id)-1;
    uint8           ErrorMessageArgs[8] = { 0 };
    rct_xyz32       Position = { 0 };
    money32         Cost = 0;
    uint16          ExpenditureType = 0;

    GameActionResult();
    GameActionResult(GA_ERROR error, rct_string_id message);
    GameActionResult(const GameActionResult&) = delete;
    virtual ~GameActionResult() {};
};

struct GameAction
{
public:
    typedef std::unique_ptr<GameAction> Ptr;
    typedef std::function<void(const struct GameAction *, const GameActionResult *)> Callback_t;

private:
    uint32 const _type;

    uint32 _playerId    = 0;    // Callee
    uint32 _flags       = 0;    // GAME_COMMAND_FLAGS
    uint32 _networkId   = 0;
    Callback_t _callback;

public:
    GameAction(uint32 type)
        : _type(type)
    {
    }

    virtual ~GameAction() = default;

    uint32 GetPlayer() const
    {
        return _playerId;
    }

    void SetPlayer(uint32 playerId)
    {
        _playerId = playerId;
    }

    /**
    * Gets the GA_FLAGS flags that are enabled for this game action.
    */
    virtual uint16 GetActionFlags() const
    {
        // Make sure we execute some things only on the client.
        if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) != 0 ||
            (GetFlags() & GAME_COMMAND_FLAG_5) != 0)
        {
            return GA_FLAGS::CLIENT_ONLY;
        }

        return 0;
    }

    /**
    * Currently used for GAME_COMMAND_FLAGS, needs refactoring once everything is replaced.
    */
    uint32 GetFlags() const
    {
        return _flags;
    }

    uint32 SetFlags(uint32 flags)
    {
        return _flags = flags;
    }

    uint32 GetType() const
    {
        return _type;
    }

    void SetCallback(Callback_t cb)
    {
        _callback = cb;
    }

    const Callback_t & GetCallback() const
    {
        return _callback;
    }

    void SetNetworkId(uint32_t id)
    {
        _networkId = id;
    }

    uint32 GetNetworkId() const
    {
        return _networkId;
    }

    virtual void Serialise(DataSerialiser& stream)
    {
        stream << _networkId;
        stream << _flags;
        stream << _playerId;
    }

    // Helper function, allows const Objects to still serialize into DataSerialiser while being const.
    void Serialise(DataSerialiser& stream) const
    {
        return const_cast<GameAction&>(*this).Serialise(stream);
    }

    /**
    * Query the result of the game action without changing the game state.
    */
    virtual GameActionResult::Ptr Query() const abstract;

    /**
    * Apply the game action and change the game state.
    */
    virtual GameActionResult::Ptr Execute() const abstract;
};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#pragma GCC diagnostic pop
#endif

template<uint32 TType, typename TResultType>
struct GameActionBase : GameAction
{
public:
    typedef TResultType Result;

    static constexpr uint32 TYPE = TType;

    GameActionBase()
        : GameAction(TYPE)
    {
    }

    void SetCallback(std::function<void(const struct GameAction *, const TResultType *)> typedCallback)
    {
        GameAction::SetCallback([typedCallback](const GameAction * ga, const GameActionResult * result)
        {
            typedCallback(ga, static_cast<const TResultType *>(result));
        });
    }
};

typedef GameAction *(*GameActionFactory)();

namespace GameActions
{
    void                    Initialize();
    void                    Register();
    GameAction::Ptr         Create(uint32 id);
    GameActionResult::Ptr   Query(const GameAction * action);
    GameActionResult::Ptr   Execute(const GameAction * action);
    GameActionFactory       Register(uint32 id, GameActionFactory action);

    template<typename T>
    static GameActionFactory Register()
    {
        GameActionFactory factory = []() -> GameAction *
        {
            return new T();
        };
        Register(T::TYPE, factory);
        return factory;
    }
}
