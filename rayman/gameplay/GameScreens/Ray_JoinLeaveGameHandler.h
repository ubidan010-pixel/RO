#ifndef _ITF_RAY_JOINLEAVEGAMEHANDLER_H_
#define _ITF_RAY_JOINLEAVEGAMEHANDLER_H_

namespace ITF
{

    class Ray_Player;
    class Ray_JoinLeaveGameHandler
    {
    public:

        static void update();
        static void listenChangePlayerActivation(u32 _playerIndex, bbool _activates);

    private:
        static void mayFillAvailablePlayers();
        static void activatePlayer(u32 _playerInfo);
        static void deactivatePlayer(u32 _playerInfo);
        static u32  getTypeFromPlayer(Ray_Player *_player);
        static void updatePlayerIcons();
    };

} // namespace ITF

#endif
