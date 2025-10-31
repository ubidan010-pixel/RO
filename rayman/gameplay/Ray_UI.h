#ifndef _ITF_RAY_UI_H_
#define _ITF_RAY_UI_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

namespace ITF
{
#ifdef ITF_SUPPORT_CHEAT
    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleFriendlyFire : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleFriendlyFire(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryTogglePlayerCrush : public DebugMenuEntry
    {
    public:
        DebugMenuEntryTogglePlayerCrush(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleNpcCrush : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleNpcCrush(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleAllPowerUps : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleAllPowerUps(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleShowHUDs : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleShowHUDs(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleSinglePowerUp : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleSinglePowerUp(const String & _name, const StringID& _powerUpID, const char* _text );
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    private:
        StringID    m_powerUpId;
        const char* m_text;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleDive : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleDive(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleWalkOnWallsGlobal : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleWalkOnWallsGlobal(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleReduction : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleReduction(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleHairlicopter : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleHairlicopter(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleFight : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleFight(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleHeartShieldExclusive : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleHeartShieldExclusive(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleHeartShield : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleHeartShield(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleSuperPunchBasic : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleSuperPunchBasic(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleSuperPunchSeeking : public DebugMenuEntryToggleSinglePowerUp
    {
    public:
        DebugMenuEntryToggleSuperPunchSeeking(const String & _name);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleInvincibleEnemies : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleInvincibleEnemies(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryAddMaxHP : public DebugMenuEntry
    {
    public:
        DebugMenuEntryAddMaxHP(const String & _name, i32 _numHp) : numHp(_numHp), DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;

        const DebugMenuEntryAddMaxHP& operator = ( const DebugMenuEntryAddMaxHP& odbm )
        { // adding this avoids warnings
            ITF_ASSERT_MSG(0, "forbidden operation");
            return *this;
        }
    protected:
        const i32 numHp;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryAddHP : public DebugMenuEntry
    {
    public:
        DebugMenuEntryAddHP(const String & _name, i32 _numHp) : numHp(_numHp), DebugMenuEntry(_name){}
        virtual void select(u32 _player) const ;
        const DebugMenuEntryAddHP& operator = ( const DebugMenuEntryAddHP& odbm )
        { // adding this avoids warnings
            ITF_ASSERT_MSG(0, "forbidden operation");
            return *this;
        }
    protected:
        const i32 numHp;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleInvincibility : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleInvincibility(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryToggleInvincibilityAll : public DebugMenuEntry
    {
    public:
        DebugMenuEntryToggleInvincibilityAll(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    private:
        bbool getAllInvincible() const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryAllWorldMap : public DebugMenuEntry
    {
    public:
        DebugMenuEntryAllWorldMap(const String & _name) : DebugMenuEntry(_name) {}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryAllSkullTeeth : public DebugMenuEntry
    {
    public:
        DebugMenuEntryAllSkullTeeth(const String & _name) : DebugMenuEntry(_name) {}
        virtual void select(u32 _player) const;
        virtual const String getName(u32 _player) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryUnlockAllCostumes : public DebugMenuEntry
    {
    public:
        DebugMenuEntryUnlockAllCostumes(const String & _name) : DebugMenuEntry(_name){}
        virtual void select(u32 _player) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryTriggerToothSequence : public DebugMenuEntry
    {
    public:
        DebugMenuEntryTriggerToothSequence(const String & _name ) : DebugMenuEntry(_name) {}
        virtual void select(u32 _player) const;

    private:

    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryEnableTimeAttack : public DebugMenuEntry
    {
    public:
        DebugMenuEntryEnableTimeAttack(const String & _name ) : DebugMenuEntry(_name) {}
        virtual void select(u32 _player) const;

    private:

    };

	///////////////////////////////////////////////////////////////////////////////////////////
	class DebugMenuEntryUnlockAllBoss : public DebugMenuEntry
	{
	public:
		DebugMenuEntryUnlockAllBoss(const String & _name ) : DebugMenuEntry(_name) {}
		virtual void select(u32 _player) const;
		virtual const String getName(u32 _player) const;

	private:

	};


#endif //ITF_SUPPORT_CHEAT
}

#endif //_ITF_RAY_UI_H_

