#include "precompiled_gameplay.h"

#ifndef _ITF_GAMEPLAYFACTORYFILLER_H_
#include "gameplay/GameplayFactoryFiller.h"
#endif //_ITF_GAMEPLAYFACTORYFILLER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_



/********* ENGINE COMPONENTS ***********/

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/AnimatedComponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_
#ifndef _ITF_STANDEECOMPONENT_H_
#include "engine/actors/components/StandeeComponent.h"
#endif
#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef _ITF_AFTERFXCOMPONENT_H_
#include "engine/actors/components/AfterFxComponent.h"
#endif //_ITF_AFTERFXCOMPONENT_H_

#ifndef _ITF_PARTICLEGENERATORCOMPONENT_H_
#include "engine/actors/components/ParticleGeneratorComponent.h"
#endif //_ITF_PARTICLEGENERATORCOMPONENT_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#include "engine/actors/components/SoundComponent.h"
#include "engine\sound\SoundConfigTemplate.h"


#ifndef _ITF_SOUNDBUSCONTROLLERCOMPONENT_H_
#include "engine/actors/components/SoundBusControllerComponent.h"
#endif //_ITF_SOUNDBUSCONTROLLERCOMPONENT_H_

#ifndef _ITF_MUSICCONTROLLERCOMPONENT_H_
#include "engine/actors/components/MusicControllerComponent.h"
#endif //_ITF_MUSICCONTROLLERCOMPONENT_H_

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_FIXEDCAMERACOMPONENT_H_
#include "engine/actors/components/Camera/FixedCameraComponent.h"
#endif //_ITF_FIXEDCAMERACOMPONENT_H_

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SEQUENCETRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/SequenceLauncherComponent.h"
#endif //_ITF_SEQUENCETRIGGERCOMPONENT_H_

#ifndef _ITF_SPAWNMANAGERCOMPONENT_H_
#include "gameplay/Managers/SpawnManagerComponent.h"
#endif //_ITF_SPAWNMANAGERCOMPONENT_H_

#ifndef _ITF_KEEPALIVECOMPONENT_H_
#include "engine/actors/components/KeepAlivecomponent.h"
#endif //_ITF_KEEPALIVECOMPONENT_H_

#ifndef _ITF_RELAYEVENTCOMPONENT_H_
#include "gameplay/Components/Misc/RelayEventComponent.h"
#endif //_ITF_RELAYEVENTCOMPONENT_H_

#ifndef _ITF_FRIEZECONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/FriezeControllerComponent.h"
#endif //_ITF_FRIEZECONTROLLERCOMPONENT_H_

#ifndef _ITF_OBJECTCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/ObjectControllerComponent.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENT_H_

#ifndef _ITF_FRIEZECONTACTDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/FriezeContactDetectorComponent.h"
#endif //_ITF_FRIEZECONTACTDETECTORCOMPONENT_H_

#ifndef _ITF_SUBSCENECONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/SubsceneControllerComponent.h"
#endif //_ITF_SUBSCENECONTROLLERCOMPONENT_H_

#ifndef _ITF_PREFETCH_COMPONENT_H_
#include "gameplay/components/misc/PrefetchComponent.h"
#endif //_ITF_PREFETCH_COMPONENT_H_

#ifndef _ITF_AABBPREFETCH_COMPONENT_H_
#include "gameplay/components/misc/AABBPrefetchComponent.h"
#endif //_ITF_PREFETCH_COMPONENT_H_

#ifndef _ITF_DUMMYCOMPONENT_H_
#include "engine/actors/Components/DummyComponent.h"
#endif //_ITF_DUMMYCOMPONENT_H_

#ifndef _ITF_MOVIEPLAYERCOMPONENT_H_
#include "engine/actors/components/MoviePlayerComponent.h"
#endif //_ITF_MOVIEPLAYERCOMPONENT_H_

#ifndef _ITF_UIMENUSOUNDCOMPONENT_H_
#include "engine/actors/components/UIMenuSoundComponent.h"
#endif //_ITF_UIMENUSOUNDCOMPONENT_H_



/********* GAMEPLAY COMPONENTS ***********/

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_DEATHDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DeathDetectorComponent.h"
#endif //_ITF_DEATHDETECTORCOMPONENT_H_


#ifndef _ITF_PHANTOMDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PhantomDetectorComponent.h"
#endif //_ITF_PHANTOMDETECTORCOMPONENT_H_

#ifndef _ITF_CAMERADETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/CameraDetectorComponent.h"
#endif //_ITF_CAMERADETECTORCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_ANIMMARKERANIMMARKERTRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/AnimMarkerTriggerComponent.h"
#endif //_ITF_ANIMMARKERANIMMARKERTRIGGERCOMPONENT_H_

#ifndef _ITF_PHANTOMTRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/PhantomTriggerComponent.h"
#endif //_ITF_PHANTOMTRIGGERCOMPONENT_H_

#ifndef _ITF_TRIGGERONBUTTONPRESSEDCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerOnButtonPressedComponent.h"
#endif //_ITF_TRIGGERONBUTTONPRESSEDCOMPONENT_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

#ifndef _ITF_TRAJECTORYSPAWNERCOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectorySpawnerComponent.h"
#endif //_ITF_TRAJECTORYSPAWNERCOMPONENT_H_

#ifndef _ITF_CURVECOMPONENT_H_
#include "gameplay/Components/Misc/CurveComponent.h"
#endif //_ITF_CURVECOMPONENT_H_

#ifndef _ITF_CURVEFOLLOWERCOMPONENT_H_
#include "gameplay/Components/Misc/CurveFollowerComponent.h"
#endif //_ITF_CURVEFOLLOWERCOMPONENT_H_

#ifndef _ITF_BOXINTERPOLATORCOMPONENT_H_
#include "gameplay/Components/FX/BoxInterpolatorComponent.h"
#endif //_ITF_BOXINTERPOLATORCOMPONENT_H_

#ifndef _ITF_CIRCLEINTERPOLATORCOMPONENT_H_
#include "gameplay/Components/FX/CircleInterpolatorComponent.h"
#endif //_ITF_CIRCLEINTERPOLATORCOMPONENT_H_

#ifndef _ITF_COLORCOMPUTERCOMPONENT_H_
#include "engine/actors/components/colorComputercomponent.h"
#endif //_ITF_COLORCOMPUTERCOMPONENT_H_

#ifndef _ITF_SPEEDINPUTPROVIDERCOMPONENT_H_
#include "gameplay/Components/FX/SpeedInputProviderComponent.h"
#endif //_ITF_SPEEDINPUTPROVIDERCOMPONENT_H_

#ifndef _ITF_SWARMCOMPONENT_H_
#include "gameplay/components/misc/SwarmComponent.h"
#endif //_ITF_SWARMCOMPONENT_H_

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_
#include "gameplay/components/misc/EventReceiveAnimPlayComponent.h"
#endif //_ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_

#ifndef _ITF_PARTICLEPHYSCOMPONENT_H_
#include "gameplay/Components/Common/ParticlePhysComponent.h"
#endif //_ITF_PARTICLEPHYSCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/Common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_BOUNCEONPOLYLINEPHYSCOMPONENT_
#include "gameplay/Components/Common/BounceOnPolylinePhysComponent.h"
#endif //_ITF_BOUNCEONPOLYLINEPHYSCOMPONENT_

#ifndef _ITF_CHARACTERDBUGGERCOMPONENT_H_
#include "gameplay/components/Common/CharacterDebuggerComponent.h"
#endif //_ITF_CHARACTERDBUGGERCOMPONENT_H_

#ifndef _ITF_GRAVITYCOMPONENT_H_
#include "gameplay/components/misc/GravityComponent.h"
#endif //_ITF_GRAVITYCOMPONENT_H_

#ifndef _ITF_WINDCOMPONENT_H_
#include "gameplay/components/misc/WindComponent.h"
#endif //_ITF_WINDCOMPONENT_H_

#ifndef _ITF_SOFTPLATFORMCOMPONENT_H_
#include "gameplay/components/Environment/SoftPlatformComponent.h"
#endif //_ITF_SOFTPLATFORMCOMPONENT_H_

#ifndef _ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_
#include "gameplay/components/Environment/ProceduralSoftPlatformComponent.h"
#endif //_ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_

#ifndef _ITF_PENDULUMCOMPONENT_H_
#include "gameplay/Components/Environment/PendulumComponent.h"
#endif //_ITF_PENDULUMCOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_POINTSCOLLISIONCOMPONENT_H_
#include "gameplay/Components/Environment/PointsCollisionComponent.h"
#endif //_ITF_POINTSCOLLISIONCOMPONENT_H_

#ifndef _ITF_SOLIDPOLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/SolidPolylineComponent.h"
#endif //_ITF_SOLIDPOLYLINECOMPONENT_H_

#ifndef _ITF_AXISPOLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/AxisPolylineComponent.h"
#endif //_ITF_AXISPOLYLINECOMPONENT_H_

#ifndef _ITF_ROTATINGPOLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/RotatingPolylineComponent.h"
#endif //_ITF_ROTATINGPOLYLINECOMPONENT_H_

#ifndef _ITF_HINGEPLATFORMCOMPONENT_H_
#include "gameplay/Components/Environment/HingePlatformComponent.h"
#endif //_ITF_HINGEPLATFORMCOMPONENT_H_

#ifndef _ITF_WAYPOINTCOMPONENT_H_
#include "gameplay/Components/AI/WayPointComponent.h"
#endif //_ITF_WAYPOINTCOMPONENT_H_

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_

#ifndef _ITF_TELEPORTERCOMPONENT_H_
#include "gameplay/components/misc/TeleporterComponent.h"
#endif //_ITF_TELEPORTERCOMPONENT_H_

#ifndef _ITF_TRAILCOMPONENT_H_
#include "gameplay/components/misc/TrailComponent.h"
#endif //_ITF_TRAILCOMPONENT_H_

#ifndef _ITF_TRAIL3DCOMPONENT_H_
#include "gameplay/components/misc/Trail3DComponent.h"
#endif //_ITF_TRAIL3DCOMPONENT_H_

#ifndef _ITF_LINKCURVECOMPONENT_H_
#include "gameplay/Components/Misc/LinkCurveComponent.h"
#endif //_ITF_LINKCURVECOMPONENT_H_

#ifndef _ITF_PATCHCURVECOMPONENT_H_
#include "gameplay/Components/Misc/PatchCurveComponent.h"
#endif //_ITF_PATCHCURVECOMPONENT_H_

#ifndef _ITF_RENDERBEZIERPATCHCURVECOMPONENT_H_
#include "gameplay/Components/Display/RenderBezierPatchCurveComponent.h"
#endif //_ITF_RENDERBEZIERPATCHCURVECOMPONENT_H_

#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#include "gameplay/Components/Misc/BezierCurveComponent.h"
#endif //_ITF_BEZIERCURVECOMPONENT_H_

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_ACTORSPAWNCOMPONENT_H_
#include "gameplay/Components/Common/ActorSpawnComponent.h"
#endif //_ITF_ACTORSPAWNCOMPONENT_H_

#ifndef _ITF_BTAICOMPONENT_H_
#include "gameplay/Components/AI/BTAIComponent.h"
#endif //_ITF_BTAICOMPONENT_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_GENERICAICOMPONENT_H_
#include "gameplay/Components/AI/GenericAIComponent.h"
#endif //_ITF_GENERICAICOMPONENT_H_

#ifndef _ITF_SIMPLEAICOMPONENT_H_
#include "gameplay/Components/AI/SimpleAIComponent.h"
#endif //_ITF_SIMPLEAICOMPONENT_H_

#ifndef _ITF_TIMEDSPAWNERAICOMPONENT_H_
#include "gameplay/Components/AI/TimedSpawnerAIComponent.h"
#endif //_ITF_TIMEDSPAWNERAICOMPONENT_H_

#ifndef _ITF_UIMENUITEMTEXT_H_
#include "gameplay/components/UI/UIMenuItemText.h"
#endif //_ITF_UIMENUITEMTEXT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_TUTORIALTEXTCOMPONENT_H_
#include "gameplay/Components/UI/TutorialTextComponent.h"
#endif //_ITF_TUTORIALTEXTCOMPONENT_H_

#ifndef _ITF_INGAMETEXTCOMPONENT_H_
#include "gameplay/Components/UI/InGameTextComponent.h"
#endif //_ITF_INGAMETEXTCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_UIBUTTONCOMPONENT_H_
#include "gameplay/components/UI/UIButtonComponent.h"
#endif //_ITF_UIBUTTONCOMPONENT_H_

#ifndef _ITF_UISLIDERCOMPONENT_H_
#include "gameplay/components/UI/UISliderComponent.h"
#endif //_ITF_UISLIDERCOMPONENT_H_

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
#endif //_ITF_UIGAMEOPTIONCOMPONENT_H_

#ifndef _ITF_UILISTOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIListOptionComponent.h"
#endif //_ITF_UILISTOPTIONCOMPONENT_H_

#ifndef _ITF_UITOGGLEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIToggleOptionComponent.h"
#endif //_ITF_UITOGGLEOPTIONCOMPONENT_H_

#ifndef _ITF_UIFLOATOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIFloatOptionComponent.h"
#endif //_ITF_UIFLOATOPTIONCOMPONENT_H_

#ifndef _ITF_UIPROFILESLOTCOMPONENT_H_
#include "gameplay/components/UI/UIProfileSlotComponent.h"
#endif //_ITF_UIPROFILESLOTCOMPONENT_H_

#ifndef _ITF_UIACTIONBUTTONCOMPONENT_H_
#include "gameplay/components/UI/UIActionButtonComponent.h"
#endif //_ITF_UIACTIONBUTTONCOMPONENT_H_

#ifndef _ITF_SIMPLETEXTCOMPONENT_H_
#include "gameplay/components/UI/SimpleTextComponent.h"
#endif //_ITF_SIMPLETEXTCOMPONENT_H_

#ifndef _ITF_ANIMTRIGGEREDCOMPONENT_H_
#include "gameplay/components/misc/AnimTriggeredComponent.h"
#endif //_ITF_ANIMTRIGGEREDCOMPONENT_H_

#ifndef _ITF_PLAYANIMONEVENTCOMPONENT_H_
#include "gameplay/components/misc/PlayAnimOnWeightChangeComponent.h"
#endif //_ITF_PLAYANIMONEVENTCOMPONENT_H_

#ifndef _ITF_PLAYANIMONEVENTRECEIVECOMPONENT_H_
#include "gameplay/components/misc/PlayAnimOnEventReceiveComponent.h"
#endif //_ITF_PLAYANIMONEVENTRECEIVECOMPONENT_H_

#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#include "gameplay/Components/Common/TimedSpawnerComponent.h"
#endif //_ITF_TIMEDSPAWNERCOMPONENT_H_

#ifndef _ITF_CONSTANTMOVEMENTCOMPONENT_H_
#include "gameplay/Components/Common/ConstantMovementComponent.h"
#endif //_ITF_CONSTANTMOVEMENTCOMPONENT_H_

#ifndef _ITF_POOLACTORCOMPONENT_H_
#include "gameplay/Components/Common/PoolActorComponent.h"
#endif //_ITF_POOLACTORCOMPONENT_H_

#ifndef _ITF_TODOCOMPONENT_H_
#include "gameplay/components/misc/TODOComponent.h"
#endif //_ITF_TODOCOMPONENT_H_

#ifndef _ITF_SHAPECOMPONENT_H_
#include "gameplay/Components/Misc/ShapeComponent.h"
#endif //_ITF_SHAPECOMPONENT_H_

#ifndef _ITF_TRAJECTORYFOLLOWERCOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryFollowerComponent.h"
#endif //_ITF_TRAJECTORYFOLLOWERCOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PLAYANIMBASEDONBONEANGLECOMPONENT_H_
#include "gameplay/Components/Misc/PlayAnimBasedOnBoneAngleComponent.h"
#endif //_ITF_PLAYANIMBASEDONBONEANGLECOMPONENT_H_

#ifndef _ITF_PLAYANIMONTOUCHPOLYLINECOMPONENT_H_
#include "gameplay/components/misc/PlayAnimOnTouchPolylineComponent.h"
#endif //_ITF_PLAYANIMONTOUCHPOLYLINECOMPONENT_H_

#ifndef _ITF_AFTERFXCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/AfterFxControllerComponent.h"
#endif //_ITF_AFTERFXCONTROLLERCOMPONENT_H_

#ifndef _ITF_ARENADOORCOMPONENT_H_
#include "gameplay/Components/Misc/ArenaDoorComponent.h"
#endif //_ITF_ARENADOORCOMPONENT_H_

#ifndef _ITF_FLYINGCOMPONENT_H_
#include "gameplay/Components/Misc/FlyingComponent.h"
#endif //_ITF_FLYINGCOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_UIMENUPAGECOMPONENT_H_
#include "gameplay/components/UI/UIMenuPageComponent.h"
#endif //_ITF_UIMENUPAGECOMPONENT_H_

#ifndef _ITF_UIMENUITEMCOMPONENT_H_
#include "gameplay/components/UI/UIMenuItemComponent.h"
#endif //_ITF_UIMENUITEMCOMPONENT_H_

#ifndef _ITF_SYNCHRONIZEDANIMCOMPONENT_H_
#include "gameplay/Components/Misc/SynchronizedAnimComponent.h"
#endif //_ITF_SYNCHRONIZEDANIMCOMPONENT_H_

#ifndef _ITF_ALWAYSACTIVEACTORCOMPONENT_H_
#include "gameplay/Components/Misc/AlwaysActiveActorComponent.h"
#endif //_ITF_ALWAYSACTIVEACTORCOMPONENT_H_

#ifndef _ITF_TPCONTROLERCOMPONENT_H_
#include "engine/actors/components/TpControlerComponent.h"
#endif //_ITF_TPCONTROLERCOMPONENT_H_

#ifndef _ITF_ADDITIONALBEHAVIORSCOMPONENT_H_
#include "gameplay/Components/AI/AdditionalBehaviorsComponent.h"
#endif //_ITF_ADDITIONALBEHAVIORSCOMPONENT_H_

#ifndef _ITF_DEFORMONTRAJECTORYCOMPONENT_H_
#include "gameplay/Components/Common/DeformOnTrajectoryComponent.h"
#endif //_ITF_DEFORMONTRAJECTORYCOMPONENT_H_

#ifndef _ITF_VIRTUALLINKCOMPONENT_H_
#include "gameplay/components/misc/VirtualLinkComponent.h"
#endif //_ITF_VIRTUALLINKCOMPONENT_H_

#ifndef _ITF_TOGGLEANIMONEVENTCOMPONENT_H_
#include "gameplay/components/misc/ToggleAnimOnEventComponent.h"
#endif //_ITF_TOGGLEANIMONEVENTCOMPONENT_H_

#ifndef _ITF_ATLASANIMATIONCOMPONENT_H_
#include "engine/actors/components/AtlasAnimationComponent.h"
#endif //_ITF_ATLASANIMATIONCOMPONENT_H_


/********* GAMEPLAY AI Behaviours ***********/

#ifndef _ITF_AISIMPLEPLAYANIMBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AISimplePlayAnimBehavior.h"
#endif //_ITF_AISIMPLEPLAYANIMBEHAVIOR_H_

#ifndef _ITF_AIDEATHBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIDeathBehavior.h"
#endif //_ITF_AIDEATHBEHAVIOR_H_

#ifndef _ITF_AIROAMINGBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIRoamingBehavior.h"
#endif //_ITF_AIROAMINGBEHAVIOR_H_

#ifndef _ITF_AIPLAYACTIONSBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIPlayActionsBehavior.h"
#endif //_ITF_AIPLAYACTIONSBEHAVIOR_H_

#ifndef _ITF_AIREPLICATEPARENTANIMBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIReplicateParentAnimBehavior.h"
#endif //_ITF_AIREPLICATEPARENTANIMBEHAVIOR_H_


/********* GAMEPLAY AI Actions ***********/

#ifndef _ITF_AIFALLACTION_H_
#include "gameplay/ai/Actions/AIFallAction.h"
#endif //_ITF_AIFALLACTION_H_

#ifndef _ITF_AIFALLNOPHYSACTION_H_
#include "gameplay/ai/Actions/AIFallNoPhysAction.h"
#endif //_ITF_AIFALLNOPHYSACTION_H_

#ifndef _ITF_AIFOLLOWACTORACTION_H_
#include "gameplay/ai/Actions/AIFollowActorAction.h"
#endif //_ITF_AIFOLLOWACTORACTION_H_

#ifndef _ITF_AIFOLLOWCOLLIDEACTORACTION_H_
#include "gameplay/ai/Actions/AIFollowCollideActorAction.h"
#endif //_ITF_AIFOLLOWCOLLIDEACTORACTION_H_

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_

#ifndef _ITF_AIJUMPANGLEACTION_H_
#include "gameplay/ai/Actions/AIJumpAngleAction.h"
#endif //_ITF_AIJUMPANGLEACTION_H_

#ifndef _ITF_AIJUMPINDIRACTION_H_
#include "gameplay/ai/Actions/AIJumpInDirAction.h"
#endif //_ITF_AIJUMPINDIRACTION_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_AISTICKBONEACTION_H_
#include "gameplay/ai/Actions/AIStickBoneAction.h"
#endif //_ITF_AISTICKBONEACTION_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_AIUTURNACTION_H_
#include "gameplay/ai/Actions/AIUTurnAction.h"
#endif //_ITF_AIUTURNACTION_H_

#ifndef _ITF_AIFADEACTION_H_
#include "gameplay/ai/Actions/AIFadeAction.h"
#endif //_ITF_AIFADEACTION_H_

#ifndef _ITF_AIPLAYFXACTION_H_
#include "gameplay/ai/Actions/AIPlayFXAction.h"
#endif //_ITF_AIPLAYFXACTION_H_

#ifndef _ITF_AIBALLISTICSAPEXACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsApexAction.h"
#endif //_ITF_AIBALLISTICSAPEXACTION_H_

#ifndef _ITF_AIBALLISTICSFIXEDGRAVITYACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsFixedGravityAction.h"
#endif //_ITF_AIBALLISTICSFIXEDGRAVITYACTION_H_

#ifndef _ITF_AIBEZIERACTION_H_
#include "gameplay/ai/Actions/AIBezierAction.h"
#endif //_ITF_AIBEZIERACTION_H_

#ifndef _ITF_AIJUMPTOTARGETACTION_H_
#include "gameplay/ai/Actions/AIJumpToTargetAction.h"
#endif //_ITF_AIJUMPTOTARGETACTION_H_

#ifndef _ITF_AIBOUNCEACTION_H_
#include "gameplay/ai/Actions/AIBounceAction.h"
#endif //_ITF_AIBOUNCEACTION_H_

#ifndef _ITF_AISPAWNACTION_H_
#include "gameplay/ai/Actions/AISpawnAction.h"
#endif //_ITF_AISPAWNACTION_H_

#ifndef _ITF_AIDESTROYACTION_H_
#include "gameplay/ai/Actions/AIDestroyAction.h"
#endif //_ITF_AIDESTROYACTION_H_

#ifndef _ITF_AIDISABLEACTION_H_
#include "gameplay/ai/Actions/AIDisableAction.h"
#endif //_ITF_AIDISABLEACTION_H_

#ifndef _ITF_AIBUMPERACTION_H_
#include "gameplay/ai/Actions/AIBumperAction.h"
#endif //_ITF_AIBUMPERACTION_H_

#ifndef _ITF_AIBOUNCETOLAYERACTION_H_
#include "gameplay/ai/Actions/AIBounceToLayerAction.h"
#endif //_ITF_AIBOUNCETOLAYERACTION_H_

#ifndef _ITF_AISENDEVENTACTION_H_
#include "gameplay/ai/Actions/AISendEventAction.h"
#endif //_ITF_AISENDEVENTACTION_H_

/********* SEQUENCE EVENTS ***********/

#ifndef _ITF_EXECUTELUA_EVT_H_
#include "engine/sequenceplayer/seq_events/ExecuteLUA_evt.h"
#endif //_ITF_EXECUTELUA_EVT_H_

#ifndef _ITF_PLAYANIM_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayAnim_evt.h"
#endif //_ITF_PLAYANIM_EVT_H_

#ifndef _ITF_PLAYLOOP_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayLoop_evt.h"
#endif //_ITF_PLAYLOOP_EVT_H_

#ifndef _ITF_PLAYSKIP_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySkip_evt.h"
#endif //_ITF_PLAYSKIP_EVT_H_

#ifndef _ITF_LABEL_EVT_H_
#include "engine/sequenceplayer/seq_events/Label_evt.h"
#endif //_ITF_LABEL_EVT_H_

#ifndef _ITF_PLAYSPAWN_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySpawn_evt.h"
#endif //_ITF_PLAYSPAWN_EVT_H_

#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTrajectory_evt.h"
#endif //_ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_STARTCAPTURE_EVT_H_
#include "engine/sequenceplayer/seq_events/StartCapture_evt.h"
#endif //_ITF_STARTCAPTURE_EVT_H_

#ifndef _ITF_PLAYGAMEPLAYEVENT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayGameplay_evt.h"
#endif //_ITF_PLAYGAMEPLAYEVENT_EVT_H_

#ifndef _ITF_PLAYWAIT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayWait_evt.h"
#endif //_ITF_PLAYWAIT_EVT_H_

#ifndef _ITF_PLAYSOUND_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySound_evt.h"
#endif //_ITF_PLAYSOUND_EVT_H_

#ifndef _ITF_PLAYTEXT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayText_evt.h"
#endif //_ITF_PLAYTEXT_EVT_H_

#ifndef _ITF_PLAYTEXTBANNER_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTextBanner_evt.h"
#endif //_ITF_PLAYTEXTBANNER_EVT_H_

#ifndef _ITF_REDIRECTPATCHBANK_EVT_H_
#include "engine/sequenceplayer/seq_events/RedirectPatchBank_evt.h"
#endif //_ITF_REDIRECTPATCHBANK_EVT_H_

// AnimTree Nodes
#ifndef _ITF_ANIMTREENODEBRANCHTRANSITION_H_
#include "engine/blendTree/animTree/AnimTreeNodeBranchTransition.h"
#endif //_ITF_ANIMTREENODEBRANCHTRANSITION_H_

#ifndef _ITF_ANIMTREENODERANDOMBRANCH_H_
#include "engine/blendTree/animTree/AnimTreeNodeRandomBranch.h"
#endif //_ITF_ANIMTREENODERANDOMBRANCH_H_

#ifndef _ITF_ANIMTREENODEMARCHBLEND_H_
#include "engine/blendTree/animTree/AnimTreeNodeMarchBlend.h"
#endif //_ITF_ANIMTREENODEMARCHBLEND_H_

#ifndef _ITF_ANIMTREENODEPLAYANIM_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnim.h"
#endif //_ITF_ANIMTREENODEPLAYANIM_H_

#ifndef _ITF_ANIMTREENODEPLAYANIMINTERPOLATECURSOR_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnimInterpolateCursor.h"
#endif //_ITF_ANIMTREENODEPLAYANIMINTERPOLATECURSOR_H_

#ifndef _ITF_ANIMTREENODEPLAYANIMPROCEDURALSMOOTH_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnimProceduralSmooth.h"
#endif //_ITF_ANIMTREENODEPLAYANIMPROCEDURALSMOOTH_H_

#ifndef _ITF_ANIMTREENODEPLAYONOFF_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayOnOff.h"
#endif //_ITF_ANIMTREENODEPLAYONOFF_H_

#ifndef _ITF_ANIMTREENODESEQUENCE_H_
#include "engine/blendTree/AnimTree/AnimTreeNodeSequence.h"
#endif //_ITF_ANIMTREENODESEQUENCE_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_


// MusicTree Nodes
#ifndef _ITF_MUSICTREERESULT_H_
#include "engine/blendTree/MusicTree/MusicTreeResult.h"
#endif //_ITF_MUSICTREERESULT_H_

#ifndef _ITF_MUSICTREENODEPLAYMUSIC_H_
#include "engine/blendTree/MusicTree/MusicTreeNodePlayMusic.h"
#endif //_ITF_MUSICTREENODEPLAYMUSIC_H_

#ifndef _ITF_MUSICTREENODESEQUENCE_H_
#include "engine/blendTree/MusicTree/MusicTreeNodeSequence.h"
#endif //_ITF_MUSICTREENODESEQUENCE_H_

#ifndef _ITF_MUSICTREENODERANDOM_H_
#include "engine/blendTree/MusicTree/MusicTreeNodeRandom.h"
#endif //_ITF_MUSICTREENODERANDOM_H_


// Behavior trees
#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTActionPlayAnim.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_BTACTIONWALKTOTARGET_H_
#include "gameplay/ai/BTs/BTActionWalkToTarget.h"
#endif // _ITF_BTACTIONWALKTOTARGET_H_

#ifndef _ITF_BTACTIONSTAYIDLE_H_
#include "gameplay/ai/BTs/BTActionStayIdle.h"
#endif // _ITF_BTACTIONSTAYIDLE_H_

#ifndef _ITF_BTACTIONSTAYIDLELOOKAT_H_
#include "gameplay/ai/BTs/BTActionStayIdleLookAt.h"
#endif // _ITF_BTACTIONSTAYIDLELOOKAT_H_

#ifndef _ITF_BTACTIONJUMPTOBACK_H_
#include "gameplay/ai/BTs/BTActionJumpToBack.h"
#endif // _ITF_BTACTIONJUMPTOBACK_H_

#ifndef _ITF_BTACTIONJUMPTOTARGET_H_
#include "gameplay/ai/BTs/BTActionJumpTotarget.h"
#endif // _ITF_BTACTIONJUMPTOTARGET_H_

#ifndef _ITF_BTACTIONREMOVEFACT_H_
#include "gameplay/ai/BTs/BTActionRemoveFact.h"
#endif // _ITF_BTACTIONREMOVEFACT_H_

#ifndef _ITF_BTACTIONSETFACT_H_
#include "gameplay/ai/BTs/BTActionSetFact.h"
#endif // _ITF_BTACTIONSETFACT_H_

#ifndef _ITF_BTACTIONCOPYFACT_H_
#include "gameplay/ai/BTs/BTActionCopyFact.h"
#endif // _ITF_BTACTIONCOPYFACT_H_

#ifndef _ITF_BTACTIONSENDEVENTTOACTOR_H_
#include "gameplay/ai/BTs/BTActionSendEventToActor.h"
#endif // _ITF_BTACTIONSENDEVENTTOACTOR_H_

#ifndef _ITF_BTDECIDERHASFACT_H_
#include "gameplay/ai/BTs/BTDeciderHasFact.h"
#endif // _ITF_BTDECIDERHASFACT_H_

#ifndef _ITF_BTDECIDERFACTEQUAL_H_
#include "gameplay/ai/BTs/BTDeciderFactEqual.h"
#endif // _ITF_BTDECIDERFACTEQUAL_H_

#ifndef _ITF_BTDECIDERHASPLAYERNEAR_H_
#include "gameplay/ai/BTs/BTDeciderHasPlayerNear.h"
#endif // _ITF_BTDECIDERHASPLAYERNEAR_H_

#ifndef _ITF_BTDECIDERHASPLAYERBEHIND_H_
#include "gameplay/ai/BTs/BTDeciderHasPlayerBehind.h"
#endif // _ITF_BTDECIDERHASPLAYERBEHIND_H_

#ifndef _ITF_BTDECIDERHASACTORSALIVE_H_
#include "gameplay/ai/BTs/BTDeciderHasActorsAlive.h"
#endif // _ITF_BTDECIDERHASACTORSALIVE_H_

#ifndef _ITF_BTTIMER_H_
#include "gameplay/ai/BTs/BTTimer.h"
#endif // _ITF_BTTIMER_H_

#ifndef _ITF_BTDELAY_H_
#include "gameplay/ai/BTs/BTDelay.h"
#endif // _ITF_BTDELAY_H_

#ifndef _ITF_BTSEQUENCE_H_
#include "gameplay/ai/BTs/BTSequence.h"
#endif // _ITF_BTSEQUENCE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_SAVENOTIFICATIONCOMPONENT_H_
#include "gameplay/Components/Misc/SaveNotificationComponent.h"
#endif //_ITF_SAVENOTIFICATIONCOMPONENT_H_

#ifndef _ITF_ACTIVEBROADCASTCOMPONENT_H_
#include "gameplay/Components/Misc/ActiveBroadcastComponent.h"
#endif //_ITF_ACTIVEBROADCASTCOMPONENT_H_

namespace ITF
{
    void GameplayFactoryFiller::fillFactories()
    {
        fillEngineComponents();
        fillGameplayComponents();
        fillTemplateComponents();
        fillAIComponents();
        fillSequenceComponents();
        fillEventComponents();
        fillAnimTreeComponents();
        fillMusicTreeComponents();
        fillBtComponents();
        fillClassComponents();
    }

    /********* ENGINE COMPONENTS ***********/
    void GameplayFactoryFiller::fillEngineComponents()
    {
        ObjectFactory* actorComponentsFactory = ACTORSMANAGER->getActorComponentsFactory();

        // Graphics
        actorComponentsFactory->RegisterObject<AnimLightComponent>(ITF_GET_STRINGID_CRC(AnimLightComponent,2800021434));
        actorComponentsFactory->RegisterObject<AnimatedComponent>(ITF_GET_STRINGID_CRC(AnimatedComponent,1654726928));
        actorComponentsFactory->RegisterObject<TextureGraphicComponent>(ITF_GET_STRINGID_CRC(TextureGraphicComponent,2068359598));
        actorComponentsFactory->RegisterObject<StandeeComponent>(ITF_GET_STRINGID_CRC(StandeeComponent,4081679346));
        actorComponentsFactory->RegisterObject<TextureGraphicComponent2D>(ITF_GET_STRINGID_CRC(TextureGraphicComponent2D,1712599772));
        actorComponentsFactory->RegisterObject<AfterFxComponent>(ITF_GET_STRINGID_CRC(AfterFxComponent,1914573461));
        actorComponentsFactory->RegisterObject<ParticleGeneratorComponent>(ITF_GET_STRINGID_CRC(ParticleGeneratorComponent,1621341835));
        actorComponentsFactory->RegisterObject<FxBankComponent>(ITF_GET_STRINGID_CRC(FxBankComponent,2523615645));

        // Sound
        actorComponentsFactory->RegisterObject<SoundComponent>(ITF_GET_STRINGID_CRC(SoundComponent,2111333436));
        actorComponentsFactory->RegisterObject<SoundBusControllerComponent>(ITF_GET_STRINGID_CRC(SoundBusControllerComponent,2952737971));
        actorComponentsFactory->RegisterObject<UIMenuSoundComponent>(ITF_GET_STRINGID_CRC(UIMenuSoundComponent,3788816759));

        // Music
        actorComponentsFactory->RegisterObject<MusicControllerComponent>(ITF_GET_STRINGID_CRC(MusicControllerComponent,2032749090));

        // Video
        actorComponentsFactory->RegisterObject<MoviePlayerComponent>(ITF_GET_STRINGID_CRC(MoviePlayerComponent,57059588));

        // Camera
        actorComponentsFactory->RegisterObject<InGameCameraComponent>(ITF_GET_STRINGID_CRC(InGameCameraComponent,303426032));
        actorComponentsFactory->RegisterObject<CameraModifierComponent>(ITF_GET_STRINGID_CRC(CameraModifierComponent,1821234798));
        actorComponentsFactory->RegisterObject<FixedCameraComponent>(ITF_GET_STRINGID_CRC(FixedCameraComponent,1176318211));

        // Misc
        actorComponentsFactory->RegisterObject<SubSceneComponent>(ITF_GET_STRINGID_CRC(SubSceneComponent,393783807));
        actorComponentsFactory->RegisterObject<SequencePlayerComponent>(ITF_GET_STRINGID_CRC(SequencePlayerComponent,899993551));
        actorComponentsFactory->RegisterObject<SequenceLauncherComponent>(ITF_GET_STRINGID_CRC(SequenceLauncherComponent,759529471));
        actorComponentsFactory->RegisterObject<SpawnManagerComponent>(ITF_GET_STRINGID_CRC(SpawnManagerComponent,502828938));
        actorComponentsFactory->RegisterObject<KeepAliveComponent>(ITF_GET_STRINGID_CRC(KeepAliveComponent,4149685283));
        actorComponentsFactory->RegisterObject<RelayEventComponent>(ITF_GET_STRINGID_CRC(RelayEventComponent,1922165291));
        actorComponentsFactory->RegisterObject<PrefetchComponent>(ITF_GET_STRINGID_CRC(PrefetchComponent,3811247622));
        actorComponentsFactory->RegisterObject<AABBPrefetchComponent>(ITF_GET_STRINGID_CRC(AABBPrefetchComponent,3064131908));
        actorComponentsFactory->RegisterObject<DummyComponent>(ITF_GET_STRINGID_CRC(DummyComponent,2957589102));
        actorComponentsFactory->RegisterObject<FlyingComponent>(ITF_GET_STRINGID_CRC(FlyingComponent,907001313));
    }

    /********* GAMEPLAY COMPONENTS ***********/
    void GameplayFactoryFiller::fillGameplayComponents()
    {
        ObjectFactory* actorComponentsFactory = ACTORSMANAGER->getActorComponentsFactory();

        // Graphics
        actorComponentsFactory->RegisterObject<RenderSimpleAnimComponent>(ITF_GET_STRINGID_CRC(RenderSimpleAnimComponent,448027741));
        actorComponentsFactory->RegisterObject<LightComponent>(ITF_GET_STRINGID_CRC(LightComponent,2312787560));

        // Physics
        actorComponentsFactory->RegisterObject<PhysComponent>(ITF_GET_STRINGID_CRC(PhysComponent,1286338096));
        actorComponentsFactory->RegisterObject<ParticlePhysComponent>(ITF_GET_STRINGID_CRC(ParticlePhysComponent,2772003471));
        actorComponentsFactory->RegisterObject<StickToPolylinePhysComponent>(ITF_GET_STRINGID_CRC(StickToPolylinePhysComponent,3089159513));
        actorComponentsFactory->RegisterObject<BounceOnPolylinePhysComponent>(ITF_GET_STRINGID_CRC(BounceOnPolylinePhysComponent,2164702966));

        // Player
        actorComponentsFactory->RegisterObject<CameraSubjectComponent>(ITF_GET_STRINGID_CRC(CameraSubjectComponent,2693571618));
        actorComponentsFactory->RegisterObject<CheckpointComponent>(ITF_GET_STRINGID_CRC(CheckpointComponent,1429523170));
        actorComponentsFactory->RegisterObject<EventReceivePlayAnimComponent>(ITF_GET_STRINGID_CRC(EventReceivePlayAnimComponent,3645229715));
        actorComponentsFactory->RegisterObject<CharacterDebuggerComponent>(ITF_GET_STRINGID_CRC(CharacterDebuggerComponent,3709650243));

        // AI
        actorComponentsFactory->RegisterObject<GenericAIComponent>(ITF_GET_STRINGID_CRC(GenericAIComponent,3282978206));
        actorComponentsFactory->RegisterObject<SimpleAIComponent>(ITF_GET_STRINGID_CRC(SimpleAIComponent,806807766));
        actorComponentsFactory->RegisterObject<WaypointComponent>(ITF_GET_STRINGID_CRC(WaypointComponent,375105077));
        actorComponentsFactory->RegisterObject<TpControlerComponent>(ITF_GET_STRINGID_CRC(TpControlerComponent,1679496276));
        actorComponentsFactory->RegisterObject<BTAIComponent>(ITF_GET_STRINGID_CRC(BTAIComponent,3955454657));
        actorComponentsFactory->RegisterObject<GroundAIControllerComponent>(ITF_GET_STRINGID_CRC(GroundAIControllerComponent,2338098180));
        actorComponentsFactory->RegisterObject<TimedSpawnerAIComponent>(ITF_GET_STRINGID_CRC(TimedSpawnerAIComponent,4093386385));

        // Environment
        actorComponentsFactory->RegisterObject<SoftPlatformComponent>(ITF_GET_STRINGID_CRC(SoftPlatformComponent,3322875823));
        actorComponentsFactory->RegisterObject<ProceduralSoftPlatformComponent>(ITF_GET_STRINGID_CRC(ProceduralSoftPlatformComponent,2575753601));
        actorComponentsFactory->RegisterObject<PolylineComponent>(ITF_GET_STRINGID_CRC(PolylineComponent,4280564469));
        actorComponentsFactory->RegisterObject<PointsCollisionComponent>(ITF_GET_STRINGID_CRC(PointsCollisionComponent,313321252));
        actorComponentsFactory->RegisterObject<SolidPolylineComponent>(ITF_GET_STRINGID_CRC(SolidPolylineComponent,1361568548));
        actorComponentsFactory->RegisterObject<PendulumComponent>(ITF_GET_STRINGID_CRC(PendulumComponent,3026488334));
        actorComponentsFactory->RegisterObject<AxisPolylineComponent>(ITF_GET_STRINGID_CRC(AxisPolylineComponent,1003966487));
        actorComponentsFactory->RegisterObject<RotatingPolylineComponent>(ITF_GET_STRINGID_CRC(RotatingPolylineComponent,4217284053));
        actorComponentsFactory->RegisterObject<HingePlatformComponent>(ITF_GET_STRINGID_CRC(HingePlatformComponent,3937122948));
        actorComponentsFactory->RegisterObject<ConstantMovementComponent>(ITF_GET_STRINGID_CRC(ConstantMovementComponent,3168343774));

        // FX
        actorComponentsFactory->RegisterObject<FXControllerComponent>(ITF_GET_STRINGID_CRC(FXControllerComponent,2370830262));
        actorComponentsFactory->RegisterObject<AfterFxControllerComponent>(ITF_GET_STRINGID_CRC(AfterFxControllerComponent,3876698127));
        actorComponentsFactory->RegisterObject<BoxInterpolatorComponent>(ITF_GET_STRINGID_CRC(BoxInterpolatorComponent,4111687898));
        actorComponentsFactory->RegisterObject<CircleInterpolatorComponent>(ITF_GET_STRINGID_CRC(CircleInterpolatorComponent,3129068326));
        actorComponentsFactory->RegisterObject<SpeedInputProviderComponent>(ITF_GET_STRINGID_CRC(SpeedInputProviderComponent,3980326790));
        actorComponentsFactory->RegisterObject<colorComputerComponent>(ITF_GET_STRINGID_CRC(colorComputerComponent,3495094582));

        //Misc
        actorComponentsFactory->RegisterObject<LinkComponent>(ITF_GET_STRINGID_CRC(LinkComponent,1144483611));
        actorComponentsFactory->RegisterObject<PlayerDetectorComponent>(ITF_GET_STRINGID_CRC(PlayerDetectorComponent,683675795));
        actorComponentsFactory->RegisterObject<DeathDetectorComponent>(ITF_GET_STRINGID_CRC(DeathDetectorComponent,4247132828));
        actorComponentsFactory->RegisterObject<PhantomDetectorComponent>(ITF_GET_STRINGID_CRC(PhantomDetectorComponent,2329854250));
        actorComponentsFactory->RegisterObject<CameraDetectorComponent>(ITF_GET_STRINGID_CRC(CameraDetectorComponent,1974096445));
        actorComponentsFactory->RegisterObject<TriggerComponent>(ITF_GET_STRINGID_CRC(TriggerComponent,1580214848));
        actorComponentsFactory->RegisterObject<AnimMarkerTriggerComponent>(ITF_GET_STRINGID_CRC(AnimMarkerTriggerComponent,1235306837));
        actorComponentsFactory->RegisterObject<PhantomTriggerComponent>(ITF_GET_STRINGID_CRC(PhantomTriggerComponent,2366929498));
        actorComponentsFactory->RegisterObject<TriggerOnButtonPressedComponent>(ITF_GET_STRINGID_CRC(TriggerOnButtonPressedComponent,3116106140));
        actorComponentsFactory->RegisterObject<SwarmComponent>(ITF_GET_STRINGID_CRC(SwarmComponent,2308967169));
        actorComponentsFactory->RegisterObject<GravityComponent>(ITF_GET_STRINGID_CRC(GravityComponent,3211267022));
        actorComponentsFactory->RegisterObject<WindComponent>(ITF_GET_STRINGID_CRC(WindComponent,1340424959));
        actorComponentsFactory->RegisterObject<TeleporterComponent>(ITF_GET_STRINGID_CRC(TeleporterComponent,1305761296));
        actorComponentsFactory->RegisterObject<TrailComponent>(ITF_GET_STRINGID_CRC(TrailComponent,2721209168));
        actorComponentsFactory->RegisterObject<Trail3DComponent>(ITF_GET_STRINGID_CRC(Trail3DComponent,2699037183));
        actorComponentsFactory->RegisterObject<LinkCurveComponent>(ITF_GET_STRINGID_CRC(LinkCurveComponent,3124044823));
        actorComponentsFactory->RegisterObject<PatchCurveComponent>(ITF_GET_STRINGID_CRC(PatchCurveComponent,3824839181));
        actorComponentsFactory->RegisterObject<AnimTriggeredComponent>(ITF_GET_STRINGID_CRC(AnimTriggeredComponent,240414993));
        actorComponentsFactory->RegisterObject<PlayAnimOnWeightChangeComponent>(ITF_GET_STRINGID_CRC(PlayAnimOnWeightChangeComponent,2409137217));
        actorComponentsFactory->RegisterObject<PlayAnimOnEventReceiveComponent>(ITF_GET_STRINGID_CRC(PlayAnimOnEventReceiveComponent,2859296107));
        actorComponentsFactory->RegisterObject<PlayAnimBasedOnBoneAngleComponent>(ITF_GET_STRINGID_CRC(PlayAnimBasedOnBoneAngleComponent,1943591819));
        actorComponentsFactory->RegisterObject<PlayAnimOnTouchPolylineComponent>(ITF_GET_STRINGID_CRC(PlayAnimOnTouchPolylineComponent,2276589626));
        actorComponentsFactory->RegisterObject<SynchronizedAnimComponent>(ITF_GET_STRINGID_CRC(SynchronizedAnimComponent,4229263636));
        actorComponentsFactory->RegisterObject<ActorSpawnComponent>(ITF_GET_STRINGID_CRC(ActorSpawnComponent,2920550788));
        actorComponentsFactory->RegisterObject<TimedSpawnerComponent>(ITF_GET_STRINGID_CRC(TimedSpawnerComponent,3330253882));
        actorComponentsFactory->RegisterObject<TimedSpawnerModifierComponent>(ITF_GET_STRINGID_CRC(TimedSpawnerModifierComponent,1576493977));
        actorComponentsFactory->RegisterObject<TODOComponent>(ITF_GET_STRINGID_CRC(TODOComponent,269527281));
        actorComponentsFactory->RegisterObject<ShapeComponent>(ITF_GET_STRINGID_CRC(ShapeComponent,1137022961));
        actorComponentsFactory->RegisterObject<PhantomComponent>(ITF_GET_STRINGID_CRC(PhantomComponent,726931488));
        actorComponentsFactory->RegisterObject<ArenaDoorComponent>(ITF_GET_STRINGID_CRC(ArenaDoorComponent,1615905805));
        actorComponentsFactory->RegisterObject<TweenComponent>(ITF_GET_STRINGID_CRC(TweenComponent,3047809396));
        actorComponentsFactory->RegisterObject<TrajectoryNodeComponent>(ITF_GET_STRINGID_CRC(TrajectoryNodeComponent,1976963548));
        actorComponentsFactory->RegisterObject<TrajectorySpawnerComponent>(ITF_GET_STRINGID_CRC(TrajectorySpawnerComponent,1653629382));
        actorComponentsFactory->RegisterObject<TrajectoryFollowerComponent>(ITF_GET_STRINGID_CRC(TrajectoryFollowerComponent,2852387072));
        actorComponentsFactory->RegisterObject<FriezeControllerComponent>(ITF_GET_STRINGID_CRC(FriezeControllerComponent,455692797));
        actorComponentsFactory->RegisterObject<ObjectControllerComponent>(ITF_GET_STRINGID_CRC(ObjectControllerComponent,1155118512));
        actorComponentsFactory->RegisterObject<SubsceneControllerComponent>(ITF_GET_STRINGID_CRC(SubsceneControllerComponent,1035544071));
        actorComponentsFactory->RegisterObject<AlwaysActiveActorComponent>(ITF_GET_STRINGID_CRC(AlwaysActiveActorComponent,3338470990));
        actorComponentsFactory->RegisterObject<CurveComponent>(ITF_GET_STRINGID_CRC(CurveComponent,1270663238));
        actorComponentsFactory->RegisterObject<CurveFollowerComponent>(ITF_GET_STRINGID_CRC(CurveFollowerComponent,3282771180));
        actorComponentsFactory->RegisterObject<AdditionalBehaviorsComponent>(ITF_GET_STRINGID_CRC(AdditionalBehaviorsComponent,239653273));
        actorComponentsFactory->RegisterObject<DeformOnTrajectoryComponent>(ITF_GET_STRINGID_CRC(DeformOnTrajectoryComponent,315423501));
        actorComponentsFactory->RegisterObject<VirtualLinkComponent>(ITF_GET_STRINGID_CRC(VirtualLinkComponent,2794321058));
        actorComponentsFactory->RegisterObject<RenderBezierPatchCurveComponent>(ITF_GET_STRINGID_CRC(RenderBezierPatchCurveComponent,2750093899));
        actorComponentsFactory->RegisterObject<BezierCurveComponent>(ITF_GET_STRINGID_CRC(BezierCurveComponent,1974194984));
        actorComponentsFactory->RegisterObject<FriezeContactDetectorComponent>(ITF_GET_STRINGID_CRC(FriezeContactDetectorComponent,715604426));
        actorComponentsFactory->RegisterObject<ToggleAnimOnEventComponent>(ITF_GET_STRINGID_CRC(ToggleAnimOnEventComponent,1085435387));
        actorComponentsFactory->RegisterObject<PoolActorComponent>(ITF_GET_STRINGID_CRC(PoolActorComponent,1247828894));
        actorComponentsFactory->RegisterObject<AtlasAnimationComponent>(ITF_GET_STRINGID_CRC(AtlasAnimationComponent,240098274));
        actorComponentsFactory->RegisterObject<SaveNotificationComponent>(ITF_GET_STRINGID_CRC(SaveNotificationComponent,4133785793));

        // UI
        actorComponentsFactory->RegisterObject<UIMenuItemText>(ITF_GET_STRINGID_CRC(UIMenuItemText,3255902461));
        actorComponentsFactory->RegisterObject<UITextBox>(ITF_GET_STRINGID_CRC(UITextBox,3507273453));
        actorComponentsFactory->RegisterObject<UIComponent>(ITF_GET_STRINGID_CRC(UIComponent,2232305413));
        actorComponentsFactory->RegisterObject<UIMenuPageComponent>(ITF_GET_STRINGID_CRC(UIMenuPageComponent,1580941451));
        actorComponentsFactory->RegisterObject<UIMenuItemComponent>(ITF_GET_STRINGID_CRC(UIMenuItemComponent,4006055000));
        actorComponentsFactory->RegisterObject<UIButtonComponent>(ITF_GET_STRINGID_CRC(UIButtonComponent,3136744194));
        actorComponentsFactory->RegisterObject<UISliderComponent>(ITF_GET_STRINGID_CRC(UISliderComponent,236227314));
        actorComponentsFactory->RegisterObject<UITextBoxesComponent>(ITF_GET_STRINGID_CRC(UITextBoxesComponent,282676810));
        actorComponentsFactory->RegisterObject<SimpleTextComponent>(ITF_GET_STRINGID_CRC(SimpleTextComponent,2170710125));
        actorComponentsFactory->RegisterObject<TutorialTextComponent>(ITF_GET_STRINGID_CRC(TutorialTextComponent,2460478030));
        actorComponentsFactory->RegisterObject<InGameTextComponent>(ITF_GET_STRINGID_CRC(InGameTextComponent,3492037591));
        actorComponentsFactory->RegisterObject<UIGameOptionComponent>(ITF_GET_STRINGID_CRC(UIGameOptionComponent,3059104641));
        actorComponentsFactory->RegisterObject<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent,3621365669));
        actorComponentsFactory->RegisterObject<UIToggleOptionComponent>(ITF_GET_STRINGID_CRC(UIToggleOptionComponent,3689192266));
        actorComponentsFactory->RegisterObject<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent,226609316));
        actorComponentsFactory->RegisterObject<UIProfileSlotComponent>(ITF_GET_STRINGID_CRC(UIProfileSlotComponent,1711782705));
        actorComponentsFactory->RegisterObject<UIActionButtonComponent>(ITF_GET_STRINGID_CRC(UIActionButtonComponent,1149467690));
        actorComponentsFactory->RegisterObject<ActiveBroadcastComponent>(ITF_GET_STRINGID_CRC(ActiveBroadcastComponent, 239394893));
    }

    /********* TEMPLATE COMPONENTS ***********/
    void GameplayFactoryFiller::fillTemplateComponents()
    {
        ObjectFactory* templatesFactory = TEMPLATEDATABASE->getComponentsFactory();

        // Templates
        templatesFactory->RegisterObject<UISliderComponent_Template>(ITF_GET_STRINGID_CRC(UISliderComponent_Template,3650720017));
        templatesFactory->RegisterObject<UIButtonComponent_Template>(ITF_GET_STRINGID_CRC(UIButtonComponent_Template,684890016));
        templatesFactory->RegisterObject<UIComponent_Template>(ITF_GET_STRINGID_CRC(UIComponent_Template,1440308778));
        templatesFactory->RegisterObject<UIMenuItemText_Template>(ITF_GET_STRINGID_CRC(UIMenuItemText_Template,3322807307));
        templatesFactory->RegisterObject<UIMenuPageComponent_Template>(ITF_GET_STRINGID_CRC(UIMenuPageComponent_Template,4026707220));
        templatesFactory->RegisterObject<UIMenuItemComponent_Template>(ITF_GET_STRINGID_CRC(UIMenuItemComponent_Template,780160883));
        templatesFactory->RegisterObject<UIGameOptionComponent_Template>(ITF_GET_STRINGID_CRC(UIGameOptionComponent_Template,1914604504));
        templatesFactory->RegisterObject<UIListOptionComponent_Template>(ITF_GET_STRINGID_CRC(UIListOptionComponent_Template,1371529000));
        templatesFactory->RegisterObject<UIToggleOptionComponent_Template>(ITF_GET_STRINGID_CRC(UIToggleOptionComponent_Template,3491665612));
        templatesFactory->RegisterObject<UIFloatOptionComponent_Template>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent_Template,443628752));
        templatesFactory->RegisterObject<UIProfileSlotComponent_Template>(ITF_GET_STRINGID_CRC(UIProfileSlotComponent_Template,4231517306));
        templatesFactory->RegisterObject<UIActionButtonComponent_Template>(ITF_GET_STRINGID_CRC(UIActionButtonComponent_Template,443268294));
        templatesFactory->RegisterObject<GraphicComponent_Template>(ITF_GET_STRINGID_CRC(GraphicComponent_Template,1900486610));
        templatesFactory->RegisterObject<TextureGraphicComponent_Template>(ITF_GET_STRINGID_CRC(TextureGraphicComponent_Template,2628666149));
        templatesFactory->RegisterObject<StandeeComponent_Template>(ITF_GET_STRINGID_CRC(StandeeComponent_Template,170543165));
		templatesFactory->RegisterObject<TextureGraphicComponent2D_Template>(ITF_GET_STRINGID_CRC(TextureGraphicComponent2D_Template,2729231470));
        templatesFactory->RegisterObject<AnimLightComponent_Template>(ITF_GET_STRINGID_CRC(AnimLightComponent_Template,2740286289));
        templatesFactory->RegisterObject<AnimatedComponent_Template>(ITF_GET_STRINGID_CRC(AnimatedComponent_Template,2655002388));
        templatesFactory->RegisterObject<AfterFxComponent_Template>(ITF_GET_STRINGID_CRC(AfterFxComponent_Template,2558007817));
        templatesFactory->RegisterObject<FxBankComponent_Template>(ITF_GET_STRINGID_CRC(FxBankComponent_Template,12983557));
        templatesFactory->RegisterObject<ParticleGeneratorComponent_Template>(ITF_GET_STRINGID_CRC(ParticleGeneratorComponent_Template,4010009333));
        templatesFactory->RegisterObject<PrefetchComponent_Template>(ITF_GET_STRINGID_CRC(PrefetchComponent_Template,3860101993));
        templatesFactory->RegisterObject<AABBPrefetchComponent_Template>(ITF_GET_STRINGID_CRC(AABBPrefetchComponent_Template,1890823098));
        templatesFactory->RegisterObject<DummyComponent_Template>(ITF_GET_STRINGID_CRC(DummyComponent_Template,2112894595));
        templatesFactory->RegisterObject<FlyingComponent_Template>(ITF_GET_STRINGID_CRC(FlyingComponent_Template,2120704391));
        templatesFactory->RegisterObject<RenderSimpleAnimComponent_Template>(ITF_GET_STRINGID_CRC(RenderSimpleAnimComponent_Template,2172854595));
        templatesFactory->RegisterObject<PhysComponent_Template>(ITF_GET_STRINGID_CRC(PhysComponent_Template,2661944379));
        templatesFactory->RegisterObject<ParticlePhysComponent_Template>(ITF_GET_STRINGID_CRC(ParticlePhysComponent_Template,3309029528));
        templatesFactory->RegisterObject<StickToPolylinePhysComponent_Template>(ITF_GET_STRINGID_CRC(StickToPolylinePhysComponent_Template,3898305065));
        templatesFactory->RegisterObject<BounceOnPolylinePhysComponent_Template>(ITF_GET_STRINGID_CRC(BounceOnPolylinePhysComponent_Template,419162262));
        templatesFactory->RegisterObject<CheckpointComponent_Template>(ITF_GET_STRINGID_CRC(CheckpointComponent_Template,260764834));
        templatesFactory->RegisterObject<EventReceivePlayAnimComponent_Template>(ITF_GET_STRINGID_CRC(EventReceivePlayAnimComponent_Template,2878807208));
        templatesFactory->RegisterObject<CharacterDebuggerComponent_Template>(ITF_GET_STRINGID_CRC(CharacterDebuggerComponent_Template,2200057270));
        templatesFactory->RegisterObject<WaypointComponent_Template>(ITF_GET_STRINGID_CRC(WaypointComponent_Template,2944183250));
        templatesFactory->RegisterObject<PolylineComponent_Template>(ITF_GET_STRINGID_CRC(PolylineComponent_Template,1921333574));
        templatesFactory->RegisterObject<PointsCollisionComponent_Template>(ITF_GET_STRINGID_CRC(PointsCollisionComponent_Template,1109051015));
        templatesFactory->RegisterObject<SoftPlatformComponent_Template>(ITF_GET_STRINGID_CRC(SoftPlatformComponent_Template,1141144583));
        templatesFactory->RegisterObject<ProceduralSoftPlatformComponent_Template>(ITF_GET_STRINGID_CRC(ProceduralSoftPlatformComponent_Template,2432053287));
        templatesFactory->RegisterObject<SolidPolylineComponent_Template>(ITF_GET_STRINGID_CRC(SolidPolylineComponent_Template,3737161728));
        templatesFactory->RegisterObject<PendulumComponent_Template>(ITF_GET_STRINGID_CRC(PendulumComponent_Template,1909095444));
        templatesFactory->RegisterObject<AxisPolylineComponent_Template>(ITF_GET_STRINGID_CRC(AxisPolylineComponent_Template,2334182358));
        templatesFactory->RegisterObject<RotatingPolylineComponent_Template>(ITF_GET_STRINGID_CRC(RotatingPolylineComponent_Template,2496154729));
        templatesFactory->RegisterObject<HingePlatformComponent_Template>(ITF_GET_STRINGID_CRC(HingePlatformComponent_Template,187693019));
        templatesFactory->RegisterObject<LinkComponent_Template>(ITF_GET_STRINGID_CRC(LinkComponent_Template,1680185781));
        templatesFactory->RegisterObject<SwarmComponent_Template>(ITF_GET_STRINGID_CRC(SwarmComponent_Template,1576593566));
        templatesFactory->RegisterObject<GravityComponent_Template>(ITF_GET_STRINGID_CRC(GravityComponent_Template,3412637300));
        templatesFactory->RegisterObject<WindComponent_Template>(ITF_GET_STRINGID_CRC(WindComponent_Template,3412567697));
        templatesFactory->RegisterObject<TeleporterComponent_Template>(ITF_GET_STRINGID_CRC(TeleporterComponent_Template,1759675961));
        templatesFactory->RegisterObject<AnimTriggeredComponent_Template>(ITF_GET_STRINGID_CRC(AnimTriggeredComponent_Template,1411916197));
        templatesFactory->RegisterObject<PlayAnimOnEventReceiveComponent_Template>(ITF_GET_STRINGID_CRC(PlayAnimOnEventReceiveComponent_Template,3614863395));
        templatesFactory->RegisterObject<TODOComponent_Template>(ITF_GET_STRINGID_CRC(TODOComponent_Template,1007411025));
        templatesFactory->RegisterObject<PlayAnimOnTouchPolylineComponent_Template>(ITF_GET_STRINGID_CRC(PlayAnimOnTouchPolylineComponent_Template,3958001507));
        templatesFactory->RegisterObject<SynchronizedAnimComponent_Template>(ITF_GET_STRINGID_CRC(SynchronizedAnimComponent_Template,1810829771));
        templatesFactory->RegisterObject<UITextBox_Template>(ITF_GET_STRINGID_CRC(UITextBox_Template,2138714152));
        templatesFactory->RegisterObject<UITextBoxesComponent_Template>(ITF_GET_STRINGID_CRC(UITextBoxesComponent_Template,565524987));
        templatesFactory->RegisterObject<SimpleTextComponent_Template>(ITF_GET_STRINGID_CRC(SimpleTextComponent_Template,1086261024));
        templatesFactory->RegisterObject<TutorialTextComponent_Template>(ITF_GET_STRINGID_CRC(TutorialTextComponent_Template,386828231));
        templatesFactory->RegisterObject<InGameTextComponent_Template>(ITF_GET_STRINGID_CRC(InGameTextComponent_Template,4085259041));
        templatesFactory->RegisterObject<TriggerComponent_Template>(ITF_GET_STRINGID_CRC(TriggerComponent_Template,249157773));
        templatesFactory->RegisterObject<AnimMarkerTriggerComponent_Template>(ITF_GET_STRINGID_CRC(AnimMarkerTriggerComponent_Template,1569578840));
        templatesFactory->RegisterObject<PhantomTriggerComponent_Template>(ITF_GET_STRINGID_CRC(PhantomTriggerComponent_Template,3505476799));
        templatesFactory->RegisterObject<TriggerOnButtonPressedComponent_Template>(ITF_GET_STRINGID_CRC(TriggerOnButtonPressedComponent_Template,1700547359));
        templatesFactory->RegisterObject<RelayEventComponent_Template>(ITF_GET_STRINGID_CRC(RelayEventComponent_Template,101416385));
        templatesFactory->RegisterObject<ConstantMovementComponent_Template>(ITF_GET_STRINGID_CRC(ConstantMovementComponent_Template,1235751057));
        templatesFactory->RegisterObject<DetectorComponent_Template>(ITF_GET_STRINGID_CRC(DetectorComponent_Template,2207592726));
        templatesFactory->RegisterObject<PhantomDetectorComponent_Template>(ITF_GET_STRINGID_CRC(PhantomDetectorComponent_Template,4113622071));
        templatesFactory->RegisterObject<PlayerDetectorComponent_Template>(ITF_GET_STRINGID_CRC(PlayerDetectorComponent_Template,982701186));
        templatesFactory->RegisterObject<DeathDetectorComponent_Template>(ITF_GET_STRINGID_CRC(DeathDetectorComponent_Template,1075865440));
        templatesFactory->RegisterObject<CameraDetectorComponent_Template>(ITF_GET_STRINGID_CRC(CameraDetectorComponent_Template,1965871294));
        templatesFactory->RegisterObject<LinkCurveComponent_Template>(ITF_GET_STRINGID_CRC(LinkCurveComponent_Template,3414317268));
        templatesFactory->RegisterObject<PatchCurveComponent_Template>(ITF_GET_STRINGID_CRC(PatchCurveComponent_Template,2155725626));
        templatesFactory->RegisterObject<PlayAnimOnWeightChangeComponent_Template>(ITF_GET_STRINGID_CRC(PlayAnimOnWeightChangeComponent_Template,1203810110));
        templatesFactory->RegisterObject<ShapeComponent_Template>(ITF_GET_STRINGID_CRC(ShapeComponent_Template,112285537));
        templatesFactory->RegisterObject<PhantomComponent_Template>(ITF_GET_STRINGID_CRC(PhantomComponent_Template,1750004224));
        templatesFactory->RegisterObject<ArenaDoorComponent_Template>(ITF_GET_STRINGID_CRC(ArenaDoorComponent_Template,3452134421));
        templatesFactory->RegisterObject<TrajectoryNodeComponent_Template>(ITF_GET_STRINGID_CRC(TrajectoryNodeComponent_Template,67924776));
        templatesFactory->RegisterObject<TrajectorySpawnerComponent_Template>(ITF_GET_STRINGID_CRC(TrajectorySpawnerComponent_Template,32697399));
        templatesFactory->RegisterObject<TweenComponent_Template>(ITF_GET_STRINGID_CRC(TweenComponent_Template,2864992789));
        templatesFactory->RegisterObject<TrajectoryFollowerComponent_Template>(ITF_GET_STRINGID_CRC(TrajectoryFollowerComponent_Template,1567319308));
        templatesFactory->RegisterObject<PlayAnimBasedOnBoneAngleComponent_Template>(ITF_GET_STRINGID_CRC(PlayAnimBasedOnBoneAngleComponent_Template,4264622728));
        templatesFactory->RegisterObject<KeepAliveComponent_Template>(ITF_GET_STRINGID_CRC(KeepAliveComponent_Template,2568491216));
        templatesFactory->RegisterObject<SequencePlayerComponent_Template>(ITF_GET_STRINGID_CRC(SequencePlayerComponent_Template,2333879612));
        templatesFactory->RegisterObject<SequenceLauncherComponent_Template>(ITF_GET_STRINGID_CRC(SequenceLauncherComponent_Template,1287162758));
        templatesFactory->RegisterObject<LightComponent_Template>(ITF_GET_STRINGID_CRC(LightComponent_Template,975033407));
        templatesFactory->RegisterObject<Trail3DComponent_Template>(ITF_GET_STRINGID_CRC(Trail3DComponent_Template,3314277959));
        templatesFactory->RegisterObject<TrailComponent_Template>(ITF_GET_STRINGID_CRC(TrailComponent_Template,3058571470));
        templatesFactory->RegisterObject<SpawnManagerComponent_Template>(ITF_GET_STRINGID_CRC(SpawnManagerComponent_Template,1136686137));
        templatesFactory->RegisterObject<ActorSpawnComponent_Template>(ITF_GET_STRINGID_CRC(ActorSpawnComponent_Template,3516031925));
        templatesFactory->RegisterObject<TimedSpawnerComponent_Template>(ITF_GET_STRINGID_CRC(TimedSpawnerComponent_Template,84813706));
        templatesFactory->RegisterObject<TimedSpawnerModifierComponent_Template>(ITF_GET_STRINGID_CRC(TimedSpawnerModifierComponent_Template,3567246828));
        templatesFactory->RegisterObject<FriezeControllerComponent_Template>(ITF_GET_STRINGID_CRC(FriezeControllerComponent_Template,1837670405));
        templatesFactory->RegisterObject<ObjectControllerComponent_Template>(ITF_GET_STRINGID_CRC(ObjectControllerComponent_Template,24176894));
        templatesFactory->RegisterObject<SimpleAIComponent_Template>(ITF_GET_STRINGID_CRC(SimpleAIComponent_Template,688798707));
        templatesFactory->RegisterObject<TimedSpawnerAIComponent_Template>(ITF_GET_STRINGID_CRC(TimedSpawnerAIComponent_Template,2720447795));
        templatesFactory->RegisterObject<GenericAIComponent_Template>(ITF_GET_STRINGID_CRC(GenericAIComponent_Template,3427293913));
        templatesFactory->RegisterObject<AlwaysActiveActorComponent_Template>(ITF_GET_STRINGID_CRC(AlwaysActiveActorComponent_Template,1432225983));
        templatesFactory->RegisterObject<CurveComponent_Template>(ITF_GET_STRINGID_CRC(CurveComponent_Template,449299892));
        templatesFactory->RegisterObject<CurveFollowerComponent_Template>(ITF_GET_STRINGID_CRC(CurveFollowerComponent_Template,4222421127));
        templatesFactory->RegisterObject<BTAIComponent_Template>(ITF_GET_STRINGID_CRC(BTAIComponent_Template,2431330779));
        templatesFactory->RegisterObject<GroundAIControllerComponent_Template>(ITF_GET_STRINGID_CRC(GroundAIControllerComponent_Template,1160833416));
        templatesFactory->RegisterObject<TpControlerComponent_Template>(ITF_GET_STRINGID_CRC(TpControlerComponent_Template,639472293));
        templatesFactory->RegisterObject<AdditionalBehaviorsComponent_Template>(ITF_GET_STRINGID_CRC(AdditionalBehaviorsComponent_Template,1452832865));
        templatesFactory->RegisterObject<DeformOnTrajectoryComponent_Template>(ITF_GET_STRINGID_CRC(DeformOnTrajectoryComponent_Template,3452429417));
        templatesFactory->RegisterObject<VirtualLinkComponent_Template>(ITF_GET_STRINGID_CRC(VirtualLinkComponent_Template,4122908951));
        templatesFactory->RegisterObject<FXControllerComponent_Template>(ITF_GET_STRINGID_CRC(FXControllerComponent_Template,238378088));
        templatesFactory->RegisterObject<AfterFxControllerComponent_Template>(ITF_GET_STRINGID_CRC(AfterFxControllerComponent_Template,1843163453));
        templatesFactory->RegisterObject<BoxInterpolatorComponent_Template>(ITF_GET_STRINGID_CRC(BoxInterpolatorComponent_Template,1912108630));
        templatesFactory->RegisterObject<CircleInterpolatorComponent_Template>(ITF_GET_STRINGID_CRC(CircleInterpolatorComponent_Template,1785323374));
        templatesFactory->RegisterObject<SpeedInputProviderComponent_Template>(ITF_GET_STRINGID_CRC(SpeedInputProviderComponent_Template,2751973800));
        templatesFactory->RegisterObject<colorComputerComponent_Template>(ITF_GET_STRINGID_CRC(colorComputerComponent_Template,1246862451));
        templatesFactory->RegisterObject<SoundComponent_Template>(ITF_GET_STRINGID_CRC(SoundComponent_Template,3645729875));
        templatesFactory->RegisterObject<SoundBusControllerComponent_Template>(ITF_GET_STRINGID_CRC(SoundBusControllerComponent_Template,2216764263));
        templatesFactory->RegisterObject<InGameCameraComponent_Template>(ITF_GET_STRINGID_CRC(InGameCameraComponent_Template,3647884283));
        templatesFactory->RegisterObject<CameraModifierComponent_Template>(ITF_GET_STRINGID_CRC(CameraModifierComponent_Template,3701615444));
        templatesFactory->RegisterObject<FixedCameraComponent_Template>(ITF_GET_STRINGID_CRC(FixedCameraComponent_Template,3300170984));
        templatesFactory->RegisterObject<SubSceneComponent_Template>(ITF_GET_STRINGID_CRC(SubSceneComponent_Template,3856437678));
        templatesFactory->RegisterObject<CameraSubjectComponent_Template>(ITF_GET_STRINGID_CRC(CameraSubjectComponent_Template,1531192225));
        templatesFactory->RegisterObject<MusicControllerComponent_Template>(ITF_GET_STRINGID_CRC(MusicControllerComponent_Template,4226307952));
        templatesFactory->RegisterObject<MoviePlayerComponent_Template>(ITF_GET_STRINGID_CRC(MoviePlayerComponent_Template,3807008301));
        templatesFactory->RegisterObject<RenderBezierPatchCurveComponent_Template>(ITF_GET_STRINGID_CRC(RenderBezierPatchCurveComponent_Template,2089577523));
        templatesFactory->RegisterObject<BezierCurveComponent_Template>(ITF_GET_STRINGID_CRC(BezierCurveComponent_Template,87532024));
        templatesFactory->RegisterObject<FriezeContactDetectorComponent_Template>(ITF_GET_STRINGID_CRC(FriezeContactDetectorComponent_Template,4035314567));
        templatesFactory->RegisterObject<ToggleAnimOnEventComponent_Template>(ITF_GET_STRINGID_CRC(ToggleAnimOnEventComponent_Template,1671062590));
        templatesFactory->RegisterObject<PoolActorComponent_Template>(ITF_GET_STRINGID_CRC(PoolActorComponent_Template,3923000469));
        templatesFactory->RegisterObject<AtlasAnimationComponent_Template>(ITF_GET_STRINGID_CRC(AtlasAnimationComponent_Template,2427973027));
        templatesFactory->RegisterObject<SaveNotificationComponent_Template>(ITF_GET_STRINGID_CRC(SaveNotificationComponent_Template,495222735));
        templatesFactory->RegisterObject<UIMenuSoundComponent_Template>(ITF_GET_STRINGID_CRC(UIMenuSoundComponent_Template,473615840));
        templatesFactory->RegisterObject<ActiveBroadcastComponent_Template>(ITF_GET_STRINGID_CRC(ActiveBroadcastComponent_Template, 4009445990));
    }

    /********* AI COMPONENTS ***********/
    void GameplayFactoryFiller::fillAIComponents()
    {
        ObjectFactory* behaviorFactory = ACTORSMANAGER->getAIBehaviorsFactory();
        ObjectFactory* aiActionsFactory = ACTORSMANAGER->getAIActionsFactory();

        /********* AI Behaviors Classes ***********/
        behaviorFactory->RegisterObject<AISimplePlayAnimBehavior_Template>(ITF_GET_STRINGID_CRC(AISimplePlayAnimBehavior_Template,3789495365));
        behaviorFactory->RegisterObject<AIDeathBehavior_Template>(ITF_GET_STRINGID_CRC(AIDeathBehavior_Template,2518331708));
        behaviorFactory->RegisterObject<AIRoamingBehavior_Template>(ITF_GET_STRINGID_CRC(AIRoamingBehavior_Template,3697341506));
        behaviorFactory->RegisterObject<AIPlayActionsBehavior_Template>(ITF_GET_STRINGID_CRC(AIPlayActionsBehavior_Template,3298039445));
        behaviorFactory->RegisterObject<AIReplicateParentAnimBehavior_Template>(ITF_GET_STRINGID_CRC(AIReplicateParentAnimBehavior_Template,1147033395));

        /********* AI Action Classes ***********/
        aiActionsFactory->RegisterObject<AIFadeAction_Template>(ITF_GET_STRINGID_CRC(AIFadeAction_Template,649375324));
        aiActionsFactory->RegisterObject<AISpawnAction_Template>(ITF_GET_STRINGID_CRC(AISpawnAction_Template,1094967091));
        aiActionsFactory->RegisterObject<AIFallAction_Template>(ITF_GET_STRINGID_CRC(AIFallAction_Template,2620008885));
        aiActionsFactory->RegisterObject<AIFallNoPhysAction_Template>(ITF_GET_STRINGID_CRC(AIFallNoPhysAction_Template,3237588225));
        aiActionsFactory->RegisterObject<AIFollowActorAction_Template>(ITF_GET_STRINGID_CRC(AIFollowActorAction_Template,1310157366));
        aiActionsFactory->RegisterObject<AIFollowCollideActorAction_Template>(ITF_GET_STRINGID_CRC(AIFollowCollideActorAction_Template,1575891110));
        aiActionsFactory->RegisterObject<AIIdleAction_Template>(ITF_GET_STRINGID_CRC(AIIdleAction_Template,1495714450));
        aiActionsFactory->RegisterObject<AIJumpAction_Template>(ITF_GET_STRINGID_CRC(AIJumpAction_Template,1081270361));
        aiActionsFactory->RegisterObject<AIJumpAngleAction_Template>(ITF_GET_STRINGID_CRC(AIJumpAngleAction_Template,3098797069));
        aiActionsFactory->RegisterObject<AIJumpInDirAction_Template>(ITF_GET_STRINGID_CRC(AIJumpInDirAction_Template,4120346116));
        aiActionsFactory->RegisterObject<AIPlayAnimAction_Template>(ITF_GET_STRINGID_CRC(AIPlayAnimAction_Template,3016991921));
        aiActionsFactory->RegisterObject<AIPlayFXAction_Template>(ITF_GET_STRINGID_CRC(AIPlayFXAction_Template,3661521510));
        aiActionsFactory->RegisterObject<AIStickBoneAction_Template>(ITF_GET_STRINGID_CRC(AIStickBoneAction_Template,1011191924));
        aiActionsFactory->RegisterObject<AIUTurnAction_Template>(ITF_GET_STRINGID_CRC(AIUTurnAction_Template,1794956752));
        aiActionsFactory->RegisterObject<AIWalkInDirAction_Template>(ITF_GET_STRINGID_CRC(AIWalkInDirAction_Template,3736858575));
        aiActionsFactory->RegisterObject<AIBallisticsApexAction_Template>(ITF_GET_STRINGID_CRC(AIBallisticsApexAction_Template,615420746));
        aiActionsFactory->RegisterObject<AIBallisticsFixedGravityAction_Template>(ITF_GET_STRINGID_CRC(AIBallisticsFixedGravityAction_Template,837751950));
        aiActionsFactory->RegisterObject<AIBezierAction_Template>(ITF_GET_STRINGID_CRC(AIBezierAction_Template,358550596));
        aiActionsFactory->RegisterObject<AIJumpToTargetAction_Template>(ITF_GET_STRINGID_CRC(AIJumpToTargetAction_Template,3165987287));
        aiActionsFactory->RegisterObject<AIBounceAction_Template>(ITF_GET_STRINGID_CRC(AIBounceAction_Template,1670937690));
        aiActionsFactory->RegisterObject<AIDestroyAction_Template>(ITF_GET_STRINGID_CRC(AIDestroyAction_Template,1880254611));
        aiActionsFactory->RegisterObject<AIDisableAction_Template>(ITF_GET_STRINGID_CRC(AIDisableAction_Template,1941849167));
        aiActionsFactory->RegisterObject<AIBumperAction_Template>(ITF_GET_STRINGID_CRC(AIBumperAction_Template,1033957686));
        aiActionsFactory->RegisterObject<AIBounceToLayerAction_Template>(ITF_GET_STRINGID_CRC(AIBounceToLayerAction_Template,4055762704));
        aiActionsFactory->RegisterObject<AISendEventAction_Template>(ITF_GET_STRINGID_CRC(AISendEventAction_Template,4032035978));
    }

    /********* SEQUENCE COMPONENTS ***********/
    void GameplayFactoryFiller::fillSequenceComponents()
    {
        ObjectFactory* sequenceEventsFactory = ACTORSMANAGER->getSequenceEventFactory();

        /********* Sequence Editor Events Classes ***********/
        sequenceEventsFactory->RegisterObject<ExecuteLUA_evtTemplate>(ITF_GET_STRINGID_CRC(ExecuteLUA_evtTemplate,3753561476));
        sequenceEventsFactory->RegisterObject<PlayAnim_evtTemplate>(ITF_GET_STRINGID_CRC(PlayAnim_evtTemplate,143171982));
        sequenceEventsFactory->RegisterObject<PlayLoop_evtTemplate>(ITF_GET_STRINGID_CRC(PlayLoop_evtTemplate,3712358567));
        sequenceEventsFactory->RegisterObject<PlaySkip_evtTemplate>(ITF_GET_STRINGID_CRC(PlaySkip_evtTemplate,173814531));
        sequenceEventsFactory->RegisterObject<Label_evtTemplate>(ITF_GET_STRINGID_CRC(Label_evtTemplate,3005583724));
        sequenceEventsFactory->RegisterObject<PlaySpawn_evtTemplate>(ITF_GET_STRINGID_CRC(PlaySpawn_evtTemplate,2135584656));
        sequenceEventsFactory->RegisterObject<PlayTrajectory_evtTemplate>(ITF_GET_STRINGID_CRC(PlayTrajectory_evtTemplate,2715452934));
        sequenceEventsFactory->RegisterObject<StartCapture_evtTemplate>(ITF_GET_STRINGID_CRC(StartCapture_evtTemplate,2073084928));
        sequenceEventsFactory->RegisterObject<PlayGameplay_evtTemplate>(ITF_GET_STRINGID_CRC(PlayGameplay_evtTemplate,1477606536));
        sequenceEventsFactory->RegisterObject<PlayWait_evtTemplate>(ITF_GET_STRINGID_CRC(PlayWait_evtTemplate,2722851120));
        sequenceEventsFactory->RegisterObject<PlaySound_evtTemplate>(ITF_GET_STRINGID_CRC(PlaySound_evtTemplate,2329430233));
        sequenceEventsFactory->RegisterObject<PlayText_evtTemplate>(ITF_GET_STRINGID_CRC(PlayText_evtTemplate,2006548793));
        sequenceEventsFactory->RegisterObject<PlayTextBanner_evtTemplate>(ITF_GET_STRINGID_CRC(PlayTextBanner_evtTemplate,304365072));
        sequenceEventsFactory->RegisterObject<RedirectPatchBank_evtTemplate>(ITF_GET_STRINGID_CRC(RedirectPatchBank_evtTemplate,2425993289));
    }

    /********* GAMEPLAY EVENT COMPONENTS ***********/
    void GameplayFactoryFiller::fillEventComponents()
    {
        ObjectFactory* gameplayEventFactory = ACTORSMANAGER->getEventFactory();

        // from Events.h
        gameplayEventFactory->RegisterObject<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
        gameplayEventFactory->RegisterObject<EventTeleport>(ITF_GET_STRINGID_CRC(EventTeleport,1075308886));
        gameplayEventFactory->RegisterObject<EventTeleportToActor>(ITF_GET_STRINGID_CRC(EventTeleportToActor,557779126));
        gameplayEventFactory->RegisterObject<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510));
        gameplayEventFactory->RegisterObject<EventReset>(ITF_GET_STRINGID_CRC(EventReset,2551104629));
        gameplayEventFactory->RegisterObject<EventActivateCheckpoint>(ITF_GET_STRINGID_CRC(EventActivateCheckpoint,1094365377));
        gameplayEventFactory->RegisterObject<EventSequenceControl>(ITF_GET_STRINGID_CRC(EventSequenceControl,1151213732));
        gameplayEventFactory->RegisterObject<EventSequenceActorPrepare>(ITF_GET_STRINGID_CRC(EventSequenceActorPrepare,3178520967));
        gameplayEventFactory->RegisterObject<EventSequenceActorSaveZ>(ITF_GET_STRINGID_CRC(EventSequenceActorSaveZ,2573269496));
        gameplayEventFactory->RegisterObject<EventSequenceSetPlayerPos>(ITF_GET_STRINGID_CRC(EventSequenceSetPlayerPos,3409872873));
        gameplayEventFactory->RegisterObject<EventSequenceSpeedChange>(ITF_GET_STRINGID_CRC(EventSequenceSpeedChange,3037544556));
        gameplayEventFactory->RegisterObject<EventSequenceActivatePlayers>(ITF_GET_STRINGID_CRC(EventSequenceActivatePlayers,3249842910));
        gameplayEventFactory->RegisterObject<EventSequenceActorActivate>(ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699));
        gameplayEventFactory->RegisterObject<EventSequenceActorReady>(ITF_GET_STRINGID_CRC(EventSequenceActorReady,1601154857));
        gameplayEventFactory->RegisterObject<EventWaitForActor>(ITF_GET_STRINGID_CRC(EventWaitForActor,866051242));
        gameplayEventFactory->RegisterObject<EventEndWait>(ITF_GET_STRINGID_CRC(EventEndWait,4251945611));
        gameplayEventFactory->RegisterObject<EventVideoCapture>(ITF_GET_STRINGID_CRC(EventVideoCapture,1922224351));
        gameplayEventFactory->RegisterObject<EventPause>(ITF_GET_STRINGID_CRC(EventPause,829919267));
        gameplayEventFactory->RegisterObject<EventAddForce>(ITF_GET_STRINGID_CRC(EventAddForce,1731368564));
        gameplayEventFactory->RegisterObject<EventSetSpeed>(ITF_GET_STRINGID_CRC(EventSetSpeed,2924911315));
        gameplayEventFactory->RegisterObject<EventUnstick>(ITF_GET_STRINGID_CRC(EventUnstick,54024986));
        gameplayEventFactory->RegisterObject<EventChildActorDestroyed>(ITF_GET_STRINGID_CRC(EventChildActorDestroyed,1818946457));
        gameplayEventFactory->RegisterObject<EventAnimChanged>(ITF_GET_STRINGID_CRC(EventAnimChanged,153009624));
        gameplayEventFactory->RegisterObject<EventSetBusVolume>(ITF_GET_STRINGID_CRC(EventSetBusVolume,1648440666));
        gameplayEventFactory->RegisterObject<EventSetBusReverb>(ITF_GET_STRINGID_CRC(EventSetBusReverb,98707735));
        gameplayEventFactory->RegisterObject<EventSetBusFilter>(ITF_GET_STRINGID_CRC(EventSetBusFilter,326081700));
        gameplayEventFactory->RegisterObject<EventDie>(ITF_GET_STRINGID_CRC(EventDie,1997554881));
        gameplayEventFactory->RegisterObject<EventRegisterCameraSubject>(ITF_GET_STRINGID_CRC(EventRegisterCameraSubject,1683052496));
        gameplayEventFactory->RegisterObject<EventCameraShake>(ITF_GET_STRINGID_CRC(EventCameraShake,458805059));
        gameplayEventFactory->RegisterObject<EventCameraIgnoreShake>(ITF_GET_STRINGID_CRC(EventCameraIgnoreShake,1831643083));
        gameplayEventFactory->RegisterObject<EventPadRumbleStart>(ITF_GET_STRINGID_CRC(EventPadRumbleStart,2869984648));
        gameplayEventFactory->RegisterObject<EventPadRumbleStop>(ITF_GET_STRINGID_CRC(EventPadRumbleStop,2252742105));
        gameplayEventFactory->RegisterObject<EventMetronomeSetBPM>(ITF_GET_STRINGID_CRC(EventMetronomeSetBPM,1145786736));
        gameplayEventFactory->RegisterObject<EventSetFloatInput>(ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645));
        gameplayEventFactory->RegisterObject<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903));
        gameplayEventFactory->RegisterObject<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687));
        gameplayEventFactory->RegisterObject<EventOnLink>(ITF_GET_STRINGID_CRC(EventOnLink,1558255516));
        gameplayEventFactory->RegisterObject<EventUndelaySpawn>(ITF_GET_STRINGID_CRC(EventUndelaySpawn,2219542589));
        gameplayEventFactory->RegisterObject<EventFluidCollisionPolylineChanged>(ITF_GET_STRINGID_CRC(EventFluidCollisionPolylineChanged,3871666563));
        gameplayEventFactory->RegisterObject<EventPlayMusic>(ITF_GET_STRINGID_CRC(EventPlayMusic,2305092816));
        gameplayEventFactory->RegisterObject<EventStopMusic>(ITF_GET_STRINGID_CRC(EventStopMusic,2288454954));
        gameplayEventFactory->RegisterObject<EventResetAfterFxAlpha>(ITF_GET_STRINGID_CRC(EventResetAfterFxAlpha,1923263133));
        gameplayEventFactory->RegisterObject<EventShowScoreboard>(ITF_GET_STRINGID_CRC(EventShowScoreboard,4168491256));
        gameplayEventFactory->RegisterObject<EventGoToMainMenu>(ITF_GET_STRINGID_CRC(EventGoToMainMenu,1087093414));
        gameplayEventFactory->RegisterObject<EventBusMix>(ITF_GET_STRINGID_CRC(EventBusMix,2930162672));
        gameplayEventFactory->RegisterObject<EventPlayMovie>(ITF_GET_STRINGID_CRC(EventPlayMovie,797811199));

        // from GameplayEvents.h
        gameplayEventFactory->RegisterObject<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601));
        gameplayEventFactory->RegisterObject<EventHangUpdate>(ITF_GET_STRINGID_CRC(EventHangUpdate,3049416002));
        gameplayEventFactory->RegisterObject<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076));
        gameplayEventFactory->RegisterObject<EventStickOnPolylineChangeEdge>(ITF_GET_STRINGID_CRC(EventStickOnPolylineChangeEdge,4222832239));
        gameplayEventFactory->RegisterObject<EventStickOnPolylineUpdate>(ITF_GET_STRINGID_CRC(EventStickOnPolylineUpdate,2702146949));
        gameplayEventFactory->RegisterObject<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768));
        gameplayEventFactory->RegisterObject<EventPolylineBlockingContact>(ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979));
        gameplayEventFactory->RegisterObject<EventWallJump>(ITF_GET_STRINGID_CRC(EventWallJump,2476933602));
        gameplayEventFactory->RegisterObject<EventRopeSwingImpulse>(ITF_GET_STRINGID_CRC(EventRopeSwingImpulse,807862862));
        gameplayEventFactory->RegisterObject<EventCollide>(ITF_GET_STRINGID_CRC(EventCollide,3122529699));
        gameplayEventFactory->RegisterObject<EventQueryPhysShape>(ITF_GET_STRINGID_CRC(EventQueryPhysShape,2080285933));
        gameplayEventFactory->RegisterObject<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608));
        gameplayEventFactory->RegisterObject<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623));
        gameplayEventFactory->RegisterObject<EventQueryPhysicsData>(ITF_GET_STRINGID_CRC(EventQueryPhysicsData,2528864682));
        gameplayEventFactory->RegisterObject<EventDetach>(ITF_GET_STRINGID_CRC(EventDetach,889341216));
        gameplayEventFactory->RegisterObject<EventTrail>(ITF_GET_STRINGID_CRC(EventTrail,2898640431));
        gameplayEventFactory->RegisterObject<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100));
        gameplayEventFactory->RegisterObject<EventShow>(ITF_GET_STRINGID_CRC(EventShow,111532112));
        gameplayEventFactory->RegisterObject<EventQuerySnapData>(ITF_GET_STRINGID_CRC(EventQuerySnapData,747808288));
        gameplayEventFactory->RegisterObject<EventQueryFaction>(ITF_GET_STRINGID_CRC(EventQueryFaction,3678638658));
        gameplayEventFactory->RegisterObject<EventQueryIsDead>(ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443));
        gameplayEventFactory->RegisterObject<EventQueryIsDisabled>(ITF_GET_STRINGID_CRC(EventQueryIsDisabled,1476509545));
        gameplayEventFactory->RegisterObject<EventQueryIsCaught>(ITF_GET_STRINGID_CRC(EventQueryIsCaught,1045544516));
        gameplayEventFactory->RegisterObject<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013));
        gameplayEventFactory->RegisterObject<EventForceOnActor>(ITF_GET_STRINGID_CRC(EventForceOnActor,267634505));
        gameplayEventFactory->RegisterObject<EventForceTransfer>(ITF_GET_STRINGID_CRC(EventForceTransfer,1043808691));
        gameplayEventFactory->RegisterObject<EventAnimUpdated>(ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305));
        gameplayEventFactory->RegisterObject<EventPhysicsUpdated>(ITF_GET_STRINGID_CRC(EventPhysicsUpdated,2632338894));
        gameplayEventFactory->RegisterObject<EventPlayerActivationChanged>(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208));
        gameplayEventFactory->RegisterObject<EventPlayerIndexChange>(ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152));
        gameplayEventFactory->RegisterObject<EventPlayerModeChanged>(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275));
        gameplayEventFactory->RegisterObject<EventPlayerModeSet>(ITF_GET_STRINGID_CRC(EventPlayerModeSet,722903700));
        gameplayEventFactory->RegisterObject<EventPlayerHpChanged>(ITF_GET_STRINGID_CRC(EventPlayerHpChanged,1555193400));
        gameplayEventFactory->RegisterObject<EventCheckpointReached>(ITF_GET_STRINGID_CRC(EventCheckpointReached,2346783043));
        gameplayEventFactory->RegisterObject<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082));
        gameplayEventFactory->RegisterObject<EventPushForce>(ITF_GET_STRINGID_CRC(EventPushForce,405779985));
        gameplayEventFactory->RegisterObject<EventSetPlayer>(ITF_GET_STRINGID_CRC(EventSetPlayer,4056921472));
        gameplayEventFactory->RegisterObject<EventZoneTrigger>(ITF_GET_STRINGID_CRC(EventZoneTrigger,173270600));
        gameplayEventFactory->RegisterObject<EventQueryCanStickOrCollide>(ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404));
        gameplayEventFactory->RegisterObject<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938));
        gameplayEventFactory->RegisterObject<EventSetTarget>(ITF_GET_STRINGID_CRC(EventSetTarget,4067373371));
        gameplayEventFactory->RegisterObject<EventSetTargetPos>(ITF_GET_STRINGID_CRC(EventSetTargetPos,2424671732));
        gameplayEventFactory->RegisterObject<EventDead>(ITF_GET_STRINGID_CRC(EventDead,634720568));
        gameplayEventFactory->RegisterObject<EventStopPlayer>(ITF_GET_STRINGID_CRC(EventStopPlayer,3369505273));
        gameplayEventFactory->RegisterObject<EventBouncedOnPolyline>(ITF_GET_STRINGID_CRC(EventBouncedOnPolyline,1421560006));
        gameplayEventFactory->RegisterObject<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380));
        gameplayEventFactory->RegisterObject<EventQueryCanReceiveHit>(ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316));
        gameplayEventFactory->RegisterObject<EventScaleChanged>(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374));
        gameplayEventFactory->RegisterObject<EventBossBubonHit>(ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172));
        gameplayEventFactory->RegisterObject<EventChangeBehaviourQuery>(ITF_GET_STRINGID_CRC(EventChangeBehaviourQuery,1120766952));
        gameplayEventFactory->RegisterObject<EventChangeExternBhvValidationQuery>(ITF_GET_STRINGID_CRC(EventChangeExternBhvValidationQuery,1163849318));
        gameplayEventFactory->RegisterObject<EventSetupBezier>(ITF_GET_STRINGID_CRC(EventSetupBezier,713993105));
        gameplayEventFactory->RegisterObject<EventVirtualLinkChildQuery>(ITF_GET_STRINGID_CRC(EventVirtualLinkChildQuery,940758855));
        gameplayEventFactory->RegisterObject<EventVirtualLinkBroadcast>(ITF_GET_STRINGID_CRC(EventVirtualLinkBroadcast,3923486039));
        gameplayEventFactory->RegisterObject<EventSkipSequence>(ITF_GET_STRINGID_CRC(EventSkipSequence,4247999814));
        gameplayEventFactory->RegisterObject<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874));
        gameplayEventFactory->RegisterObject<EventLockPlayers>(ITF_GET_STRINGID_CRC(EventLockPlayers,359572052));
        gameplayEventFactory->RegisterObject<EventQueryCanZoomOnTarget>(ITF_GET_STRINGID_CRC(EventQueryCanZoomOnTarget,4237975921));
        gameplayEventFactory->RegisterObject<EventForceStickOnEdge>(ITF_GET_STRINGID_CRC(EventForceStickOnEdge,3946845675));
        gameplayEventFactory->RegisterObject<EventQueryWaterInfluence>(ITF_GET_STRINGID_CRC(EventQueryWaterInfluence,1378586140));
        gameplayEventFactory->RegisterObject<EventTrajectorySpawn>(ITF_GET_STRINGID_CRC(EventTrajectorySpawn,3846946197));
        gameplayEventFactory->RegisterObject<EventTriggerBehaviour>(ITF_GET_STRINGID_CRC(EventTriggerBehaviour,2225168250));
        gameplayEventFactory->RegisterObject<EventOrangeContact>(ITF_GET_STRINGID_CRC(EventOrangeContact,4040257774));
        gameplayEventFactory->RegisterObject<EventPoolActorRegistration>(ITF_GET_STRINGID_CRC(EventPoolActorRegistration,3036909603));
        gameplayEventFactory->RegisterObject<EventPoolActorRecycle>(ITF_GET_STRINGID_CRC(EventPoolActorRecycle,1859888857));
        gameplayEventFactory->RegisterObject<EventTimedSpawnerDataSet>(ITF_GET_STRINGID_CRC(EventTimedSpawnerDataSet,436096372));
        gameplayEventFactory->RegisterObject<EventQueryIsSprinting>(ITF_GET_STRINGID_CRC(EventQueryIsSprinting,2326390223));
        gameplayEventFactory->RegisterObject<EventSetOriginalSender>(ITF_GET_STRINGID_CRC(EventSetOriginalSender,1174287572));
        gameplayEventFactory->RegisterObject<EventPreCheckpointSave>(ITF_GET_STRINGID_CRC(EventPreCheckpointSave,3321202665));

        // from GameplayStims.h
        gameplayEventFactory->RegisterObject<EventStim>(ITF_GET_STRINGID_CRC(EventStim,316939190));
        gameplayEventFactory->RegisterObject<EventStimNotify>(ITF_GET_STRINGID_CRC(EventStimNotify,1337973749));
        gameplayEventFactory->RegisterObject<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796));
        gameplayEventFactory->RegisterObject<WindStim>(ITF_GET_STRINGID_CRC(WindStim,2026477007));
        gameplayEventFactory->RegisterObject<SolidStim>(ITF_GET_STRINGID_CRC(SolidStim,3144684763));

        // from AnimationTrack.h
        gameplayEventFactory->RegisterObject<AnimFXEvent>(ITF_GET_STRINGID_CRC(AnimFXEvent,3753665187));
        gameplayEventFactory->RegisterObject<AnimAnimationEvent>(ITF_GET_STRINGID_CRC(AnimAnimationEvent,1183342651));
        gameplayEventFactory->RegisterObject<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301));
        gameplayEventFactory->RegisterObject<AnimPolylineEvent>(ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773));

        gameplayEventFactory->RegisterObject<EventGetPolylineConstrainedPosition>(ITF_GET_STRINGID_CRC(EventGetPolylineConstrainedPosition,3189215538));
        gameplayEventFactory->RegisterObject<EventDisplayText>(ITF_GET_STRINGID_CRC(EventDisplayText,3103222744));
        gameplayEventFactory->RegisterObject<EventActorActiveChanged>(ITF_GET_STRINGID_CRC(EventActorActiveChanged, 323621555));
    }

    /********* ANIM TREE COMPONENTS ***********/
    void GameplayFactoryFiller::fillAnimTreeComponents()
    {
        ObjectFactory* animTreeNodeFactory = ACTORSMANAGER->getAnimTreeNodeFactory();

        /********* AnimTree nodes ***********/
        animTreeNodeFactory->RegisterObject<BlendTreeNodeAddBranch_Template<AnimTreeResult> >(ITF_GET_STRINGID_CRC(BlendTreeNodeAddBranch_Template,575024261));
        animTreeNodeFactory->RegisterObject<BlendTreeNodeBlendBranches_Template<AnimTreeResult> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlendBranches_Template,3239327879));
        animTreeNodeFactory->RegisterObject<BlendTreeNodeChooseBranch_Template<AnimTreeResult> >(ITF_GET_STRINGID_CRC(BlendTreeNodeChooseBranch_Template,2519193777));
        animTreeNodeFactory->RegisterObject<BlendTreeNodeBranchTransition_Template<AnimTreeResult> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBranchTransition_Template,3100575532));
        animTreeNodeFactory->RegisterObject<AnimTreeNodePlayAnimInterpolateCursor_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodePlayAnimInterpolateCursor_Template,2482758920));
        animTreeNodeFactory->RegisterObject<AnimTreeNodePlayAnimProceduralSmooth_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodePlayAnimProceduralSmooth_Template,4008896927));
        animTreeNodeFactory->RegisterObject<AnimTreeNodePlayAnim_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodePlayAnim_Template,3928059907));
        animTreeNodeFactory->RegisterObject<AnimTreeNodePlayOnOff_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodePlayOnOff_Template,984447049));
        animTreeNodeFactory->RegisterObject<AnimTreeNodeMarchBlend_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodeMarchBlend_Template,1365303512));
        animTreeNodeFactory->RegisterObject<AnimTreeNodeBranchTransition_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodeBranchTransition_Template,806186784));
        animTreeNodeFactory->RegisterObject<AnimTreeNodeRandomBranch_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodeRandomBranch_Template,475419902));
        animTreeNodeFactory->RegisterObject<AnimTreeNodeSequence_Template>(ITF_GET_STRINGID_CRC(AnimTreeNodeSequence_Template,895279477));
    }

    /********* MUSIC TREE COMPONENTS ***********/
    void GameplayFactoryFiller::fillMusicTreeComponents()
    {
        ObjectFactory* musicTreeNodeFactory = ACTORSMANAGER->getMusicTreeNodeFactory();

        /********* MusicTree nodes ***********/
        musicTreeNodeFactory->RegisterObject<BlendTreeNodeChooseBranch_Template<MusicTreeResult> >(ITF_GET_STRINGID_CRC(BlendTreeNodeChooseBranch_Template,2519193777));
        musicTreeNodeFactory->RegisterObject<MusicTreeNodePlayMusic_Template>(ITF_GET_STRINGID_CRC(MusicTreeNodePlayMusic_Template,1746451784));
        musicTreeNodeFactory->RegisterObject<MusicTreeNodeSequence_Template>(ITF_GET_STRINGID_CRC(MusicTreeNodeSequence_Template,2816407103));
        musicTreeNodeFactory->RegisterObject<MusicTreeNodeRandom_Template>(ITF_GET_STRINGID_CRC(MusicTreeNodeRandom_Template,1249287778));
    }

    /********* BT COMPONENTS ***********/
    void GameplayFactoryFiller::fillBtComponents()
    {
        ObjectFactory* btNodesFactory = TEMPLATEDATABASE->getBTNodesFactory();

        // BT Nodes
        btNodesFactory->RegisterObject<BTDecider_Template>(ITF_GET_STRINGID_CRC(BTDecider_Template,528484444));
        btNodesFactory->RegisterObject<BTDeciderHasFact_Template>(ITF_GET_STRINGID_CRC(BTDeciderHasFact_Template,2356683879));
        btNodesFactory->RegisterObject<BTDeciderFactEqual_Template>(ITF_GET_STRINGID_CRC(BTDeciderFactEqual_Template,3776094096));
        btNodesFactory->RegisterObject<BTDeciderHasPlayerNear_Template>(ITF_GET_STRINGID_CRC(BTDeciderHasPlayerNear_Template,15924324));
        btNodesFactory->RegisterObject<BTDeciderHasPlayerBehind_Template>(ITF_GET_STRINGID_CRC(BTDeciderHasPlayerBehind_Template,2308490063));
        btNodesFactory->RegisterObject<BTDeciderHasActorsAlive_Template>(ITF_GET_STRINGID_CRC(BTDeciderHasActorsAlive_Template,59634142));
        btNodesFactory->RegisterObject<BTSequence_Template>(ITF_GET_STRINGID_CRC(BTSequence_Template,2865513351));
        btNodesFactory->RegisterObject<BTTimer_Template>(ITF_GET_STRINGID_CRC(BTTimer_Template,3783268492));
        btNodesFactory->RegisterObject<BTDelay_Template>(ITF_GET_STRINGID_CRC(BTDelay_Template,157870822));
        btNodesFactory->RegisterObject<BTActionPlayAnim_Template>(ITF_GET_STRINGID_CRC(BTActionPlayAnim_Template,2872294888));
        btNodesFactory->RegisterObject<BTActionWalkToTarget_Template>(ITF_GET_STRINGID_CRC(BTActionWalkToTarget_Template,2552012380));
        btNodesFactory->RegisterObject<BTActionStayIdle_Template>(ITF_GET_STRINGID_CRC(BTActionStayIdle_Template,2301954430));
        btNodesFactory->RegisterObject<BTActionStayIdleLookAt_Template>(ITF_GET_STRINGID_CRC(BTActionStayIdleLookAt_Template,760234255));
        btNodesFactory->RegisterObject<BTActionJumpToBack_Template>(ITF_GET_STRINGID_CRC(BTActionJumpToBack_Template,2731024669));
        btNodesFactory->RegisterObject<BTActionJumpToTarget_Template>(ITF_GET_STRINGID_CRC(BTActionJumpToTarget_Template,1525238737));
        btNodesFactory->RegisterObject<BTActionRemoveFact_Template>(ITF_GET_STRINGID_CRC(BTActionRemoveFact_Template,1011214178));
        btNodesFactory->RegisterObject<BTActionSetFact_Template>(ITF_GET_STRINGID_CRC(BTActionSetFact_Template,2150309077));
        btNodesFactory->RegisterObject<BTActionCopyFact_Template>(ITF_GET_STRINGID_CRC(BTActionCopyFact_Template,1389052205));
        btNodesFactory->RegisterObject<BTActionSendEventToActor_Template>(ITF_GET_STRINGID_CRC(BTActionSendEventToActor_Template,4110749335));
    }

    /********* CLASS COMPONENTS ***********/
    void GameplayFactoryFiller::fillClassComponents()
    {
        ObjectFactory* templatesClassFactory    = TEMPLATEDATABASE->getClassTemplateFactory();

        //Class Template factory
        templatesClassFactory->RegisterObject<GameManagerConfig_Template>(ITF_GET_STRINGID_CRC(GameManagerConfig_Template,2069144052));
        templatesClassFactory->RegisterObject<FactionManager_Template>(ITF_GET_STRINGID_CRC(FactionManager_Template,2119702796));
        templatesClassFactory->RegisterObject<FeedbackFXManager_Template>(ITF_GET_STRINGID_CRC(FeedbackFXManager_Template,152627411));
        templatesClassFactory->RegisterObject<Actor_Template>(ITF_GET_STRINGID_CRC(Actor_Template,461732814));
        templatesClassFactory->RegisterObject<CameraShakeConfig_Template>(ITF_GET_STRINGID_CRC(CameraShakeConfig_Template,73546132));
        templatesClassFactory->RegisterObject<Loading_Template>(ITF_GET_STRINGID_CRC(Loading_Template,675203100));
        templatesClassFactory->RegisterObject<SoundConfig_Template>(ITF_GET_STRINGID_CRC(SoundConfig_Template,2612383856));
        templatesClassFactory->RegisterObject<MusicManager_Template>(ITF_GET_STRINGID_CRC(MusicManager_Template,2068797502));
        templatesClassFactory->RegisterObject<PadRumbleManager_Template>(ITF_GET_STRINGID_CRC(PadRumbleManager_Template,3418702772));
        templatesClassFactory->RegisterObject<MetronomeManager_Template>(ITF_GET_STRINGID_CRC(MetronomeManager_Template,3007306876));
        templatesClassFactory->RegisterObject<ZInputManager_Template>(ITF_GET_STRINGID_CRC(ZInputManager_Template,3594148990));
        templatesClassFactory->RegisterObject<UITextManager_Template>(ITF_GET_STRINGID_CRC(UITextManager_Template,2855701572));
        templatesClassFactory->RegisterObject<ContextIconsManager_Template>(ITF_GET_STRINGID_CRC(ContextIconsManager_Template,543381722));
    }
}
