# Git Workflow Rules

1. **Vague & Simple Commit Messages**:
   - Keep commit messages short and vague (e.g. `make color transparent`, `add jump walk`, `update level`, `fix collision`). Avoid detailed conventional commit prefixes or long descriptions unless requested.

2. **Atomic Commits (One feature per commit)**:
   - Commit after every single feature or task step.
   - Never bundle multiple distinct changes into a single commit (e.g. do NOT combine `make color transparent` and `add jump walk`).

3. **No Automatic Pushing**:
   - Only run `git commit` locally.
   - Do NOT run `git push` unless the user explicitly asks to push.
