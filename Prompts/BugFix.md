# Bug Fix Workflow

Use this prompt when fixing a defect.

## Process

1. Reproduce or clearly characterize the problem.
2. Inspect logs, source, assets, docs, and MCP/editor state as appropriate.
3. Find the root cause before editing.
4. Apply the smallest fix that addresses the cause.
5. Avoid opportunistic refactors.
6. Preserve unrelated dirty changes.

## Verification

- Verify with logs, build, smoke test, or MCP checks depending on the bug.
- If the first fix fails, analyze the new evidence and iterate.
- Document any remaining risk or partial validation.

## Report

Include:

- Cause.
- Fix.
- Files changed.
- Verification performed.
- Remaining risks.
