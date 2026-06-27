# Review Workflow

Use this prompt for code, docs, project, or architecture review.

## Rules

- Do not modify files unless explicitly commanded.
- Review from a bug/risk perspective first.
- Prioritize findings by severity.
- Cite files and line numbers where possible.
- Distinguish confirmed problems from assumptions.

## Scope

Review may include:

- C++ architecture and replication.
- Blueprint/content risks through MCP metadata.
- Documentation accuracy.
- Build and smoke-test gaps.
- Dirty worktree risks.
- Project organization.

## Output

Report:

- Findings first.
- Open questions.
- Recommended actions.
- Test/build gaps.

If no issues are found, say so directly and note residual risk.
