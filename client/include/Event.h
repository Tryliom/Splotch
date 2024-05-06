#pragma once

/**
 * Event that can be triggered by the game
 */
enum class Event
{
	PLAYER_LEAVE_GAME,
	START_GAME,
	READY_TO_PLAY,
	WIN_GAME,
	LOSE_GAME
};