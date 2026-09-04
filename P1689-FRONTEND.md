# P1689 frontend work — forward-port onto `main` (in progress)

This branch is an **incomplete** forward-port of the P1689 frontend work onto LLVM
`main`. It currently contains only:

- the extraction of `clang/lib/DependencyAnalysis` out of `clang/lib/Frontend`, and
- the `-fdeps*` driver options.

The P1689 emitters are **not** here yet.

**The complete implementation is on
[`p1689-frontend`](https://github.com/silvanshade/llvm-p1689-deps/tree/p1689-frontend)**
(based on `release/20.x`), which includes `P1689DataCollector`,
`P1689DataFormatter`, `P1689FileGenerator` and the driver plumbing. Read that branch
and its `P1689-FRONTEND.md` first; this one exists to track the rebase.

- **Base:** `7162f191b8ce` (LLVM `main`, 2025-07-29)
- **Diff:** [`7162f191b8ce...p1689-frontend-main-wip`](https://github.com/silvanshade/llvm-p1689-deps/compare/7162f191b8ce...p1689-frontend-main-wip)
