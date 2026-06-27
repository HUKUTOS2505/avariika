# DECISIONS

Architectural decision record. `PROJECT_BRAIN.md` remains the canonical project description.

## ADR-0001 - `PROJECT_BRAIN.md` Is Canonical

Decision: the complete current project description lives in `PROJECT_BRAIN.md`.

Reason: older notes overlap and sometimes contain stale assumptions. Other root documents should link to the brain instead of repeating the whole project.

## ADR-0002 - Preserve Original Notes

Decision: historical documents remain in `Docs/Archive/Originals/`.

Reason: they contain audit trails, asset research, and design context. They are not automatically current truth.

## ADR-0003 - Do Not Move Unreal Folders

Decision: `Content/`, `Source/`, `Config/`, `Plugins/`, `Binaries/`, `Intermediate/`, and `Saved/` stay where Unreal expects them.

Reason: filesystem moves can break asset references, generated data, source layout, or active editor state.

## ADR-0004 - Use Unreal MCP For Editor Work

Decision: AI agents use Unreal MCP for editor/content operations.

Reason: MCP can query and modify Unreal state through editor APIs instead of unsafe binary file edits.

## ADR-0005 - Server-Authoritative Multiplayer

Decision: gameplay outcomes should be server-authoritative, with replicated or RPC-driven client feedback.

Reason: co-op stability is the top project priority, and many historical bugs were host/client desyncs.

## ADR-0006 - Animation Skeleton Convention

Decision: player locomotion work targets the UE mannequin-compatible WorkAnim/Mobility path documented in `PROJECT_BRAIN.md` and `Docs/Animation/ANIMATION_AND_LOCOMOTION.md`.

Reason: this path is the currently working route for Modular Workers/Quantum visuals and Mobility Pro locomotion assets.

## ADR-0007 - GAS Is Planned, Not Assumed

Decision: Gameplay Ability System is a project priority but should not be retrofitted without a design pass.

Reason: existing vitals, repair, item, and effect systems already exist. A GAS migration needs explicit boundaries to avoid duplicate state.

## ADR-0008 - UI Accent Is Orange

Decision: UI work should follow the orange accent direction and avoid purple UI.

Reason: this is an explicit project convention.

## ADR-0009 - Listener Monster Deferred

Decision: do not implement the Listener Monster without explicit instruction.

Reason: the project currently prioritizes systems, movement, co-op stability, and atmosphere before monster implementation.

## ADR-0010 - Cherry-Pick Heavy Frameworks

Decision: large marketplace frameworks, weather systems, and asset packs should be cherry-picked instead of wholesale integrated.

Reason: the project already has custom C++ systems, co-op state, HUD, and performance constraints. Full frameworks can create duplicate authority and bloat.

