# Build Rules

See [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md) for project context and [BUILD_PIPELINE.md](../Architecture/BUILD_PIPELINE.md) for the full build flow.

- Documentation-only tasks do not require Unreal Build Tool unless source/config changed.
- C++ tasks must build `avariikaEditor` and iterate until clean.
- Regenerate project files only when target/module/plugin structure changes.
- For multiplayer-sensitive work, verify with at least a listen-server or PIE-2 smoke test.
- Never commit a broken build.
