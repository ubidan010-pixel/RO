# P0 — Platformer Perception Expansion (Platformer Bot)

Context: Perception for platforming is minimal. Missing sensors for ground-ahead, edges/gaps, walls, moving platforms, hazards, affordances (hang/climb/helico), grounded time, and coyote-time.

Goal
- Provide robust perception signals so AI can make reliable jump/route decisions and avoid hazards, enabling better heuristics now and data collection later.

Scope
- Update `PerceptionModule` and `GameState` to compute and expose richer terrain/affordance signals.
- Keep changes contained to gameplay AI perception; no core physics refactor.

Out of Scope
- Advanced learning/training pipeline (logging is optional integration).
- Full navigation graph or pathfinding.

Deliverables
- Extended `GameState` with terrain/affordance fields.
- `PerceptionModule` raycast-based probes for ground-ahead, wall, gap; moving platform and hazard sensing; grounded timers and coyote-time tracking.
- Debug overlay for new sensors.
- (Optional) Telemetry hooks to persist new fields.

Implementation Checklist
- [x] Data model
  - [x] Add `TerrainSensors` struct in `gameplay/AI/PlayerBot/PerceptionModule.h` with fields:
    - `groundAheadDist{near,mid,far}` (f32), `hasGroundAhead{near,mid,far}` (bool)
    - `gapDepth` (f32), `frontWall` (bool), `frontWallHeight` (f32)
    - `slopeAhead` (f32), `ledgeAbove` (bool)
    - `movingPlatformBelow` (bool), `movingPlatformVel` (Vec2d)
    - `hazardAhead` (bool), `hazardDist` (f32), `hazardType` (u32/tag)
    - `groundedFrames` (u32), `airborneFrames` (u32)
    - `coyoteTimeRemaining` (f32)
  - [x] Extend `GameState` to include `TerrainSensors sensors`.

- [x] Raycast probes (environment sensing)
  - [x] Use `PhysWorld::rayCastEnvironment` to measure ground-ahead at multiple X offsets (near/mid/far) and compute hit distance.
  - [x] Compute `hasGroundAhead*`, `groundAheadDist*`, and infer `gapDepth` when no hit in a vertical window.
  - [x] Detect `frontWall` via forward probes with edge orientation + step-height threshold; report refined `frontWallHeight`.
  - [x] Estimate `slopeAhead` (approx via dy/dx from two nearby ground samples).

- [x] Moving platform detection
  - [x] When grounded, identify contact body as platform; infer `movingPlatformBelow` and `movingPlatformVel` from actor/phys component if available.

- [x] Hazard sensing
  - [x] Define hazard classification using game material danger levels (spikes/fire/etc.).
  - [x] Probe forward arc (few rays) to set `hazardAhead`, `hazardDist`, `hazardType` using filters/tags.

- [ ] Affordances
  - [ ] Expose booleans: `canHang`, `canClimb`, `canHelico` based on stance, proximity to hangable edges/walls, and ability rules.
  - [ ] Add ledge detection (`ledgeAbove`) via short upward ray near a front wall.

- [x] Grounding and coyote-time
  - [x] Track `groundedFrames`/`airborneFrames` counters every update.
  - [x] Implement configurable `coyoteTime` window; update `coyoteTimeRemaining` when leaving ground.
  - [x] Add config values (probe offsets, coyote time) as PerceptionModule members (or a small config struct) with sensible defaults.

- [x] Integration
  - [x] Populate `GameState.sensors` in `PerceptionModule::update` (new `extractTerrainSensors()` helper).
  - [x] Keep current callbacks (`stance`, `scanTargets`) intact.
  - [ ] Ensure no behavior regressions when bot disabled.

- [x] Debug overlay
  - [x] Extend existing overlay (`PerceptionModule::debugDraw`) to show key fields: groundAhead*, frontWall, gapDepth, platform, hazard, coyote.

- [ ] Optional: Telemetry
  - [ ] If a logger exists (or later), include new sensor fields in per-tick records for training/analysis.

Acceptance Criteria
- [ ] Overlay displays all new sensor values correctly while moving through a level.
- [ ] Bot heuristics (even simple) can avoid walking off obvious gaps and can decide to jump over front walls using these signals.
- [ ] No crashes or significant frame-time regressions in levels with many colliders.

Dependencies / Notes
- Use `engine/physics/PhysWorld::rayCastEnvironment` for probes; confirm proper collision filters for ground vs. hazards.
- Hazard classification will rely on existing components/tags (e.g., spikes, fire/lighting patches, water). Start with a minimal set and expand as needed.
- Keep sensor sampling lightweight (3–5 rays/frame typical). Consider caching or spacing far probe cadence if needed.

Ownership
- Primary: Gameplay/AI Platformer
- Review: Physics/Gameplay Integrators

Status
- In progress
